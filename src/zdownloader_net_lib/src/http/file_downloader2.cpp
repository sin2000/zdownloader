#include "file_downloader2.h"
#include <QElapsedTimer>
#include <QTimer>
#include <QFile>
#include <qdebugex.h>

file_downloader2::file_downloader2(QNetworkAccessManager * global_nam, QObject * parent)
  :abstract_get_request(global_nam, parent),
   bytes_received_last(0),
   bytes_received_at_point_of_time(0),
   bytes_received_and_on_disk(0),
   download_start_pos_bytes(0),
   download_end_pos_bytes(0),
   file_write_cursor(0),
   download_time(new QElapsedTimer),
   reply_read_chunk_and_save_timer(new QTimer(this)),
   write_buffer_size(1 * 1024 * 1024),
   wrong_content_type(true),
   has_download_started(false),
   read_buffer_size(1 * 1024 * 1024)
{
  reply_read_chunk_and_save_timer->setTimerType(Qt::PreciseTimer);
  reply_read_chunk_and_save_timer->setInterval(250);
  connect(reply_read_chunk_and_save_timer, &QTimer::timeout, this, &file_downloader2::reply_read_chunk_and_save);
}

file_downloader2::~file_downloader2()
{
  conn_state->stop();
  reply_read_chunk_and_save_timer->stop();

  delete download_time;
}

void file_downloader2::set_download_start_pos(qint64 bytes)
{
  download_start_pos_bytes = bytes;
}

void file_downloader2::set_download_end_pos(qint64 bytes)
{
  download_end_pos_bytes = bytes;
}

void file_downloader2::set_downloaded_bytes_already(qint64 bytes)
{
  bytes_received_and_on_disk = bytes;
}

void file_downloader2::set_file_name(const QString & name)
{
  file_name = name;
}

void file_downloader2::set_shared_file(QFile * file)
{
  shared_file = file;
}

void file_downloader2::set_read_buffer_size(int bytes)
{
  read_buffer_size = bytes;
}

void file_downloader2::set_write_buffer_size(int bytes)
{
  write_buffer_size = bytes / 2;
  read_buffer.reserve(write_buffer_size + write_buffer_size);
}

double file_downloader2::get_current_download_speed_bps() const
{
  double current_speed = 0.0;
  const qint64 elapsed_ms_since_start = download_time->elapsed();
  const qint64 bytes_received_since_start = bytes_received_last - bytes_received_at_point_of_time;
  if(elapsed_ms_since_start != 0)
    current_speed = static_cast<double>(bytes_received_since_start) / elapsed_ms_since_start;

  current_speed *= 1000;

  download_time->start();
  bytes_received_at_point_of_time = bytes_received_last;

  return current_speed;
}

double file_downloader2::get_current_download_speed_mbps() const
{
  double current_speed = get_current_download_speed_bps();
  current_speed *= 8;
  current_speed /= 1024 * 1024;

  return current_speed;
}

qint64 file_downloader2::get_bytes_received_and_on_disk() const
{
  return bytes_received_and_on_disk;
}

qint64 file_downloader2::get_last_file_write_cursor_pos() const
{
  return file_write_cursor;
}

void file_downloader2::save_and_abort_download()
{
  if(is_working)
  {
    reply_read_chunk_and_save_timer->stop();
    reply_read_all_and_save(false);
    abort_operation(true, true);
    // do not remove QTimer::singleShot or abort could crash
    //QTimer::singleShot(0, net_reply, &QNetworkReply::abort);
  }
}

void file_downloader2::download()
{
  bytes_received_last = 0;
  bytes_received_at_point_of_time = 0;
  file_write_cursor = download_start_pos_bytes;
  wrong_content_type = true;
  has_download_started = false;

  QNetworkRequest req(url);
  req.setMaximumRedirectsAllowed(3);
  req.setHeader(QNetworkRequest::UserAgentHeader, http_user_agent);
  for(const auto & header : req_headers)
  {
    req.setRawHeader(header.first, header.second);
  }
  const QByteArray range_header_value = "bytes=" + QByteArray::number(download_start_pos_bytes) + "-" + QByteArray::number(download_end_pos_bytes);
  req.setRawHeader("Range", range_header_value);

  net_reply = nam->get(req);
  net_reply->setReadBufferSize(read_buffer_size);
  connect(net_reply, &QNetworkReply::downloadProgress, this, &file_downloader2::download_progress);
  connect(net_reply, &QNetworkReply::metaDataChanged, this, &file_downloader2::metadata_changed);
  connect(net_reply, &QNetworkReply::finished, this, &file_downloader2::operation_finished);

  conn_state->start();

  download_time->start();
  reply_read_chunk_and_save_timer->start();
}

