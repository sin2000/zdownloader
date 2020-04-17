#include "gdrive_service.h"
#include "../head_downloader.h"
#include "../../model/download_item.h"
#include "gdrive_api.h"
#include "gdrive_file.h"
#include <string_utils.h>
#include <QUrlQuery>

gdrive_service::gdrive_service(QNetworkAccessManager * global_nam, QObject * parent)
  :service(parent),
   head_dl(new head_downloader(global_nam, this)),
   use_gdrive_api(false),
   gapi(nullptr)
{
}

void gdrive_service::abort_operation() const
{
  head_dl->abort_operation();
  if(gapi)
    gapi->abort_all_connections();
}

void gdrive_service::set_use_gdrive_api(bool gdrive_api_enabled)
{
  use_gdrive_api = gdrive_api_enabled;
  if(use_gdrive_api && gapi == nullptr)
    gapi = new gdrive_api(this);
}

QString gdrive_service::get_file_id_from_gdrive_url(const QString & gdrive_url)
{
  // gdrive file id should have 33 chars or 28 chars but I don't know for sure
  // so I don't check fid length
  const QUrl gurl = gdrive_url;
  const QUrlQuery query(gurl);
  QString fid = query.queryItemValue("id");
  if(fid.isEmpty())
  {
    const QString path = gurl.path();
    fid = string_utils::pull_string(path, "/file/d/", "/");
    if(fid.isEmpty())
      fid = string_utils::pull_string(path, "/file/d/");
  }

  return fid;
}

void gdrive_service::fetch_info()
{
  file_id = get_file_id_from_gdrive_url(base_url);
  if(file_id.isEmpty())
  {
    emit fetch_download_info_error("file_id not found in gdrive url", fetch_error::file_does_not_exists, this);
    return;
  }

  if(use_gdrive_api)
  {
    api_get_access_token();
  }
  else
  {
    const QString gurl = construct_gdrive_download_link(file_id);
    get_head_with_confirm_code(gurl);
  }
}

void gdrive_service::get_head_with_confirm_code(const QString & url)
{
  head_dl->disconnect();
  connect(head_dl, &head_downloader::success, this, &gdrive_service::get_head_with_confirm_code_success);
  connect(head_dl, &head_downloader::error_occured, this, &gdrive_service::get_head_with_confirm_code_error);

  head_dl->set_url(url);
  head_dl->start_download();
}

void gdrive_service::get_head_with_confirm_code_success(head_downloader * sender)
{
  const QString cookies = sender->get_reply_header_value("Set-Cookie");
  const QString confirm_code = string_utils::pull_string(cookies, file_id + "=", ";");
  if(confirm_code.isEmpty())
  {
    const QString location = sender->get_reply_header_value("Location");
    const QString err = validate_location(QUrl(location, QUrl::StrictMode));
    if(err.isEmpty())
    {
      get_direct_download_link(location);
      return;
    }

    emit fetch_download_info_error("gdrive confirm code is empty or location error: " + err, fetch_error::unknown_error, this);
    return;
  }

  const QString durl = construct_gdrive_download_link(file_id, confirm_code);
  get_head_with_download_link(durl, cookies);
}

void gdrive_service::get_head_with_confirm_code_error(head_downloader * /*sender*/, QNetworkReply::NetworkError err_code, const QString & err_text)
{
  fetch_error err;
  if(err_code == QNetworkReply::ContentNotFoundError)
    err = fetch_error::file_does_not_exists;
  else
    err = fetch_error::network_error;

  emit fetch_download_info_error(err_text, err, this);
}

void gdrive_service::get_head_with_download_link(const QString & url, const QString & cookies)
{
  head_dl->disconnect();
  connect(head_dl, &head_downloader::success, this, &gdrive_service::get_head_with_download_link_success);
  connect(head_dl, &head_downloader::error_occured, this, &gdrive_service::get_head_with_download_link_error);

  head_dl->set_url(url);
  head_dl->set_request_cookies(cookies.toUtf8());
  head_dl->start_download();
}

void gdrive_service::get_head_with_download_link_success(head_downloader * sender)
{
  const QString location = sender->get_reply_header_value("Location");
  const QString err = validate_location(QUrl(location, QUrl::StrictMode));
  if(!err.isEmpty())
  {
    emit fetch_download_info_error(err, fetch_error::network_error, this);
    return;
  }

  get_direct_download_link(location);
}

