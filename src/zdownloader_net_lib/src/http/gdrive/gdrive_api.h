#ifndef GDRIVE_API_H
#define GDRIVE_API_H

#include "../../utils/jwt.h"
#include <QObject>
#include <QNetworkReply>
#include <QList>
#include <memory>

class logged_nam;
class post_request;
class jwt;
class get_request;
class abstract_http_request;
class del_request;
class gdrive_file;
class QTimer;

class gdrive_api : public QObject
{
  Q_OBJECT
public:
  gdrive_api(QObject * parent = nullptr);
  ~gdrive_api() final;

  static void set_jwt_header(const jwt & header);
  static void stop_access_token_timer();
  void abort_all_connections();
  bool has_access_token_expired() const;
  void request_access_token();
  void list_files();
  void copy_file(const QString & file_id);
  void remove_file(const QString & file_id);
  void share_file(const QString & file_id);
  void file_info(const QString & file_id);
  const QByteArray & get_access_token() const;

signals:
  void request_access_token_finished(const QString & access_token);
  void list_files_finished(bool success, const QList<gdrive_file> & files);
  void copy_file_finished(const QString & dup_file_id);
  void remove_file_finished(bool success);
  void share_file_finished(bool success);
  void file_info_finished(QNetworkReply::NetworkError error, const gdrive_file & file_info);

private:
  void request_access_token_success(post_request * sender, const QByteArray & response);
  void request_access_token_error(post_request * sender, QNetworkReply::NetworkError error);

  void list_files_success(get_request * sender, const QByteArray & response);
  void list_files_error(get_request * sender, QNetworkReply::NetworkError error);

  void copy_file_success(post_request * sender, const QByteArray & response);
  void copy_file_error(post_request * sender, QNetworkReply::NetworkError error);

  void remove_file_success(abstract_http_request * sender, const QByteArray & response);
  void remove_file_error(abstract_http_request * sender, QNetworkReply::NetworkError error);

  void share_file_success(post_request * sender, const QByteArray & response);
  void share_file_error(post_request * sender, QNetworkReply::NetworkError error);

  void file_info_success(get_request * sender, const QByteArray & response);
  void file_info_error(get_request * sender, QNetworkReply::NetworkError error);

  void access_token_timer_timeout();

  logged_nam * nam; // one instance for gdrive api
  QSet<abstract_http_request *> req_set;
  static std::unique_ptr<jwt> jwt_header;
  static QByteArray access_token;
  static std::unique_ptr<QTimer> access_token_timer;
};

#endif // GDRIVE_API_H
