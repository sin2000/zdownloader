#ifndef ZD_LOGGER_H
#define ZD_LOGGER_H

#include "zd_logger_global.h"
#include "private/io_thread.h"
#include "private/file_logger.h"
#include "private/text_logger_manager.h"

class io_thread;

class ZD_LOGGERSHARED_EXPORT zd_logger : public io_thread
{
public:
  static zd_logger & inst();
  static void delete_this();

  // if log_directory_path is not empty open log file and redirect qDebug etc
  void install_logger(const QString & log_directory_path, const QString & timestamp_format, bool enable_debug_logger = true,
                      const QString & encryption_password = "", const QByteArray & rsa_pubkey = "",
                      int max_rotate_bytes = 4 * 1024 * 1024, int max_archive_files = 3);
  void uninstall_logger();

private:
  zd_logger() {}
  ~zd_logger();
  zd_logger(zd_logger const &);
  void operator=(zd_logger const &);

  static void qt_log_msg_handler(QtMsgType type, const QMessageLogContext & context, const QString & msg);
};

#endif // ZD_LOGGER_H