void gdrive_service::get_head_with_download_link_error(head_downloader * /*sender*/, QNetworkReply::NetworkError /*err_code*/, const QString & err_text)
{
  emit fetch_download_info_error(err_text, fetch_error::network_error, this);
}

void gdrive_service::get_direct_download_link(const QString & url)
{
  head_dl->disconnect();
  connect(head_dl, &head_downloader::success, this, &gdrive_service::get_direct_download_link_success);
  connect(head_dl, &head_downloader::error_occured, this, &gdrive_service::get_direct_download_link_error);

  head_dl->set_url(url);
  head_dl->set_use_get_method(true);
  head_dl->add_request_header(QNetworkReply::RawHeaderPair("Range", "bytes=0-"));
  head_dl->start_download();
}

void gdrive_service::get_direct_download_link_success(head_downloader * sender)
{
  const QString content_disposition = sender->get_reply_header_value("Content-Disposition");
  const QString file_name = string_utils::pull_string(content_disposition, "filename=\"", "\";");
  if(file_name.isEmpty())
  {
    emit fetch_download_info_error("No filename in Content-Disposition header", fetch_error::network_error, this);
    return;
  }

  const QString content_range = sender->get_reply_header_value("Content-Range");
  const QString len = string_utils::pull_string(content_range, "/");
  const qint64 file_size_bytes = len.toLongLong();
  if(file_size_bytes <= 0)
  {
    emit fetch_download_info_error("gdrive_service error: file_size_bytes <= 0 url: " + sender->get_url_as_string(), service::unknown_error, this);
    return;
  }

  download_item dl_item;
  dl_item.set_link(base_url);
  dl_item.set_filename(file_name);
  dl_item.set_direct_download_link(sender->get_url_as_string());
  dl_item.set_file_size_bytes(file_size_bytes);

  emit fetch_download_info_success(dl_item, this);
}

void gdrive_service::get_direct_download_link_error(head_downloader * /*sender*/, QNetworkReply::NetworkError /*err_code*/, const QString & err_text)
{
  emit fetch_download_info_error(err_text, fetch_error::network_error, this);
}

QString gdrive_service::construct_gdrive_download_link(const QString & id_of_file, const QString & confirm_code) const
{
  QString res = "https://" + gdrive_host + "/uc?export=download&id=" + id_of_file;
  if(!confirm_code.isEmpty())
    res += "&confirm=" + confirm_code;

  return res;
}

QString gdrive_service::validate_location(const QUrl & location) const
{
  if(location.isEmpty())
    return "Location header not found";

  if(location.isValid() == false)
    return "Location header has invalid URL";

  if(location.scheme() != "https")
    return "Location header has wrong scheme";

  if(location.host().endsWith(gdocs_host) == false)
    return "Location header has wrong domain";

  return "";
}

void gdrive_service::api_get_access_token()
{
  if(gapi->has_access_token_expired())
  {
    connect(gapi, &gdrive_api::request_access_token_finished, this, &gdrive_service::api_get_access_token_finished, Qt::UniqueConnection);
    gapi->request_access_token();
  }
  else
  {
    api_get_file_info();
  }
}

void gdrive_service::api_get_access_token_finished(const QString & access_token)
{
  if(access_token.isEmpty())
  {
    emit fetch_download_info_error("gdrive access token is empty", fetch_error::network_error, this);
    return;
  }

  api_get_file_info();
}

void gdrive_service::api_get_file_info()
{
  connect(gapi, &gdrive_api::file_info_finished, this, &gdrive_service::api_get_file_info_finished, Qt::UniqueConnection);
  gapi->file_info(file_id);
}

void gdrive_service::api_get_file_info_finished(QNetworkReply::NetworkError error, const gdrive_file & file_info)
{
  if(error == QNetworkReply::NoError)
  {
    download_item dl_item;
    dl_item.set_link(base_url);
    dl_item.set_filename(file_info.name);
    dl_item.set_direct_download_link("https://www.googleapis.com/drive/v3/files/" + file_id + "?alt=media");
    dl_item.set_file_size_bytes(file_info.size);

    emit fetch_download_info_success(dl_item, this);
    return;
  }

  fetch_error err;
  if(error == QNetworkReply::ContentNotFoundError)
    err = fetch_error::file_does_not_exists;
  else
    err = fetch_error::network_error;

  emit fetch_download_info_error("gdrive_api error", err, this);
}
