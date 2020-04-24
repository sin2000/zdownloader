#include "downloadable_items.h"
#include "download_item.h"
#include "../settings/finished_downloads_settings.h"
#include "../utils/archive_filename_comparator.h"
#include <zd_logger.h>
#include <QFile>
#include <QDateTime>
#include <qdebugex.h>

downloadable_items::downloadable_items()
  :save_file(nullptr),
   fin_dls_logger(nullptr),
   max_group_id(0)
{
  set_download_lists_directory(".");
}

downloadable_items::~downloadable_items()
{
  if(fin_dls_logger)
  {
    fin_dls_logger->invoke_disable(false);
    zd_logger::inst().text_log_mgr->remove_object(finished_downloads_base_fn);
  }

  delete save_file;
  clear();
}

void downloadable_items::load_state(const finished_downloads_settings & fin_set)
{
  if(fin_set.log_to_file_enabled)
  {
    fin_dls_logger = zd_logger::inst().text_log_mgr->add_object(finished_downloads_base_fn);
    fin_dls_logger->invoke_set_timestamp_format(fin_set.timestamp_format);
    fin_dls_logger->invoke_set_rotate_settings(fin_set.log_max_archive_files, fin_set.log_max_rotate_kb * 1024);
    fin_dls_logger->invoke_enable(download_lists_directory, finished_downloads_base_fn, false);
  }

  load_from_file();

  save_file = new QFile(download_list_file_path);
  if(save_file->open(QIODevice::ReadWrite | QIODevice::Text) == false)
    qDebug() << "Failed to open file for Read&Write:" << download_list_file_path;
}

void downloadable_items::sort_and_add(const QList<download_item> & items)
{
  QList<download_item> sorted_items = items;
  std::sort(sorted_items.begin(), sorted_items.end(), [](const download_item & x, const download_item & y)
  {
    return x.get_filename() < y.get_filename();
  });

  if(max_group_id == std::numeric_limits<int>().max())
    max_group_id = 0;

  archive_filename_comparator archive_cmp;
  int curr_gid = ++max_group_id;
  for(int i = 0; i < sorted_items.size(); ++i)
  {
    download_item & curr_fi = sorted_items[i];
    if(i > 0)
    {
      const QString prev_fn = sorted_items.at(i - 1).get_filename();
      if(archive_cmp.compare(prev_fn, curr_fi.get_filename()) == false)
        curr_gid = ++max_group_id;
    }

    curr_fi.set_group_id(curr_gid);
  }

  link_map.reserve(link_map.size() + sorted_items.size());

  for(const auto & item : sorted_items)
    add(item);

  save_to_file();
}

void downloadable_items::add(const download_item & item)
{
  if(filename_map.contains(item.get_filename()))
  {
    qDebug() << "Item not added. Filename" << item.get_filename() << "already exists in filename_map";
    return;
  }

  if(link_map.contains(item.get_link()))
  {
    qDebug() << "Item not added. Link" << item.get_link() << "already exists in link_map. File" << item.get_filename();
    return;
  }

  const auto item_ptr = new download_item(item);

  pending_items.enqueue(item_ptr);
  all_items.append(item_ptr);

  filename_map.insert(item_ptr->get_filename(), item_ptr);
  link_map.insert(item_ptr->get_link(), item_ptr);

  const auto it = items_left_in_group_map.find(item.get_group_id());
  if(it == items_left_in_group_map.end())
  {
    items_left_in_group_map.insert(item.get_group_id(), 1);
  }
  else
  {
    ++(it.value());
  }
}

void downloadable_items::clear()
{
  filename_map.clear();
  link_map.clear();
  items_left_in_group_map.clear();

  qDeleteAll(all_items);
  all_items.clear();
  pending_items.clear();
  working_items.clear();
  paused_items.clear();
  max_group_id = 0;
}

bool downloadable_items::has_pending_items() const
{
  return pending_items.isEmpty() == false;
}

bool downloadable_items::has_working_items() const
{
  return working_items.isEmpty() == false;
}

download_item * downloadable_items::get_next()
{
  auto * next_item = const_cast<download_item *>(pending_items.dequeue());
  working_items.append(next_item);

  return next_item;
}

void downloadable_items::pause(download_item * item)
{
  if(remove_working_item(item))
  {
    pending_items.removeOne(item);
    paused_items.append(item);
  }
}

void downloadable_items::resume_first_paused()
{
  if(paused_items.isEmpty())
    return;

  pending_items.prepend(paused_items.first());
  paused_items.removeFirst();
}

