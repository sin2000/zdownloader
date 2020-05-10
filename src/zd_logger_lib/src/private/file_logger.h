#ifndef FILE_LOGGER_H
#define FILE_LOGGER_H

#include "../zd_logger_global.h"
#include <QString>

class QFile;
class msg_log_context;
class QLockFile;

class ZD_LOGGERSHARED_EXPORT file_logger
{
public:
  file_logger(const QString & name = "", bool use_lock_file = false, bool use_qdebug = true);
  virtual ~file_logger();

  void set_directory_path(const QString & directory_path);
  QString get_filename_from_pattern(int arch_counter, bool alt_name = false);
  void set_filename_pattern(const QString & filename_pattern);
  void set_max_rotate_bytes(int bytes);
  void set_max_archive_files(int max_files);
  bool open_log();
  void close_log();
  bool rotate_log(); // returns true if log was rotated
  void save_log(const QByteArray & text, bool flush = true, bool * log_rotated = nullptr);
  void save_log(const QString & text, bool flush = true);
  bool is_valid() const;
  QByteArray read_first_line() const;
  qint64 get_current_log_size() const;

private:
  QString dir_path;
  QString fn_pattern;
  QFile * cur_log_file;
  QLockFile * lock_file;
  bool enabled_lock_file;
  bool enabled_qdebug;

  static const QString archive_placeholder;
  int max_archive_files;
  int max_rotate_bytes;
};

#endif // FILE_LOGGER_H
