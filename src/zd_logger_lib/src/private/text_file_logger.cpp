#include "text_file_logger.h"
#include <meta_object_ext.h>
#include <QDateTime>

text_file_logger::text_file_logger(const QString & /*name*/)
{
  set_max_archive_files(6);
  set_max_rotate_bytes(8 * 1024 * 1024);
}

void text_file_logger::invoke_set_rotate_settings(int max_archive_file_count, int rotate_after_bytes)
{
  meta_object_ext::invoke_async(this, &text_file_logger::set_rotate_settings, max_archive_file_count, rotate_after_bytes);
}

void text_file_logger::invoke_set_timestamp_format(const QString & new_timestamp_format)
{
  meta_object_ext::invoke_async(this, &text_file_logger::set_timestamp_format, new_timestamp_format);
}

void text_file_logger::invoke_enable(const QString & directory_path, const QString & filename, bool write_open_msg)
{
  meta_object_ext::invoke_async(this, &text_file_logger::enable, directory_path, filename, write_open_msg);
}

void text_file_logger::invoke_disable(bool write_close_msg)
{
  meta_object_ext::invoke_async(this, &text_file_logger::disable, write_close_msg);
}

void text_file_logger::invoke_log_text(qint64 timestamp_msecs, const QString & raw_text, const QString & log_type)
{
  meta_object_ext::invoke_async(this, &text_file_logger::log_text, timestamp_msecs, raw_text, log_type);
}

void text_file_logger::set_rotate_settings(int max_archive_file_count, int rotate_after_bytes)
{
  set_max_archive_files(max_archive_file_count);
  set_max_rotate_bytes(rotate_after_bytes);
}

void text_file_logger::set_timestamp_format(const QString & new_timestamp_format)
{
  timestamp_format = new_timestamp_format;
}

void text_file_logger::enable(const QString & directory_path, const QString & filename, bool write_open_msg)
{
  set_directory_path(directory_path);
  set_filename_pattern(filename + "_%archive_counter%.txt");
  open_log();

  if(write_open_msg)
  {
    const qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    log_text(timestamp, "");
    log_text(timestamp, "===========LOG OPENED=============");
  }
}

void text_file_logger::disable(bool write_close_message)
{
  if(write_close_message)
  {
    const qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    log_text(timestamp, "===========LOG CLOSED=============");
  }
  close_log();
}

void text_file_logger::log_text(qint64 timestamp_msecs, const QString & raw_text, const QString & log_type)
{
  if(is_valid())
  {
    QByteArray text = format_log_text(timestamp_msecs, raw_text, log_type).toUtf8();
    save_log(text, false);
  }
}

QString text_file_logger::format_log_text(qint64 timestamp_msecs, const QString & raw_text, const QString & log_type) const
{
  const QString curr_datetime = QDateTime::fromMSecsSinceEpoch(timestamp_msecs).toString(timestamp_format);
  const QString prefix = QString("[%1][%2] ").arg(curr_datetime, log_type);

  // left justify text to prefix
  QString out_text;
  QString tmp = raw_text;
  // remove all CR
  tmp.remove(QChar::CarriageReturn);
  const QStringList line_list = tmp.split(QChar::LineFeed, QString::SkipEmptyParts);
  if(line_list.size() > 1)
    out_text = line_list.join(QChar::LineFeed + QString(prefix.size() + 1, QChar::Space));
  else
    out_text = tmp;

  const QString log_msg = prefix + out_text;
  return log_msg;
}
