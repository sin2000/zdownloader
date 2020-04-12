#include "io_thread.h"
#include "app_logger.h"
#include "text_logger_manager.h"
#include <QCoreApplication>

io_thread::io_thread()
{
  app_log = new app_logger();
  app_log->moveToThread(this);
  text_log_mgr = new text_logger_manager(this);
}

io_thread::~io_thread()
{
  if(isRunning())
  {
    quit();
    wait();
  }

  delete text_log_mgr;
  delete app_log;
  text_log_mgr = nullptr;
  app_log = nullptr;
}

void io_thread::run()
{
  emit init_finished();
  exec();
  QCoreApplication::processEvents();
}
