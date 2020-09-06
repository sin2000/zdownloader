#include "gdrive_api.h"
#include "../post_request.h"
#include "../get_request.h"
#include "../del_request.h"
#include "../../utils/jwt.h"
#include "../../logged_nam.h"
#include "gdrive_file.h"
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <qdebugex.h>

std::unique_ptr<jwt> gdrive_api::jwt_header = nullptr;
QByteArray gdrive_api::access_token;
std::unique_ptr<QTimer> gdrive_api::access_token_timer = nullptr;

gdrive_api::gdrive_api(QObject * parent)
  :QObject(parent),
   nam(new logged_nam(this))
{
  if(access_token_timer == nullptr)
  {
    access_token_timer = std::unique_ptr<QTimer>(new QTimer);
    jwt_header = std::unique_ptr<jwt>(new jwt);
    access_token_timer->setTimerType(Qt::PreciseTimer);
    access_token_timer->setSingleShot(true);
    // 3000 secs for update access token
    access_token_timer->setInterval(3000 * 1000);
  }
  connect(access_token_timer.get(), &QTimer::timeout, this, &gdrive_api::access_token_timer_timeout);
}

gdrive_api::~gdrive_api()
{
  abort_all_connections();
}

void gdrive_api::abort_all_connections()
{
  qDeleteAll(req_set);
  req_set.clear();
}

void gdrive_api::set_jwt_header(const jwt & header)
{
  if(jwt_header)
    *jwt_header = header;
}

void gdrive_api::stop_access_token_timer()
{
  access_token_timer->stop();
}

bool gdrive_api::has_access_token_expired() const
{
  return access_token.isEmpty();
}

void gdrive_api::request_access_token()
{
  post_request * postman = new post_request(nam);
  req_set.insert(postman);

  QUrlQuery query;
  query.addQueryItem("grant_type", "urn:ietf:params:oauth:grant-type:jwt-bearer");
  query.addQueryItem("assertion", QString::fromUtf8(jwt_header->compose()));

  postman->set_url(QUrl("https://oauth2.googleapis.com/token"));
  postman->set_post_body(query.toString(QUrl::FullyEncoded).toUtf8());

  connect(postman, &post_request::success, this, &gdrive_api::request_access_token_success);
  connect(postman, &post_request::error_occured, this, &gdrive_api::request_access_token_error);

  postman->start_post();
  access_token_timer->start();
}

void gdrive_api::request_access_token_success(post_request * sender, const QByteArray & response)
{
  req_set.remove(sender);
  sender->deleteLater();

  qDebug() << "gdrive_api::request_access_token_success";
  const QJsonDocument doc = QJsonDocument::fromJson(response);
  if(doc.isNull())
  {
    qDebug() << "gdrive_api::request_access_token_success - json document is null";
    emit request_access_token_finished("");
    return;
  }

  if(doc.isObject() == false)
  {
    qDebug() << "gdrive_api::request_access_token_success - json document is not object";
    emit request_access_token_finished("");
    return;
  }

  const QJsonValue jv = doc.object().value("access_token");
  if(jv.isUndefined())
  {
    qDebug() << "gdrive_api::request_access_token_success - missing access_token key in json object";
    emit request_access_token_finished("");
    return;
  }

  access_token = jv.toString().toUtf8();
  if(access_token.isEmpty())
  {
    qDebug() << "gdrive_api::request_access_token_success - access_token value is empty";
    emit request_access_token_finished("");
    return;
  }

  emit request_access_token_finished(QString::fromUtf8(access_token));
}

void gdrive_api::request_access_token_error(post_request * sender, QNetworkReply::NetworkError error)
{
  req_set.remove(sender);
  sender->deleteLater();

  qDebug() << "gdrive_api::request_access_token_error" << error;

  access_token.clear();

  emit request_access_token_finished("");
}

void gdrive_api::list_files()
{
  get_request * get = new get_request(nam);
  req_set.insert(get);

  //QUrlQuery query;
  //query.addQueryItem("fields", "*");
  get->set_url(QUrl("https://www.googleapis.com/drive/v3/files"));
  get->set_request_headers({ QNetworkReply::RawHeaderPair("Authorization", "Bearer " + access_token) });
  //get->set_url_query(query);

  connect(get, &get_request::success, this, &gdrive_api::list_files_success);
  connect(get, &get_request::error_occured, this, &gdrive_api::list_files_error);

  get->start_download();
}

void gdrive_api::list_files_success(get_request * sender, const QByteArray & response)
{
  req_set.remove(sender);
  sender->deleteLater();

  qDebug() << "gdrive_api::list_files_success";

  QList<gdrive_file> files;
  const QJsonDocument doc = QJsonDocument::fromJson(response);
  const QJsonArray arr = doc["files"].toArray();
  for(const auto & item : arr)
  {
    if(item["kind"] == "drive#file")
    {
      gdrive_file f(item["name"].toString(), item["id"].toString());
      if(f.name.isEmpty() || f.id.isEmpty())
        qDebug() << "gdrive_api::list_files_success - empty name or id";
      else
        files += gdrive_file(item["name"].toString(), item["id"].toString());
    }
  }

  emit list_files_finished(true, files);
}

void gdrive_api::list_files_error(get_request * sender, QNetworkReply::NetworkError error)
{
  req_set.remove(sender);
  sender->deleteLater();

  qDebug() << "gdrive_api::list_files_error" << error;

  emit list_files_finished(false, QList<gdrive_file>());
}

