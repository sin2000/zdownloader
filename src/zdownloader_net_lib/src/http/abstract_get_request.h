#ifndef ABSTRACT_GET_REQUEST_H
#define ABSTRACT_GET_REQUEST_H

#include "../zdownloader_net_lib_global.h"
#include "abstract_http_request.h"

class ZD_NET_LIBSHARED_EXPORT abstract_get_request : public abstract_http_request
{
  Q_OBJECT
public:
  abstract_get_request(QNetworkAccessManager * global_nam = nullptr, QObject * parent = nullptr);
  virtual ~abstract_get_request();

  void start_download();

private:
  virtual void download() = 0;
};

#endif // ABSTRACT_GET_REQUEST_H
