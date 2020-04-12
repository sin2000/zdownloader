#include "segmented_file_downloader.h"
#include "file_downloader2.h"
#include "../model/download_item.h"
#include "segment_calculator.h"
#include <string_utils.h>
#include <QFile>
#include <QTimer>
#include <qdebugex.h>

segmented_file_downloader::segmented_file_downloader(QNetworkAccessManager * global_nam, QObject * parent)
  :QObject(parent),
   curr_dl_item(nullptr),
   nam(global_nam),
   max_segments(1),
   active_segments(0),
   finished_segments(0),
   min_segment_size_bytes(2 * 1024 * 1024),
   shared_file(new QFile(this)),
   all_seg_error_state(false),
   segment_read_buffer_size_bytes(1024),
   segment_write_buffer_size_bytes(1024)
{
}

segmented_file_downloader::~segmented_file_downloader()
{
  qDeleteAll(downloaders);
}

void segmented_file_downloader::start_download(download_item * item)
{
  curr_dl_item = item;
  active_segments = 0;
  finished_segments = 0;
  all_seg_error_state = false;

  qDebug() << "HTTP: start download FILE" << item->get_filename() << "from" << item->get_direct_download_link();

  QString error;
  setup_file_error err_ret = setup_file(&error);
  if(err_ret != file_error_no_error)
  {
    if(err_ret == file_error_already_exists)
    {
      curr_dl_item->set_status(download_item::download_status_finished_already_exists);
      emit download_finished("", curr_dl_item->get_direct_download_link());
    }
    else
    {
      emit download_finished(error, curr_dl_item->get_direct_download_link());
    }
    return;
  }

  compute_seg_metadata_list();
  prepare_downloaders();
  start_next_segment_download();
}

void segmented_file_downloader::set_download_dir(const QString & dir_path)
{
  download_dir = dir_path;
}

void segmented_file_downloader::set_max_download_segments(int segments)
{
  max_segments = segments;
}

void segmented_file_downloader::set_min_segment_size(int bytes)
{
  min_segment_size_bytes = bytes;
}

void segmented_file_downloader::set_segment_network_read_buffer_size(int bytes)
{
  segment_read_buffer_size_bytes = bytes;
}

void segmented_file_downloader::set_segment_write_buffer_size(int bytes)
{
  segment_write_buffer_size_bytes = bytes;
}

double segmented_file_downloader::get_current_download_speed_mbps() const
{
  double sum = 0;
  for(const auto * dl : downloaders)
  {
    if(dl->is_busy())
      sum += dl->get_current_download_speed_mbps();
  }

  return sum;
}

QString segmented_file_downloader::get_current_progress_msg() const
{
  QString progress_msg;
  qint64 all_seg_bytes_recv = 0;
  double download_speed_sum = 0;
  for(const auto * dl : downloaders)
  {
    all_seg_bytes_recv += dl->get_bytes_received_and_on_disk();
    if(dl->is_busy())
      download_speed_sum += dl->get_current_download_speed_bps();
  }

  const double all_seg_mbytes_recv = all_seg_bytes_recv / (static_cast<double>(1024 * 1024));
  double percent = 0.0;
  const qint64 file_total_size_bytes = curr_dl_item->get_file_size_bytes();
  if(file_total_size_bytes > 0)
  {
    const qint64 remaining_bytes = file_total_size_bytes - all_seg_bytes_recv;
    qint64 remaining_seconds;
    if(download_speed_sum > 0)
      remaining_seconds = static_cast<qint64>(remaining_bytes / download_speed_sum);
    else
      remaining_seconds = 0;

    QString remaining_msg;
    if(remaining_seconds != 0)
      remaining_msg = string_utils::seconds_to_string(remaining_seconds);
    else
      remaining_msg = "-";

    percent = static_cast<double>(all_seg_bytes_recv) / file_total_size_bytes;
    percent *= 100;

    const double mbytes_total = file_total_size_bytes / (static_cast<double>(1024 * 1024));
    progress_msg = QString("Progress: %1% [%2/%3MB] ETA: %4 ActSeg: %5/%6 - %7").arg(
          QString::number(percent, 'f', 2),
          QString::number(all_seg_mbytes_recv, 'f', 2),
          QString::number(mbytes_total, 'f', 2),
          remaining_msg,
          QString::number(active_segments),
          QString::number(max_segments),
          curr_dl_item->get_filename()
          );
  }
  else
  {
    progress_msg = QString("Progress: [%1MB] ActSeg: %2/%3 - %4").arg(
          QString::number(all_seg_mbytes_recv, 'f', 2),
          QString::number(active_segments),
          QString::number(max_segments),
          curr_dl_item->get_filename()
          );
  }

  return progress_msg;
}

