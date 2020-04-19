#include "link_manager.h"
#include "download_item_updater.h"
#include "../model/download_item.h"
#include "../settings/finished_downloads_settings.h"
#include <meta_object_ext.h>
#include <QFile>
#include <QDataStream>
#include <QDirIterator>
#include <qdebugex.h>

link_manager::link_manager(QNetworkAccessManager * global_nam, QObject * parent)
  :QObject(parent),
   curr_links_group_idx(0),
   nam(global_nam),
   link_checker_in_use(false),
   max_parallel_link_checks(5),
   max_ms_before_check_link_again_on_error(10 * 1000),
   use_gdrive_api(false)
{
}

link_manager::~link_manager()
{
  if(link_checker)
    delete link_checker;

  qDeleteAll(di_updaters);
}

void link_manager::load_state(const QString & download_dir, const QString & download_lists_dir, const finished_downloads_settings & fin_dl_settings)
{
  download_queue.set_download_lists_directory(download_lists_dir);
  download_queue.load_state(fin_dl_settings);
  load_segments_ends(download_dir);

  if(load_links_from_file())
  {
    check_links_on_server();
  }
  else
  {
    const QString err = "Content from file " + links_file_path + " not loaded";
    qDebug() << err;
    links_in_groups.clear();
    emit load_state_finished(err);
  }
}

void link_manager::save_state(const QString & download_dir)
{
  save_segments_ends(download_dir);
}

void link_manager::update_download_item(download_item * item)
{
  qDebug() << "Updating download item:" << item->get_link();

  auto * updater = new download_item_updater(max_ms_before_check_link_again_on_error, nam);
  di_updaters.insert(updater);
  connect(updater, &download_item_updater::update_finished, this, &link_manager::download_item_updater_finished);

  updater->start_update(item);
}

void link_manager::download_item_updater_finished(download_item_updater * sender, service::fetch_error error_code, const QString & error_text, download_item * item)
{
  emit update_download_item_finished(error_code, error_text, item);

  sender->deleteLater();
  di_updaters.remove(sender);
}

bool link_manager::load_links_from_file()
{
  QFile file(links_file_path);
  if(file.open(QIODevice::ReadOnly) == false)
  {
    qDebug() << "Could not open file for read:" << links_file_path;
    return false;
  }

  const QString content = QString::fromUtf8(file.readAll());
  file.close();

  parse_file_content(content);
  return true;
}

downloadable_items & link_manager::get_download_queue()
{
  return download_queue;
}

void link_manager::set_links_file_dir(const QString & dir)
{
  links_file_dir = dir;
  links_file_path = links_file_dir + "/" + links_file_name;
}

void link_manager::set_max_parallel_link_checks(int max)
{
  max_parallel_link_checks = max;
}

void link_manager::set_max_seconds_before_check_link_again_on_error(int seconds)
{
  max_ms_before_check_link_again_on_error = seconds * 1000;
}

void link_manager::set_use_gdrive_api(bool gdrive_api_enabled)
{
  use_gdrive_api = gdrive_api_enabled;
}

void link_manager::abort_dl_item_updater_connections()
{
  for(auto * updater : di_updaters)
  {
    download_item * item = updater->get_updatable_item();
    if(item)
      download_queue.stop(item);

    delete updater;
  }

  di_updaters.clear();
}

void link_manager::parse_file_content(const QString & content)
{
  const QStringList lines = content.split(QChar::LineFeed, QString::SkipEmptyParts);

  QStringList links;
  for(const auto & line : lines)
  {
    QString simplified_line = line;
    // skip commments started with #
    const int hash_pos = simplified_line.indexOf('#');
    if(hash_pos != -1)
      simplified_line = simplified_line.left(hash_pos);

    simplified_line = simplified_line.simplified();

    if(simplified_line.isEmpty() == false)
    {
      if(simplified_line != "--")
      {
        const QString decoded_uri = QUrl::fromPercentEncoding(simplified_line.toUtf8());
        if(validate_link(decoded_uri))
          links.append(decoded_uri);
      }
      else
      {
        if(links.isEmpty() == false)
        {
          links_in_groups.append(links);
          links.clear();
        }
      }
    }
  }

  if(links.isEmpty() == false)
    links_in_groups.append(links);
}

