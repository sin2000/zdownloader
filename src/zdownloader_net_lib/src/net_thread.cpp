#include "net_thread.h"
#include "logged_nam.h"
#include "http/download_manager.h"

#include <QCoreApplication>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <qdebugex.h>

net_thread::net_thread(QObject * parent)
  :QThread(parent),
   http_download_manager(nullptr)
   //global_nam(nullptr)
{
}

net_thread::~net_thread()
{
  if(isRunning())
  {
    quit();
    wait();
  }
}

download_manager * net_thread::get_dl_mgr()
{
  rw_locker.lockForRead();
  download_manager * dl = http_download_manager;
  rw_locker.unlock();
  return dl;
}

void net_thread::run()
{
  //global_nam = new logged_nam();
  preload_network();

  rw_locker.lockForWrite();
  http_download_manager = new download_manager(nullptr, nullptr);
  rw_locker.unlock();

  emit init_finished();
  exec();
  QCoreApplication::processEvents();

  rw_locker.lockForWrite();
  delete http_download_manager;
  http_download_manager = nullptr;
  rw_locker.unlock();
  //delete global_nam;
}

void net_thread::preload_network() const
{
  set_global_ssl_configuration();

//  QNetworkRequest req;
//  QNetworkReply * reply = global_nam->get(req);
//  reply->abort();
//  delete reply;
}

void net_thread::set_global_ssl_configuration() const
{
  QSslConfiguration ssl_conf = QSslConfiguration::defaultConfiguration();

#if defined(ZD_DEBUG) || defined(DEBUG_PROXY)
  qDebug() << "WARNING: NO CERT VERIFY MODE";
  ssl_conf.setPeerVerifyMode(QSslSocket::VerifyNone);
#endif

  QSslConfiguration::setDefaultConfiguration(ssl_conf);
}
