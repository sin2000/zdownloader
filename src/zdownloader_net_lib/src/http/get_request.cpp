#include "get_request.h"
#include <QUrlQuery>
#include <QTimer>
#include <qdebugex.h>

get_request::get_request(QNetworkAccessManager * global_nam, QObject * parent)
  :abstract_get_request(global_nam, parent),
   req_verb("GET"),
   operate_as_delete_req(false)
{
}

void get_request::set_url_query(const QUrlQuery & query)
{
  url.setQuery(query);
}

void get_request::set_operate_as_delete_req(bool set_as_delete_req)
{
  operate_as_delete_req = set_as_delete_req;
  if(operate_as_delete_req)
    req_verb = "DELETE";
}

void get_request::download()
{
  qDebug() << "HTTP: " + req_verb +  " req" << get_url_as_string();

  QNetworkRequest req(url);
  req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, redirects_enabled);
  req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, false);
  req.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);

  req.setHeader(QNetworkRequest::UserAgentHeader, http_user_agent);
  if(req_cookies.isEmpty() == false)
    req.setRawHeader("Cookie", req_cookies);

  if(req_headers.isEmpty() == false)
  {
    for(const auto & header_pair : qAsConst(req_headers))
      req.setRawHeader(header_pair.first, header_pair.second);
  }

  if(operate_as_delete_req)
    net_reply = nam->deleteResource(req);
  else
    net_reply = nam->get(req);

  connect(net_reply, &QNetworkReply::finished, this, &get_request::operation_finished);

  conn_state->start();
}

void get_request::operation_finished()
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

  qDebug() << "HTTP: finished " + req_verb + " req from" << get_url_as_string();

  reply_headers = net_reply->rawHeaderPairs();
  const QByteArray response = net_reply->readAll();

  net_reply->deleteLater();

  emit success(this, response);
}