bool link_manager::validate_link(const QString & link)
{
  const QUrl url(link, QUrl::StrictMode);

  if(url.isValid() == false)
  {
    qDebug() << "Link is invalid:" << link << "file:" << links_file_path << "-> Skipping";
    return false;
  }

  if(url.host().endsWith(".zippyshare.com") == false
     && url.host().endsWith("megaup.net") == false
     && url.host().endsWith("drive.google.com") == false)
  {
    qDebug() << "Link domain:" << url.host() << "is not allowed. Link:" << link << "file:" << links_file_path << "-> Skipping";
    return false;
  }

  if(url.scheme() != "https")
  {
    qDebug() << "Link has NO HTTPS scheme:" << link << "file:" << links_file_path << "-> Skipping";
    return false;
  }

  if(download_queue.contains_link(link))
  {
    qDebug() << "Download queue already contains link:" << link << "file:" << links_file_path << "-> Skipping";
    return false;
  }

  return true;
}

void link_manager::check_links_on_server()
{
  if(link_checker_in_use)
  {
    qDebug() << "Link checker already in use. Skipping check links from file on server.";
    return;
  }

  link_checker_in_use = true;

  link_checker = new multi_link_checker(nam);
  link_checker->set_max_parallel_link_checks(max_parallel_link_checks);
  link_checker->set_max_ms_before_check_link_again_on_error(max_ms_before_check_link_again_on_error);
  link_checker->set_use_gdrive_api(use_gdrive_api);
  link_checker->set_zippyshare_fetch_file_size(true);

  connect(link_checker, &multi_link_checker::download_links_info_success, this, &link_manager::check_links_on_server_success);
  connect(link_checker, &multi_link_checker::download_error, this, &link_manager::check_links_on_server_error);

  curr_links_group_idx = 0;
  check_next_links_on_server();
}

void link_manager::check_next_links_on_server()
{
  if(curr_links_group_idx < links_in_groups.size())
  {
    link_checker->download_links_info(links_in_groups.at(curr_links_group_idx));
  }
  else
  {
    move_links_file();

    link_checker->deleteLater();
    link_checker_in_use = false;
    links_in_groups.clear();
    emit load_state_finished();
  }
}

void link_manager::check_links_on_server_success(const QList<download_item> & infos)
{
  ++curr_links_group_idx;

  download_queue.sort_and_add(infos);

  meta_object_ext::invoke_async(this, &link_manager::check_next_links_on_server);
}

void link_manager::check_links_on_server_error(service::fetch_error /*error_code*/, const QString & error_text)
{
  qDebug() << "LINK CHECKER: skipped one link. Error:" << error_text;

  ++curr_links_group_idx;
  meta_object_ext::invoke_async(this, &link_manager::check_next_links_on_server);
}

void link_manager::move_links_file()
{
  const QString new_links_filename = "old_links.txt";

  QFile::remove(links_file_dir + "/" + new_links_filename);
  QFile::rename(links_file_path, links_file_dir + "/" + new_links_filename);
}

void link_manager::load_segments_ends(const QString & download_dir)
{
  QDirIterator it(download_dir, QStringList() << "*.zdm", QDir::Files);
  while(it.hasNext())
  {
    QVector<qint64> saved_ends;
    qint64 remaining_bytes = -1;
    QFile file(it.next());
    if(file.open(QIODevice::ReadOnly))
    {
      QDataStream in_data(&file);
      in_data.setVersion(QDataStream::Qt_5_14);
      in_data >> saved_ends;
      in_data >> remaining_bytes;
      if(in_data.status() != QDataStream::Ok)
        remaining_bytes = -1;
    }
    else
    {
      qDebug() << "Could not open file" << file.fileName() << "for read";
    }

    const QString dl_filename = it.fileInfo().completeBaseName();
    download_queue.set_item_segment_ends_by_filename(dl_filename, saved_ends, remaining_bytes);

    if(file.remove() == false)
      qDebug() << "Could not remove file" << file.fileName();

    //qDebug() << "LOAD SEGMENT OFFSETS:" << saved_ends << "REMAINING BYTES:" << remaining_bytes << dl_filename;
  }
}

void link_manager::save_segments_ends(const QString & download_dir) const
{
  const auto seg_items = download_queue.get_segmented_items();
  for(const auto item : seg_items)
  {
    const QString meta_filename = download_dir + "/" + item->get_filename() + ".zdm";
    QFile file(meta_filename);
    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
    {
      qDebug() << "Could not open file" << meta_filename << "for write";
      return;
    }

    QDataStream outds(&file);
    outds.setVersion(QDataStream::Qt_5_14);
    // Write the data
    outds << item->get_segment_ends();
    outds << item->get_remaining_bytes();

    //qDebug() << "SAVE SEGMENT OFFSETS:" << item->get_segment_ends() << "REMAINING BYTES:" << item->get_remaining_bytes() << item->get_filename();
  }
}
