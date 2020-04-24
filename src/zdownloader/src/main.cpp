#include "cmd_runner.h"
#include "app_settings.h"
#include <zd_logger.h>
#include <QCoreApplication>
#include <QSslSocket>
#include <QElapsedTimer>
#include <csignal>
#ifdef DEBUG_PROXY
  #include <QSslConfiguration>
  #include <QNetworkProxy>
  #include <QFile>
#endif
#ifndef SYSTEM_WINDOWS
  #include <unix_sigwatch.h>
#else
  #include "win_sigwatch.h"
#endif
#include <qdebugex.h>


int main(int argc, char *argv[])
{
  qApp->setApplicationName("zdownloader");
  qApp->setApplicationVersion("2.0");

  // https://bugreports.qt.io/browse/QTBUG-40332
  qputenv("QT_BEARER_POLL_TIMEOUT", "2000000000");

  QCoreApplication * app = new QCoreApplication(argc, argv);

  app_settings * app_set = new app_settings;
  app_set->load();
  const QString out_msgs_val_msg = app_set->out_msgs.validate();
  if(out_msgs_val_msg.isEmpty() == false)
  {
    std::printf("OutputMessages settings error:\n");
    std::printf("%s\n", out_msgs_val_msg.toStdString().c_str());
    delete app;
    delete app_set;
    return 0;
  }

  bool is_debug_console_logger_enabled = true;
  const QString log_dir = app_set->out_msgs.log_to_file_enabled ? app_set->out_msgs.log_directory : "";

  zd_logger::inst().install_logger(log_dir, app_set->out_msgs.timestamp_format, is_debug_console_logger_enabled, "", "",
                                    app_set->out_msgs.log_max_rotate_kb * 1024, app_set->out_msgs.log_max_archive_files);
  qDebug() << "";
  qDebug() << "=========Logger start=========";
  qDebug() << "zdownloader version:" << qApp->applicationVersion();
  qDebug() << "(C) 2019-2020 sin2000";
  qDebug() << "------------------------------";
  qDebug() << "qt runtime version:" << qVersion();
  qDebug() << "compiled with qt version:" << QT_VERSION_STR;
  qDebug() << "SSL runtime version:" << QSslSocket::sslLibraryVersionString();
  qDebug() << "compiled with SSL version:" << QSslSocket::sslLibraryBuildVersionString();
  qDebug() << "QElapsedTimer clock type:" << QElapsedTimer::clockType();

#ifdef DEBUG_PROXY
//  QFile pem_file("charlesss.pem");
//  pem_file.open(QIODevice::ReadOnly);
//  QByteArray pem = pem_file.readAll();
//  QSslConfiguration sconf = QSslConfiguration::defaultConfiguration();
//  sconf.setCaCertificates(QList<QSslCertificate>() << QSslCertificate(pem, QSsl::Pem));
//  QSslConfiguration::setDefaultConfiguration(sconf);
//  qDebug() << QSslConfiguration::defaultConfiguration().caCertificates();
#endif

  cmd_runner * runner = new cmd_runner(app_set);

#ifndef SYSTEM_WINDOWS
  unix_sigwatch * sigwatch = new unix_sigwatch(app);
  sigwatch->watch_for_signal(SIGINT);
  sigwatch->watch_for_signal(SIGTERM);
  sigwatch->watch_for_signal(SIGUSR1);
  sigwatch->watch_for_signal(SIGUSR2);
  sigwatch->watch_for_signal(SIGHUP);
  QObject::connect(sigwatch, &unix_sigwatch::unix_signal, runner, &cmd_runner::unix_signal_received);
#else
  win_sigwatch * sigwatch = new win_sigwatch(app);
  QObject::connect(sigwatch, &win_sigwatch::signal_received, runner, &cmd_runner::win_signal_received);
  sigwatch->watch_for_signals();
#endif

  app->exec();
  delete runner;
  delete sigwatch;
  delete app;

  zd_logger::inst().uninstall_logger();
  zd_logger::delete_this();
  return 0;
}
