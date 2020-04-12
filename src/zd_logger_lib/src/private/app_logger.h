#ifndef APP_LOGGER_H
#define APP_LOGGER_H

#include <QObject>
#include <QElapsedTimer>

class file_logger;
class msg_log_context;
class debug_logger;
class botan_wrapper;

class app_logger : public QObject
{
  Q_OBJECT
public:
  app_logger(QObject * parent = nullptr);
  ~app_logger();

  void enable_filelog(const QString & dir_path, const QString & enc_pass, const QByteArray & rsa_public_key,
                      const QString & timestamp_format_str, int max_rotate_bytes, int max_archive_files);
  void enable_debuglog();

  void log_msg(const msg_log_context & ctx);

private:
  QString format_msg(const msg_log_context & ctx) const;
  QString msg_type_to_string(QtMsgType type) const;
  void log_text_and_handle_rotate(const QByteArray & text);

  QString timestamp_format;
  file_logger * flog;
  debug_logger * dlog;
};

#endif // APP_LOGGER_H
