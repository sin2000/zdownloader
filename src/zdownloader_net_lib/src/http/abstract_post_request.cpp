#include "abstract_post_request.h"
#include <qdebugex.h>

abstract_post_request::abstract_post_request(QNetworkAccessManager * global_nam, QObject * parent)
  :abstract_http_request(global_nam, parent)
{
}

abstract_post_request::~abstract_post_request()
{
}

void abstract_post_request::start_post()
{
  if(is_working)
  {
    qDebug() << "HTTP post already running for url:" << url << ". No action taken.";
    return;
  }

  is_working = true;
  post();
}
