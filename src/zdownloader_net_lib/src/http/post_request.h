#ifndef POST_REQUEST_H
#define POST_REQUEST_H

#include "abstract_post_request.h"

class post_request : public abstract_post_request
{
  Q_OBJECT
public:
  post_request(QNetworkAccessManager * global_nam = nullptr, QObject * parent = nullptr);

  void set_post_body(const QByteArray & body);
  void set_content_type(const QString & conent_type);

signals:
  void success(post_request * sender, const QByteArray & response);
  void error_occured(post_request * sender, QNetworkReply::NetworkError error);

private:
  void post() final;
  void operation_finished() final;

  QByteArray post_body;
  QString post_content_type;
};

#endif // POST_REQUEST_H
