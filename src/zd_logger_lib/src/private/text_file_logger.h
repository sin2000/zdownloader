#ifndef TEXT_FILE_LOGGER_H
#define TEXT_FILE_LOGGER_H

#include "file_logger.h"
#include <QObject>

class ZD_LOGGERSHARED_EXPORT text_file_logger : public QObject, protected file_logger
{
public:
  text_file_logger(const QString & name);

  void invoke_set_rotate_settings(int max_archive_file_count, int rotate_after_bytes);
  void invoke_set_timestamp_format(const QString & new_timestamp_format);
  void invoke_enable(const QString & directory_path, const QString & filename, bool write_open_msg);
  void invoke_disable(bool write_close_msg);
  void invoke_log_text(qint64 timestamp_msecs, const QString & raw_text, const QString & log_type = "I");

private:
  void set_rotate_settings(int max_archive_file_count, int rotate_after_bytes);
  void set_timestamp_format(const QString & new_timestamp_format);
  void enable(const QString & directory_path, const QString & filename, bool write_open_msg);
  void disable(bool write_close_message);
  void log_text(qint64 timestamp_msecs, const QString & raw_text, const QString & log_type = "I");
  QString format_log_text(qint64 timestamp_msecs, const QString & raw_text, const QString & log_type) const;

  QString timestamp_format;
};

#endif // TEXT_FILE_LOGGER_H
