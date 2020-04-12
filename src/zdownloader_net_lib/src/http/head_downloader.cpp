#include "head_downloader.h"
#include <meta_object_ext.h>
#include <QNetworkReply>
#include <QTimer>
#include <qdebugex.h>

head_downloader::head_downloader(QNetworkAccessManager * global_nam, QObject * parent)
  :abstract_get_request(global_nam, parent),
   use_get_method(false)
{
}

void head_downloader::set_use_get_method(bool enabled)
{
  use_get_method = enabled;
}

void head_downloader::download()
{
  qDebug() << "HTTP: head from" << get_url_as_string();

  QNetworkRequest req(url);
  req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, redirects_enabled);
  req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
  req.setHeader(QNetworkRequest::UserAgentHeader, http_user_agent);
  if(req_cookies.isEmpty() == false)
    req.setRawHeader("Cookie", req_cookies);
  if(req_headers.isEmpty() == false)
  {
    for(const auto & header_pair : req_headers)
      req.setRawHeader(header_pair.first, header_pair.second);
  }

  req.setRawHeader("Accept-Encoding", "identity");

  if(use_get_method == false)
  {
    net_reply = nam->head(req);
  }
  else
  {
    net_reply = nam->get(req);
    connect(net_reply, &QNetworkReply::metaDataChanged, this, &head_downloader::metadata_changed);
  }

  connect(net_reply, &QNetworkReply::finished, this, &head_downloader::operation_finished);

  conn_state->start();
}

void head_downloader::metadata_changed()
{
  if(is_working)
  {
    disconnect(net_reply, &QNetworkReply::metaDataChanged, this, &head_downloader::metadata_changed);
    disconnect(net_reply, &QNetworkReply::finished, this, &head_downloader::operation_finished);

    reply_headers = net_reply->rawHeaderPairs();

    abort_operation();

    emit success(this);
  }
}

void head_downloader::operation_finished()
{
  is_working = false;
  conn_state->stop();

  const QNetworkReply::NetworkError err_code = net_reply->error();
  if(err_code != QNetworkReply::NoError)
  {
    const QString err_text = tr("HTTP: head error - %1").arg(net_reply->errorString());
    net_reply->deleteLater();
    emit error_occured(this, err_code, err_text);
    return;
  }

  qDebug() << "HTTP: finished head from" << get_url_as_string();

  reply_headers = net_reply->rawHeaderPairs();

  net_reply->deleteLater();
  emit success(this);
}
