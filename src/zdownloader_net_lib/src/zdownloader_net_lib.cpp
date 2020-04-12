#include "zdownloader_net_lib.h"
#include <QEventLoop>
#include <qdebugex.h>

zdownloader_net_lib::zdownloader_net_lib(bool start_and_init, QObject * parent)
  :net_thread(parent),
   has_init_finished(false)
{
  connect(this, &net_thread::init_finished, this, &zdownloader_net_lib::set_init_finished, Qt::QueuedConnection);
  if(start_and_init)
  {
    QEventLoop loop;
    connect(this, &net_thread::init_finished, &loop, &QEventLoop::quit, Qt::QueuedConnection);
    start();
    loop.exec();
  }
}

zdownloader_net_lib::~zdownloader_net_lib()
{
  qDebug() << "destructor net_lib";
}

void zdownloader_net_lib::set_init_finished()
{
  has_init_finished = true;
}