void file_downloader2::download_progress(qint64 bytes_received, qint64 /*bytes_total*/)
{
  conn_state->start();

  bytes_received_last = bytes_received;
}

void file_downloader2::metadata_changed()
{
  //disconnect(net_reply, &QNetworkReply::metaDataChanged, this, &file_downloader2::metadata_changed);

  const QString unexpected_content_type = "text/";
  const QString content_type = net_reply->header(QNetworkRequest::ContentTypeHeader).toString().trimmed();
  if(content_type.startsWith(unexpected_content_type))
  {
    wrong_content_type = true;
    const QString err_text = tr("HTTP: FILE download error (wrong content type; unexpected start with %1; got: %2; file: %3)")
        .arg(unexpected_content_type, content_type, file_name);
    qDebug() << err_text;

    QTimer::singleShot(0, net_reply, &QNetworkReply::abort);
    return;
  }

  wrong_content_type = false;
  //qDebug() << "Srv content-type:" << content_type << "FILE:" << file_name;

  if(net_reply->error() == QNetworkReply::NoError && has_download_started == false)
  {
    has_download_started = true;
    emit download_started(this);
  }
}

void file_downloader2::operation_finished()
{
  is_working = false;
  conn_state->stop();
  reply_read_chunk_and_save_timer->stop();

  reply_read_all_and_save(true);

  if(net_reply->error() != QNetworkReply::NoError)
  {
    const QString err_text = "HTTP: download error for FILE " + file_name + ". Error: " + net_reply->errorString();
    emit download_error(this, err_text);

    net_reply->deleteLater();
    return;
  }

  emit download_success(this);

  net_reply->deleteLater();
}

void file_downloader2::reply_read_chunk_and_save()
{
  if(wrong_content_type)
    return;

  const QByteArray readed_data = net_reply->read(write_buffer_size);
  read_buffer.append(readed_data);
  if(read_buffer.size() >= write_buffer_size)
  {
    if(shared_file->seek(file_write_cursor) == false)
    {
      qDebug() << "Seek error on FILE:" << shared_file->fileName() << ". Aborting download";
      QTimer::singleShot(0, net_reply, &QNetworkReply::abort);
      return;
    }

    const qint64 bytes_written = shared_file->write(read_buffer);
    read_buffer.resize(0);
    if(bytes_written == -1)
    {
      qDebug() << "Error write to FILE:" << shared_file->fileName() << ". Aborting download";
      QTimer::singleShot(0, net_reply, &QNetworkReply::abort);
      return;
    }

    file_write_cursor += bytes_written;
  }

  bytes_received_and_on_disk += readed_data.size();
}

void file_downloader2::reply_read_all_and_save(bool read_all)
{
  if(wrong_content_type)
      return;

  int readed_data_size = 0;
  if(net_reply->isOpen())
  {
    QByteArray readed_data;
    if(read_all)
    {
      readed_data = net_reply->readAll();
    }
    else
    {
      const qint64 bytesAvailable = net_reply->bytesAvailable();
      if(bytesAvailable > 0)
        readed_data = net_reply->read(bytesAvailable);
    }

    read_buffer.append(readed_data);
    readed_data_size = readed_data.size();
  }

  if(read_buffer.isEmpty() == false)
  {
    if(shared_file->seek(file_write_cursor) == false)
      qDebug() << "Seek error on FILE:" << shared_file->fileName();

    const qint64 bytes_written = shared_file->write(read_buffer);
    read_buffer.clear();
    if(bytes_written == -1)
      qDebug() << "Error write to FILE:" << shared_file->fileName();
    else
      file_write_cursor += bytes_written;
  }

  bytes_received_and_on_disk += readed_data_size;
}
