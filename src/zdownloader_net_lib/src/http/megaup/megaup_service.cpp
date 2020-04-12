#include "megaup_service.h"
#include "megaup_link_extractor.h"
#include "../html_downloader.h"
#include "../head_downloader.h"
#include <QFileInfo>
#include <QTimer>
#include <qdebugex.h>

megaup_service::megaup_service(QNetworkAccessManager * global_nam, QObject * parent)
  :service(parent),
   html_dl(new html_downloader(global_nam, this)),
   head_dl(new head_downloader(global_nam, this)),
   link_extractor(new megaup_link_extractor),
   wait_timer(new QTimer(this))
{
  connect(html_dl, &html_downloader::success, this, &megaup_service::get_main_html_success);
  connect(html_dl, &html_downloader::error_occured, this, &megaup_service::get_main_html_error);

  wait_timer->setSingleShot(true);
  wait_timer->setTimerType(Qt::PreciseTimer);
  connect(wait_timer, &QTimer::timeout, head_dl, &head_downloader::start_download);
}

megaup_service::~megaup_service()
{
  delete link_extractor;
}

void megaup_service::abort_operation() const
{
  html_dl->abort_operation();
  head_dl->abort_operation();
  wait_timer->stop();
}

void megaup_service::fetch_info()
{
  get_main_html(base_url);
}

void megaup_service::get_main_html(const QString & url)
{
  html_dl->set_url(url);
  html_dl->start_download();
}

void megaup_service::get_main_html_success(html_downloader * sender, const QString & html)
{
  const QString curr_link = sender->get_url_as_string();
  if(link_extractor->parse_html(html))
  {
    get_head(link_extractor->get_extracted_link(), sender->get_reply_header_value("Set-Cookie"));
  }
  else
  {
    fetch_error err_code = fetch_error::unknown_error;
    QString err = "Html parse error for url: " + curr_link;

    if(link_extractor->get_last_error() == megaup_link_extractor::extracted_link_invalid)
      err += " Extracted link is invalid.";

    emit fetch_download_info_error(err, err_code, this);
  }
}

void megaup_service::get_main_html_error(html_downloader * /*sender*/, QNetworkReply::NetworkError err_code, const QString & err_text)
{
  fetch_error err;
  if(err_code == QNetworkReply::ContentNotFoundError)
    err = fetch_error::file_does_not_exists;
  else
    err = fetch_error::network_error;

  emit fetch_download_info_error(err_text, err, this);
}

void megaup_service::get_head(const QString & extracted_link, const QString & cookies)
{
  head_dl->disconnect();
  connect(head_dl, &head_downloader::success, this, &megaup_service::get_head_success);
  connect(head_dl, &head_downloader::error_occured, this, &megaup_service::get_head_error_occured);

  head_dl->set_url(extracted_link);
  head_dl->set_request_cookies(cookies.toUtf8());
  qDebug() << "waiting about 6 seconds for megaup.net";
  wait_timer->start(6000);
}

void megaup_service::get_head_success(head_downloader * sender)
{
  const QString location = sender->get_reply_header_value("location");
  const QUrl loc_url(location, QUrl::StrictMode);
  const QString location_error = validate_location(loc_url);
  if(location_error.isEmpty() == false)
  {
    emit fetch_download_info_error(location_error, service::network_error, this);
    return;
  }

  dl_item.set_link(base_url);
  dl_item.set_filename(QFileInfo(loc_url.path()).fileName());
  dl_item.set_direct_download_link(location);

  get_head_on_file_url(location);
}

void megaup_service::get_head_error_occured(head_downloader * /*sender*/, QNetworkReply::NetworkError /*err_code*/, const QString & err_text)
{
  emit fetch_download_info_error(err_text, service::unknown_error, this);
}

void megaup_service::get_head_on_file_url(const QString & file_url)
{
  head_dl->disconnect();
  connect(head_dl, &head_downloader::success, this, &megaup_service::get_head_on_file_url_success);
  connect(head_dl, &head_downloader::error_occured, this, &megaup_service::get_head_on_file_url_error_occured);

  head_dl->set_url(file_url);
  head_dl->start_download();
}

void megaup_service::get_head_on_file_url_success(head_downloader * sender)
{
  const qint64 file_size_bytes = sender->get_reply_header_value("Content-Length").toLongLong();
  if(file_size_bytes > 0)
  {
    dl_item.set_file_size_bytes(file_size_bytes);
    emit fetch_download_info_success(dl_item, this);
    return;
  }

  emit fetch_download_info_error("megaup_service error: file_size_bytes <= 0 url: " + sender->get_url_as_string(), service::unknown_error, this);
}

void megaup_service::get_head_on_file_url_error_occured(head_downloader * /*sender*/, QNetworkReply::NetworkError /*err_code*/, const QString & err_text)
{
  emit fetch_download_info_error(err_text, service::unknown_error, this);
}

QString megaup_service::validate_location(const QUrl & location) const
{
  if(location.isEmpty())
    return "Location header not found";

  if(location.isValid() == false)
    return "Location header has invalid URL";

  if(location.scheme() != "https")
    return "Location header has wrong scheme";

  if(location.host().endsWith("megaup.net") == false)
    return "Location header has wrong domain";

  return "";
}
