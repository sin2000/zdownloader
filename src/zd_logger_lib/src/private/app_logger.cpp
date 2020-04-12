#include "app_logger.h"
#include "file_logger.h"
#include "debug_logger.h"
#include "msg_log_context.h"

#include <QDateTime>

app_logger::app_logger(QObject * parent)
  :QObject(parent),
   timestamp_format("yyyyMMdd.hh:mm:ss"),
   flog(new file_logger("", true, false)),
   dlog(nullptr)
{
}

app_logger::~app_logger()
{
  delete dlog;
  delete flog;

#ifdef ZD_ENCRYPT_LOG_FILES
  delete bw;
#endif
}

void app_logger::enable_filelog(const QString & dir_path, const QString & enc_pass, const QByteArray & rsa_public_key,
                                const QString & timestamp_format_str, int max_rotate_bytes, int max_archive_files)
{
  timestamp_format = timestamp_format_str;

  flog->set_max_rotate_bytes(max_rotate_bytes);
  flog->set_max_archive_files(max_archive_files);
  flog->set_directory_path(dir_path);
  flog->open_log();

  Q_UNUSED(enc_pass)
  Q_UNUSED(rsa_public_key)
}

void app_logger::enable_debuglog()
{
  if(dlog == nullptr)
    dlog = new debug_logger();
}

void app_logger::log_msg(const msg_log_context & ctx)
{
  QByteArray text = format_msg(ctx).toUtf8();

  if(dlog)
    dlog->log_msg(QString::fromUtf8(text));

  flog->save_log(text);
}

QString app_logger::format_msg(const msg_log_context & ctx) const
{
  const QString curr_datetime = QDateTime::fromMSecsSinceEpoch(ctx.timestamp_msecs).toString(timestamp_format);
  const QString msg_type = msg_type_to_string(ctx.msg_type);
  // ctx.function eg. 'void __cdecl mainwindow::show()
  // get string after __cdecl
  //const QString call_conv = "__cdecl ";
  //QString func = "";
  //int pos;
  //if ((pos = ctx.function.indexOf(call_conv)) != -1)
    //func = ctx.function.mid(pos + call_conv.size());

  //const QString function_or_path = func.isEmpty() ? ctx.file : func;
  //const QString line = QString::number(ctx.line);
  const QString log_msg = QString("[%1][%2] %3").arg(curr_datetime, msg_type, ctx.msg);

  return log_msg;
}

QString app_logger::msg_type_to_string(QtMsgType type) const
{
  switch(type)
  {
    case QtDebugMsg:
      return "D";

    case QtWarningMsg:
      return "W";

    case QtCriticalMsg:
      return "C";

    case QtFatalMsg:
      return "F";

    case QtInfoMsg:
      return "I";
  }

  return "";
}

void app_logger::log_text_and_handle_rotate(const QByteArray & text)
{
  bool log_rotated;
  flog->save_log(text, true, &log_rotated);
  if(log_rotated)
    flog->save_log(QByteArray(""));
}
