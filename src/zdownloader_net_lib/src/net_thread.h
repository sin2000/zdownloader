#ifndef NET_THREAD_H
#define NET_THREAD_H

#include "zdownloader_net_lib_global.h"
#include <QReadWriteLock>
#include <QThread>

class download_manager;
class logged_nam;

class ZD_NET_LIBSHARED_EXPORT net_thread : public QThread
{
  Q_OBJECT
public:
  net_thread(QObject * parent = nullptr);
  ~net_thread();

  download_manager * get_dl_mgr();

signals:
  void init_finished();

protected:
  void run() final;

private:
  void preload_network() const;
  void set_global_ssl_configuration() const;

  download_manager * http_download_manager;
  QReadWriteLock rw_locker;

  //logged_nam * global_nam;
};

#endif // NET_THREAD_H
