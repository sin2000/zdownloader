#include "service.h"

service::service(QObject * parent)
  :QObject(parent)
{
}

void service::fetch_download_info(const QString & url)
{
  base_url = url;
  fetch_info();
}

const QString & service::get_base_url() const
{
  return base_url;
}