void segmented_file_downloader::save_and_abort_download(bool emit_finished_signal)
{
  for(int i = 0; i < downloaders.size() && i < seg_metadata_list.size(); ++i)
  {
    auto dl = downloaders.at(i);
    dl->save_and_abort_download();
    if(seg_metadata_list[i].segment_finished == false)
      curr_dl_item->set_segment_end_pos(i, dl->get_last_file_write_cursor_pos());
  }

  if(emit_finished_signal)
    emit download_finished("Connections aborted for " + curr_dl_item->get_filename(), curr_dl_item->get_direct_download_link());
}

download_item * segmented_file_downloader::get_curr_dl_item() const
{
  return curr_dl_item;
}

segmented_file_downloader::setup_file_error segmented_file_downloader::setup_file(QString * error)
{
  const QString file_name = curr_dl_item->get_filename();
  const QString file_path = download_dir + "/" + file_name;
  const QString part_file_path = file_path + ".part";
  if(QFile::exists(file_path))
  {
    *error = "File already exists: " + file_name;
    return file_error_already_exists;
  }

  shared_file->setFileName(part_file_path);
  const bool need_resize = shared_file->exists() == false;
  if(shared_file->open(QIODevice::ReadWrite) == false)
  {
    *error = "Could not open file for write&read: " + shared_file->fileName();
    return file_error_could_not_open_readwrite;
  }

  if(need_resize)
  {
    if(shared_file->resize(curr_dl_item->get_file_size_bytes()) == false)
    {
      *error = "Could not resize file: " + shared_file->fileName();
      return file_error_could_not_resize;
    }
  }

  return file_error_no_error;
}

void segmented_file_downloader::unsetup_file()
{
  if(shared_file->rename(download_dir + "/" + curr_dl_item->get_filename()) == false)
    qDebug() << "Could not rename file from:" << shared_file->fileName() << "to:" << curr_dl_item->get_filename();

  shared_file->close();
}

void segmented_file_downloader::compute_seg_metadata_list()
{
  if(curr_dl_item->is_segment_ends_empty() == false)
    max_segments = curr_dl_item->get_segment_ends_size();

  segment_calculator seg_calc;
  seg_calc.compute(curr_dl_item->get_file_size_bytes(), min_segment_size_bytes, max_segments);
  seg_metadata_list = seg_calc.get_segment_metadata_list();
  max_segments = seg_metadata_list.size();

  if(curr_dl_item->is_segment_ends_empty())
  {
    curr_dl_item->set_segment_ends_size(max_segments);
  }
  else
  {
    if(max_segments != curr_dl_item->get_segment_ends_size())
    {
      qDebug() << "WARNING: max_segments != segment_ends";
      curr_dl_item->set_segment_ends_size(max_segments);
    }
  }

  const auto & seg_ends = curr_dl_item->get_segment_ends();
  for(int i = 0; i < seg_ends.size() && i < seg_metadata_list.size(); ++i)
  {
    if(seg_ends[i] >= seg_metadata_list[i].end_pos)
    {
      seg_metadata_list[i].segment_finished = true;
      ++finished_segments;
    }
  }
}