void gdrive_api::copy_file(const QString & file_id)
{
  post_request * postman = new post_request(nam);
  req_set.insert(postman);

  QUrlQuery query;
  query.addQueryItem("supportsAllDrives", "true");
  QUrl copy_url = QUrl("https://www.googleapis.com/drive/v3/files/" + file_id + "/copy");
  copy_url.setQuery(query);

  postman->set_url(copy_url);
  postman->set_request_headers({ QNetworkReply::RawHeaderPair("Authorization", "Bearer " + access_token) });

  connect(postman, &post_request::success, this, &gdrive_api::copy_file_success);
  connect(postman, &post_request::error_occured, this, &gdrive_api::copy_file_error);

  postman->start_post();
}

void gdrive_api::copy_file_success(post_request * sender, const QByteArray & response)
{
  req_set.remove(sender);
  sender->deleteLater();

  qDebug() << "gdrive_api::copy_file_success";

  const QJsonDocument doc = QJsonDocument::fromJson(response);
  const QString dup_file_id = doc["id"].toString();

  emit copy_file_finished(dup_file_id);
}

void gdrive_api::copy_file_error(post_request * sender, QNetworkReply::NetworkError error)
{
  req_set.remove(sender);
  sender->deleteLater();

  qDebug() << "gdrive_api::copy_file_error" << error;

  emit copy_file_finished("");
}

void gdrive_api::remove_file(const QString & file_id)
{
  del_request * req = new del_request(nam);
  req_set.insert(req);

  req->set_url(QUrl("https://www.googleapis.com/drive/v3/files/" + file_id));
  req->set_request_headers({ QNetworkReply::RawHeaderPair("Authorization", "Bearer " + access_token) });

  connect(req, &del_request::success, this, &gdrive_api::remove_file_success);
  connect(req, &del_request::error_occured, this, &gdrive_api::remove_file_error);

  req->start_download();
}

void gdrive_api::remove_file_success(abstract_http_request * sender, const QByteArray & /*response*/)
{
  req_set.remove(sender);
  sender->deleteLater();

  qDebug() << "gdrive_api::remove_file_success";

  emit remove_file_finished(true);
}

void gdrive_api::remove_file_error(abstract_http_request * sender, QNetworkReply::NetworkError error)
{
  req_set.remove(sender);
  sender->deleteLater();

  qDebug() << "gdrive_api::remove_file_error" << error;

  emit remove_file_finished(false);
}

void gdrive_api::share_file(const QString & file_id)
{
  post_request * postman = new post_request(nam);
  req_set.insert(postman);

  postman->set_url(QUrl("https://www.googleapis.com/drive/v3/files/" + file_id + "/permissions"));
  postman->set_request_headers({ QNetworkReply::RawHeaderPair("Authorization", "Bearer " + access_token) });
  postman->set_content_type("application/json");
  postman->set_post_body("{role:\"reader\",type:\"anyone\"}");

  connect(postman, &post_request::success, this, &gdrive_api::share_file_success);
  connect(postman, &post_request::error_occured, this, &gdrive_api::share_file_error);

  postman->start_post();
}

void gdrive_api::share_file_success(post_request * sender, const QByteArray & /*response*/)
{
  req_set.remove(sender);
  sender->deleteLater();

  qDebug() << "gdrive_api::share_file_success";

  emit share_file_finished(true);
}

void gdrive_api::share_file_error(post_request * sender, QNetworkReply::NetworkError error)
{
  req_set.remove(sender);
  sender->deleteLater();

  qDebug() << "gdrive_api::share_file_error" << error;

  emit share_file_finished(false);
}

void gdrive_api::file_info(const QString & file_id)
{
  get_request * get = new get_request(nam);
  req_set.insert(get);

  QUrlQuery query;
  query.addQueryItem("fields", "name,size");
  query.addQueryItem("supportsAllDrives", "true");
  get->set_url(QUrl("https://www.googleapis.com/drive/v3/files/" + file_id));
  get->set_request_headers({ QNetworkReply::RawHeaderPair("Authorization", "Bearer " + access_token) });
  get->set_url_query(query);

  connect(get, &get_request::success, this, &gdrive_api::file_info_success);
  connect(get, &get_request::error_occured, this, &gdrive_api::file_info_error);

  get->start_download();
}

void gdrive_api::file_info_success(get_request * sender, const QByteArray & response)
{
  req_set.remove(sender);
  sender->deleteLater();

  qDebug() << "gdrive_api::file_info_success";

  const QJsonDocument doc = QJsonDocument::fromJson(response);
  const QJsonObject obj = doc.object();
  gdrive_file file(obj["name"].toString(), "", obj["size"].toString().toLongLong());

  emit file_info_finished(QNetworkReply::NoError, file);
}

void gdrive_api::file_info_error(get_request * sender, QNetworkReply::NetworkError error)
{
  req_set.remove(sender);
  sender->deleteLater();

  qDebug() << "gdrive_api::file_info_error" << error;
  emit file_info_finished(error, gdrive_file("", ""));
}

const QByteArray & gdrive_api::get_access_token() const
{
  return access_token;
}

void gdrive_api::access_token_timer_timeout()
{
  access_token.clear();
}
