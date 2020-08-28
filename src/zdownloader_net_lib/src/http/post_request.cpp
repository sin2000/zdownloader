#include "post_request.h"
#include <QTimer>
#include <qdebugex.h>

post_request::post_request(QNetworkAccessManager * global_nam, QObject * parent)
  :abstract_post_request(global_nam, parent)
{
}

void post_request::set_post_body(const QByteArray & body)
{
  post_body = body;
}

void post_request::set_content_type(const QString & conent_type)
{
  post_content_type = conent_type;
}

void post_request::post()
{
  qDebug() << "HTTP: POST to" << get_url_as_string();

  QNetworkRequest req(url);
  req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, redirects_enabled);
  req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, false);
  req.setHeader(QNetworkRequest::UserAgentHeader, http_user_agent);
  if(post_content_type.isEmpty())
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  else
    req.setHeader(QNetworkRequest::ContentTypeHeader, post_content_type);

  if(req_cookies.isEmpty() == false)
    req.setRawHeader("Cookie", req_cookies);
  if(req_headers.isEmpty() == false)
  {
    for(const auto & header_pair : qAsConst(req_headers))
      req.setRawHeader(header_pair.first, header_pair.second);
  }

  net_reply = nam->post(req, post_body);
  connect(net_reply, &QNetworkReply::finished, this, &post_request::operation_finished);

  conn_state->start();
}

void post_request::operation_finished()
{
  is_working = false;
  conn_state->stop();

  const auto err_code = net_reply->error();
  if(err_code != QNetworkReply::NoError)
  {
    net_reply->deleteLater();

    emit error_occured(this, err_code);
    return;
  }

  qDebug() << "HTTP: finished POST to" << get_url_as_string();

  reply_headers = net_reply->rawHeaderPairs();
  const QByteArray download_result = net_reply->readAll();

  net_reply->deleteLater();

  emit success(this, download_result);
}
