#ifndef ZD_NET_LIB_H
#define ZD_NET_LIB_H

#include "zdownloader_net_lib_global.h"
#include "net_thread.h"
#include "http/download_manager.h"

class ZD_NET_LIBSHARED_EXPORT zdownloader_net_lib : public net_thread
{
  Q_OBJECT
public:
  zdownloader_net_lib(bool start_and_init = true, QObject * parent = nullptr);
  virtual ~zdownloader_net_lib();

private:
  void set_init_finished();

  bool has_init_finished;
};

#endif // ZD_NET_LIB_H
