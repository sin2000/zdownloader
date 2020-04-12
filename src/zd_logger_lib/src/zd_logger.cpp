#include "zd_logger.h"
#include "private/app_logger.h"
#include "private/msg_log_context.h"
#include "private/io_thread.h"
#include <meta_object_ext.h>
#include <QString>
#include <QEventLoop>
#include <QDateTime>
#include <QDebug>

zd_logger & zd_logger::inst()
{
  static zd_logger * instance = new zd_logger;
  return *instance;
}

void zd_logger::delete_this()
{
  delete &zd_logger::inst();
}

zd_logger::~zd_logger()
{
  uninstall_logger();
}

void zd_logger::install_logger(const QString & log_directory_path, const QString & timestamp_format, bool enable_debug_logger,
                                const QString & encryption_password, const QByteArray & rsa_pubkey,
                                int max_rotate_bytes, int max_archive_files)
{  
  //qRegisterMetaType<msg_log_context>();

  QEventLoop loop;
  QObject::connect(this, &io_thread::init_finished, &loop, &QEventLoop::quit, Qt::BlockingQueuedConnection);
  start();
  loop.exec();

  if(enable_debug_logger)
    app_log->enable_debuglog();

  if(log_directory_path.isEmpty() == false)
  {
    app_log->enable_filelog(log_directory_path, encryption_password, rsa_pubkey, timestamp_format, max_rotate_bytes, max_archive_files);
  }

  qInstallMessageHandler(qt_log_msg_handler);
}

void zd_logger::uninstall_logger()
{
  if(isRunning())
  {
    quit();
    wait();
  }
  qInstallMessageHandler(nullptr);
}

void zd_logger::qt_log_msg_handler(QtMsgType type, const QMessageLogContext & context, const QString & msg)
{
  msg_log_context ctx;
  ctx.timestamp_msecs = QDateTime::currentMSecsSinceEpoch();
  ctx.msg_type = type;
  ctx.category = QString::fromUtf8(context.category);
  ctx.file = QString::fromUtf8(context.file);
  ctx.function = QString::fromUtf8(context.function);
  ctx.line = context.line;
  ctx.version = context.version;
  ctx.msg = msg;

  meta_object_ext::invoke_async(zd_logger::inst().app_log, &app_logger::log_msg, ctx);

  //QMetaObject::invokeMethod(zd_logger::inst().app_log, "log_msg", Qt::QueuedConnection, Q_ARG(msg_log_context, ctx));
}
