#include "del_request.h"

del_request::del_request(QNetworkAccessManager * global_nam, QObject * parent)
  :get_request(global_nam, parent)
{
  set_operate_as_delete_req(true);
}
