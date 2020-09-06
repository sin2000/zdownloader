#include "html_downloader.h"
#include <QTimer>
#include <qdebugex.h>

html_downloader::html_downloader(QNetworkAccessManager * global_nam, QObject * parent)
  :abstract_get_request(global_nam, parent)
{
}

void html_downloader::download()
{
  qDebug() << "HTTP: downloading HTML from" << get_url_as_string();

  QNetworkRequest req(url);
  req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, redirects_enabled);
  req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, false);
  req.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);

  req.setHeader(QNetworkRequest::UserAgentHeader, http_user_agent);
  if(req_cookies.isEmpty() == false)
    req.setRawHeader("Cookie", req_cookies);
  req.setRawHeader("Accept-Encoding", "identity");

  net_reply = nam->get(req);
  enable_redirects_logging();
  connect(net_reply, &QNetworkReply::finished, this, &html_downloader::operation_finished);

  conn_state->start();
}

void html_downloader::operation_finished()
{
  is_working = false;
  conn_state->stop();

  const auto err_code = net_reply->error();
  if(err_code != QNetworkReply::NoError)
  {
    const QString err_text = tr("HTTP: download html error - %1").arg(net_reply->errorString());

    net_reply->deleteLater();

    emit error_occured(this, err_code, err_text);
    return;
  }

  const QString expected_content_type = "text/html";
  const QString content_type = net_reply->header(QNetworkRequest::ContentTypeHeader).toString();
  if(content_type.startsWith(expected_content_type) == false)
  {
    const QString err_text = tr("HTTP: download html error (wrong content type; expected %1...; got: %2)").arg(expected_content_type, content_type);
    qDebug() << err_text;

    net_reply->deleteLater();

    emit error_occured(this, err_code, err_text);
    return;
  }

  qDebug() << "HTTP: finished downloading HTML from" << get_url_as_string();

  reply_headers = net_reply->rawHeaderPairs();
  const QString download_result = QString::fromUtf8(net_reply->readAll());

  net_reply->deleteLater();

  emit success(this, download_result);
}