QString downloadable_items::finish(download_item * item)
{
  QString last_item_in_group_fn;

  if(remove_working_item(item))
  {
    if(item->get_status() == download_item::download_status_pending)
      item->set_status(download_item::download_status_finished);
    if(fin_dls_logger)
      fin_dls_logger->invoke_log_text(QDateTime::currentMSecsSinceEpoch(), item->to_string_finished());

    all_items.removeOne(item);
    save_to_file();

    const auto it = items_left_in_group_map.find(item->get_group_id());
    if(it != items_left_in_group_map.end())
    {
      int & items_left_in_group = it.value();
      --items_left_in_group;
      if(items_left_in_group == 0)
      {
        last_item_in_group_fn = item->get_filename();
        items_left_in_group_map.remove(item->get_group_id());
      }
    }
  }

  return last_item_in_group_fn;
}

void downloadable_items::delete_item(download_item * item)
{
  link_map.remove(item->get_link());
  filename_map.remove(item->get_filename());

  delete item;
  item = nullptr;
}

void downloadable_items::stop(const download_item * item)
{
  if(remove_working_item(item))
  {
    pending_items.prepend(item);
  }
}

bool downloadable_items::contains_link(const QString & link) const
{
  return link_map.contains(link);
}

QList<const download_item *> downloadable_items::get_segmented_items() const
{
  QList<const download_item *> segmented_items;
  for(const auto item : all_items)
  {
    if(item->is_segment_ends_empty() == false)
      segmented_items.append(item);
  }

  return segmented_items;
}

void downloadable_items::set_item_segment_ends_by_filename(const QString & filename, const QVector<qint64> & seg_ends, qint64 remaining_bytes)
{
  auto it = filename_map.find(filename);
  if(it != filename_map.end())
  {
    it.value()->set_segment_ends(seg_ends);
    it.value()->set_remaining_bytes(remaining_bytes);
  }
}

void downloadable_items::set_download_lists_directory(const QString & dir_path)
{
  download_lists_directory = dir_path;
  download_list_file_path = download_lists_directory + "/" + pending_download_list_filename;
}

qint64 downloadable_items::get_sum_of_remaining_bytes() const
{
  qint64 sum = 0;
  for(const auto item : all_items)
  {
    const qint64 bytes = item->get_remaining_bytes();
    if(bytes > 0)
      sum += bytes;
  }

  return sum;
}

int downloadable_items::get_all_items_count() const
{
  return all_items.size();
}

void downloadable_items::load_from_file()
{
  if(QFile::exists(download_list_file_path) == false)
    return;

  QFile file(download_list_file_path);
  if(file.open(QIODevice::ReadOnly | QIODevice::Text) == false)
  {
    qDebug() << "Failed to open" << download_list_file_path;
    return;
  }

  const QString content = QString::fromUtf8(file.readAll());
  file.close();

  QList<download_item> parsed_content;
  parse_file_content(content, &parsed_content);
  load_parsed_content(parsed_content);
}

void downloadable_items::save_to_file()
{
  if(save_file == nullptr)
    return;

  save_file->seek(0);
  for(const auto & item : all_items)
  {
    const QString item_line = item->to_string() + QChar::LineFeed;
    save_file->write(item_line.toUtf8());
  }

  save_file->resize(save_file->pos());
}

void downloadable_items::parse_file_content(const QString & content, QList<download_item> * parsed_content) const
{
  const QStringList lines = content.split(QChar::LineFeed, QString::SkipEmptyParts);
  parsed_content->reserve(lines.size());

  const int max_columns = 4;
  for(int i = 0; i < lines.size(); ++i)
  {
    const QString trimmed_line = lines.at(i).trimmed();
    const QStringList words = trimmed_line.split(QChar::Tabulation, QString::SkipEmptyParts);
    if(words.size() == max_columns)
    {
      download_item dl_item;
      dl_item.set_filename(words.at(0));
      dl_item.set_link(words.at(1));
      dl_item.set_group_id(words.at(2).toInt());
      dl_item.set_file_size_bytes(words.at(3).toLongLong());

      parsed_content->append(dl_item);
    }
    else
    {
      qDebug() << "File:" << download_list_file_path << "line" << i + 1 << "should contain" << max_columns << "words";
    }
  }
}

void downloadable_items::load_parsed_content(const QList<download_item> & parsed_content)
{
  clear();

  for(const auto & item : parsed_content)
  {
    if(item.get_group_id() > max_group_id)
      max_group_id = item.get_group_id();

    add(item);
  }
}

bool downloadable_items::remove_working_item(const download_item * item)
{
  const bool removed = working_items.removeOne(const_cast<download_item *>(item));
  if(removed == false)
  {
    qDebug() << "Could not remove download_item * from working_items; url:" << item->get_link();
    return false;
  }

  return true;
}
