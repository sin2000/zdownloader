#include "abstract_get_request.h"
#include <qdebugex.h>

abstract_get_request::abstract_get_request(QNetworkAccessManager * global_nam, QObject * parent)
  :abstract_http_request(global_nam, parent)
{
}

abstract_get_request::~abstract_get_request()
{
}

void abstract_get_request::start_download()
{
  if(is_working)
  {
    qDebug() << "HTTP request already running for url:" << url << ". No action taken.";
    return;
  }

  is_working = true;
  download();
}
