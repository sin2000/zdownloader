#ifndef IO_THREAD_H
#define IO_THREAD_H

#include "../zd_logger_global.h"
#include <QThread>

class app_logger;
class text_logger_manager;

class ZD_LOGGERSHARED_EXPORT io_thread : public QThread
{
  Q_OBJECT
public:
  io_thread();
  ~io_thread();

  app_logger * app_log;
  text_logger_manager * text_log_mgr;

signals:
  void init_finished();

protected:
  void run();
};

#endif // IO_THREAD_H
