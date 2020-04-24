#include "zippy_service.h"
#include "../html_downloader.h"
#include "../head_downloader.h"
#include "zippy_link_extractor.h"

zippy_service::zippy_service(QNetworkAccessManager * global_nam, bool fetch_file_size, QObject * parent)
  :service(parent),
   html_dl(new html_downloader(global_nam, this)),
   head_dl(nullptr),
   link_extractor(new zippy_link_extractor),
   fetch_file_size_enabled(fetch_file_size)
{
  connect(html_dl, &html_downloader::success, this, &zippy_service::get_main_html_success);
  connect(html_dl, &html_downloader::error_occured, this, &zippy_service::get_main_html_error);

  if(fetch_file_size_enabled)
  {
    head_dl = new head_downloader(global_nam, this);
    connect(head_dl, &head_downloader::success, this, &zippy_service::get_head_success);
    connect(head_dl, &head_downloader::error_occured, this, &zippy_service::get_head_error_occured);
  }
}

zippy_service::~zippy_service()
{
  delete link_extractor;
}

void zippy_service::abort_operation() const
{
  html_dl->abort_operation();
  if(head_dl)
    head_dl->abort_operation();
}

void zippy_service::fetch_info()
{
  get_main_html(base_url);
}

void zippy_service::get_main_html(const QString & url)
{
  html_dl->set_url(url);
  html_dl->start_download();
}

void zippy_service::get_main_html_success(html_downloader * sender, const QString & html)
{
  const QString curr_link = sender->get_url_as_string();
  if(link_extractor->parse_html(html, curr_link))
  {
    dl_item.set_link(curr_link);
    dl_item.set_filename(link_extractor->get_extracted_filename());
    dl_item.set_direct_download_link(link_extractor->get_extracted_link());

    if(fetch_file_size_enabled)
      get_head(dl_item.get_direct_download_link());
    else
      emit fetch_download_info_success(dl_item, this);
  }
  else
  {
    fetch_error err_code = fetch_error::unknown_error;
    QString err = "Html parse error. ";
    if(link_extractor->get_last_error() == zippy_link_extractor::file_does_not_exists)
    {
      err += "File does not exists on server";
      err_code = fetch_error::file_does_not_exists;
    }
    else
    {
      err += "Zippy changed algo?";
    }

    err += " Url: " + curr_link;

    emit fetch_download_info_error(err, err_code, this);
  }
}

void zippy_service::get_main_html_error(html_downloader * /*sender*/, QNetworkReply::NetworkError /*err_code*/, const QString & err_text)
{
  emit fetch_download_info_error(err_text, fetch_error::network_error, this);
}

void zippy_service::get_head(const QString & link)
{
  head_dl->set_url(link);
  head_dl->start_download();
}

void zippy_service::get_head_success(head_downloader * sender)
{
  const qint64 file_size_bytes = sender->get_reply_header_value("Content-Length").toLongLong();
  if(file_size_bytes > 0)
  {
    dl_item.set_file_size_bytes(file_size_bytes);
    emit fetch_download_info_success(dl_item, this);
    return;
  }

  emit fetch_download_info_error("zippy_service error: file_size_bytes <= 0 url: " + sender->get_url_as_string(), service::unknown_error, this);
}

void zippy_service::get_head_error_occured(head_downloader * /*sender*/, QNetworkReply::NetworkError /*err_code*/, const QString & err_text)
{
  emit fetch_download_info_error(err_text, service::network_error, this);
}