void segmented_file_downloader::prepare_downloaders()
{
  qDeleteAll(downloaders);
  downloaders.clear();
  downloaders.reserve(max_segments);
  for(int i = 0; i < max_segments; ++i)
  {
    auto * dl = new file_downloader2(nam);
    connect(dl, &file_downloader2::download_started, this, &segmented_file_downloader::download_segment_started);
    connect(dl, &file_downloader2::download_success, this, &segmented_file_downloader::download_segment_success);
    connect(dl, &file_downloader2::download_error, this, &segmented_file_downloader::download_segment_error);

    const qint64 seg_dl_end_pos = curr_dl_item->get_segment_end_pos(i);
    qint64 seg_end_pos = seg_metadata_list.at(i).end_pos;
    const qint64 seg_start_pos = seg_metadata_list.at(i).start_pos;
    qint64 last_downloaded_bytes_in_segment = (seg_dl_end_pos > seg_start_pos) ? seg_dl_end_pos - seg_start_pos : 0;
    const qint64 seg_dl_start_pos = seg_start_pos + last_downloaded_bytes_in_segment;

    dl->set_read_buffer_size(segment_read_buffer_size_bytes);
    dl->set_write_buffer_size(segment_write_buffer_size_bytes);

    dl->set_url(curr_dl_item->get_direct_download_link());
    dl->set_file_name(curr_dl_item->get_filename());
    dl->set_shared_file(shared_file);

    dl->set_downloaded_bytes_already(last_downloaded_bytes_in_segment);
    dl->set_download_start_pos(seg_dl_start_pos);
    dl->set_download_end_pos(seg_end_pos);

    downloaders.append(dl);
  }
}

void segmented_file_downloader::start_next_segment_download()
{
  const int dl_idx = get_first_pending_downloader_idx();
  if(dl_idx == -1)
  {
    if(finished_segments == max_segments)
      emit download_finished("", curr_dl_item->get_direct_download_link());

    return;
  }

  auto * downloader = downloaders.at(dl_idx);
  const auto & token = curr_dl_item->get_access_token();
  if(token.isEmpty() == false)
    downloader->set_request_headers({ QNetworkReply::RawHeaderPair("Authorization", "Bearer " + token) });
  downloader->start_download();
  ++active_segments;
}

void segmented_file_downloader::download_segment_started(file_downloader2 * /*sender*/)
{
  QTimer::singleShot(0, this, &segmented_file_downloader::start_next_segment_download);
}

void segmented_file_downloader::download_segment_success(file_downloader2 * sender)
{
  const int idx = downloaders.indexOf(sender);

  seg_metadata_list[idx].segment_finished = true;
  ++finished_segments;
  --active_segments;
  if(finished_segments == max_segments)
  {
    unsetup_file();
    curr_dl_item->clear_segment_ends();
    qDebug() << "HTTP: download success for FILE" << curr_dl_item->get_filename() << "from" << curr_dl_item->get_direct_download_link();
    emit download_finished("", curr_dl_item->get_direct_download_link());
  }
  else
  {
    curr_dl_item->set_segment_end_pos(idx, downloaders.at(idx)->get_last_file_write_cursor_pos());
  }
}

void segmented_file_downloader::download_segment_error(file_downloader2 * /*sender*/, const QString & error_text)
{
  --active_segments;
  if(all_seg_error_state == false)
  {
    all_seg_error_state = true;
    save_and_abort_download();

    emit download_finished(error_text, curr_dl_item->get_direct_download_link());
  }
}

int segmented_file_downloader::get_first_pending_downloader_idx() const
{
  for(int i = 0; i < downloaders.size(); ++i)
  {
    if(downloaders.at(i)->is_busy() == false && seg_metadata_list.at(i).segment_finished == false)
      return i;
  }

  return -1;
}
