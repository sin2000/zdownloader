#ifndef DEL_REQUEST_H
#define DEL_REQUEST_H

#include "get_request.h"

class del_request : public get_request
{
  Q_OBJECT
public:
  del_request(QNetworkAccessManager * global_nam = nullptr, QObject * parent = nullptr);
};

#endif // DEL_REQUEST_H
