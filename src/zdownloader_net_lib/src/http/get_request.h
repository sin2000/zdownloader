#ifndef GET_REQUEST_H
#define GET_REQUEST_H

#include "abstract_get_request.h"

class get_request : public abstract_get_request
{
  Q_OBJECT
public:
  get_request(QNetworkAccessManager * global_nam = nullptr, QObject * parent = nullptr);

  void set_url_query(const QUrlQuery & query);

signals:
  void success(get_request * sender, const QByteArray & response);
  void error_occured(get_request * sender, QNetworkReply::NetworkError error);

protected:
  void set_operate_as_delete_req(bool set_as_delete_req);

private:
  void download() final;
  void operation_finished() final;

  QString req_verb;
  bool operate_as_delete_req;
};

#endif // GET_REQUEST_H
