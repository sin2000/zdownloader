#ifndef ABSTRACT_HTTP_REQUEST_H
#define ABSTRACT_HTTP_REQUEST_H

#include "../zdownloader_net_lib_global.h"
#include <QObject>
#include <QUrl>
#include <QNetworkReply>

class QNetworkAccessManager;
class QTimer;

class ZD_NET_LIBSHARED_EXPORT abstract_http_request : public QObject
{
  Q_OBJECT
public:
  abstract_http_request(QNetworkAccessManager * global_nam = nullptr, QObject * parent = nullptr);
  virtual ~abstract_http_request();

  static void set_user_agent(const QString & user_agent);
  static void set_max_seconds_before_abort_inactive_connection(int seconds);
  void set_url(const QUrl & url_input);
  QString get_url_as_string() const;
  void abort_operation(bool disconnect_netreply_signals = true, bool delete_netreply = true);
  void set_redirects_enabled(bool enabled);
  void set_request_cookies(const QByteArray & cookies);
  QString get_reply_header_value(const QString & header_name) const;
  void set_request_headers(const QList<QNetworkReply::RawHeaderPair> & request_headers);
  void add_request_header(const QNetworkReply::RawHeaderPair & request_header);

  bool is_busy() const;
  void enable_redirects_logging();

protected:
  static QString http_user_agent;
  static int conn_state_interval_ms;
  QNetworkAccessManager * nam;
  QNetworkReply * net_reply;
  QUrl url;
  QTimer * conn_state; // check if connection stuck
  bool is_working;
  bool redirects_enabled;
  QList<QNetworkReply::RawHeaderPair> reply_headers;
  QByteArray req_cookies;
  QList<QNetworkReply::RawHeaderPair> req_headers;

private:
  virtual void operation_finished() = 0;
  void check_connection();

  void log_redirect(const QUrl & redir_url);
};

#endif // ABSTRACT_HTTP_REQUEST_H
