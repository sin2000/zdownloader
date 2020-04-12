#include "abstract_http_request.h"
#include "../logged_nam.h"
#include "web_config.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <qdebugex.h>

QString abstract_http_request::http_user_agent = web_config::inst().http_user_agent;
int abstract_http_request::conn_state_interval_ms = 60 * 1000;

abstract_http_request::abstract_http_request(QNetworkAccessManager * global_nam, QObject * parent)
  :QObject(parent),
   net_reply(nullptr),
   conn_state(new QTimer(this)),
   is_working(false),
   redirects_enabled(false)
{
  nam = (global_nam == nullptr) ? new logged_nam(this) : global_nam;
  nam->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);

  conn_state->setInterval(conn_state_interval_ms);
  connect(conn_state, &QTimer::timeout, this, &abstract_http_request::check_connection);
}

abstract_http_request::~abstract_http_request()
{
  abort_operation();
}

void abstract_http_request::set_user_agent(const QString & user_agent)
{
  QString ua = user_agent;
  if(ua.isEmpty())
    ua = web_config::inst().http_user_agent;

  http_user_agent = ua;
}

void abstract_http_request::set_max_seconds_before_abort_inactive_connection(int seconds)
{
  conn_state_interval_ms = seconds * 1000;
}

void abstract_http_request::set_url(const QUrl & url_input)
{
  url = url_input;
  reply_headers.clear();
  req_cookies.clear();
  req_headers.clear();
}

QString abstract_http_request::get_url_as_string() const
{
  return url.toString(QUrl::None);
}

void abstract_http_request::abort_operation(bool disconnect_netreply_signals, bool delete_netreply)
{
  conn_state->stop();
  if(is_working)
  {
    qDebug () << "abort:" << get_url_as_string();

    if(disconnect_netreply_signals)
    {
      disconnect(net_reply, nullptr, this, nullptr);
      net_reply->abort();
    }
    else
    {
      QTimer::singleShot(0, net_reply, &QNetworkReply::abort);
    }

    if(delete_netreply)
      net_reply->deleteLater();

    is_working = false;
  }
}

void abstract_http_request::set_redirects_enabled(bool enabled)
{
  redirects_enabled = enabled;
}

void abstract_http_request::set_request_cookies(const QByteArray & cookies)
{
  req_cookies = cookies;
}

QString abstract_http_request::get_reply_header_value(const QString & header_name) const
{
  const QByteArray header_name_bytes = header_name.toUtf8();
  for(const auto & header : reply_headers)
  {
    if(header.first == header_name_bytes)
      return QString::fromUtf8(header.second);
  }

  return "";
}

void abstract_http_request::set_request_headers(const QList<QNetworkReply::RawHeaderPair> & request_headers)
{
  req_headers = request_headers;
}

void abstract_http_request::add_request_header(const QNetworkReply::RawHeaderPair & request_header)
{
  req_headers.append(request_header);
}

bool abstract_http_request::is_busy() const
{
  return is_working;
}

void abstract_http_request::check_connection()
{
  conn_state->stop();
  if(is_working)
  {
    if(net_reply->isFinished())
    {
      qDebug() << "This shouldnt happen!!!";
      operation_finished();
    }
    else
    {
      //powinno rzucic sygnalem finished
      qDebug() << "Connection inactive for" << QString::number(conn_state->interval() / 1000) << "seconds. Aborting" << get_url_as_string();
      QTimer::singleShot(0, net_reply, &QNetworkReply::abort);
    }
  }
}
