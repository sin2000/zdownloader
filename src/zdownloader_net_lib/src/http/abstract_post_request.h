#ifndef ABSTRACT_POST_REQUEST_H
#define ABSTRACT_POST_REQUEST_H

#include "../zdownloader_net_lib_global.h"
#include "abstract_http_request.h"

class ZD_NET_LIBSHARED_EXPORT abstract_post_request : public abstract_http_request
{
  Q_OBJECT
public:
  abstract_post_request(QNetworkAccessManager * global_nam = nullptr, QObject * parent = nullptr);
  virtual ~abstract_post_request();

  void start_post();

private:
  virtual void post() = 0;
};

#endif // ABSTRACT_POST_REQUEST_H
