#include "options_downloader.h"
#include <QTimer>
#include <qdebugex.h>

options_downloader::options_downloader(QNetworkAccessManager * global_nam, QObject * parent)
  :abstract_get_request(global_nam, parent)
{
}

void options_downloader::download()
{
  qDebug() << "HTTP: options to " + url.toString();

  QNetworkRequest req(url);
  req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, redirects_enabled);
  req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
  req.setHeader(QNetworkRequest::UserAgentHeader, http_user_agent);
  if(req_cookies.isEmpty() == false)
    req.setRawHeader("Cookie", req_cookies);
  req.setRawHeader("Accept-Encoding", "identity");

  net_reply = nam->sendCustomRequest(req, "OPTIONS");
  connect(net_reply, &QNetworkReply::finished, this, &options_downloader::operation_finished);

  enable_redirects_logging();

  conn_state->start();
}

void options_downloader::operation_finished()
{
  is_working = false;
  conn_state->stop();

  const QNetworkReply::NetworkError err_code = net_reply->error();
  if(err_code != QNetworkReply::NoError)
  {
    const QString err_text = tr("HTTP: options error - %1").arg(net_reply->errorString());
    net_reply->deleteLater();
    emit error_occured(this, err_code, err_text);
    return;
  }

  const QString expected_content_type = "text/html";
  const QString content_type = net_reply->header(QNetworkRequest::ContentTypeHeader).toString();
  if(content_type.startsWith(expected_content_type) == false)
  {
    const QString err_text = tr("HTTP: options error (wrong content type; expected %1...; got: %2)").arg(expected_content_type, content_type);
    qDebug() << err_text;
    net_reply->deleteLater();
    emit error_occured(this, err_code, err_text);
    return;
  }

  qDebug() << "HTTP: finished options from " + url.toString();

  reply_headers = net_reply->rawHeaderPairs();

  net_reply->deleteLater();
  emit success(this);
}
