#ifndef MEGAUP_SERVICE_H
#define MEGAUP_SERVICE_H

#include "../service.h"
#include "../../model/download_item.h"
#include <QNetworkReply>

class html_downloader;
class head_downloader;
class megaup_link_extractor;
class QTimer;

class megaup_service : public service
{
public:
  megaup_service(QNetworkAccessManager * global_nam, QObject * parent = nullptr);
  ~megaup_service() final;
  void abort_operation() const final;

protected:
  void fetch_info() final;

private:
  void get_main_html(const QString & url);
  void get_main_html_success(html_downloader * sender, const QString & html);
  void get_main_html_error(html_downloader * sender, QNetworkReply::NetworkError err_code, const QString & err_text);

  void get_head(const QString & extracted_link, const QString & cookies);
  void get_head_success(head_downloader * sender);
  void get_head_error_occured(head_downloader * sender, QNetworkReply::NetworkError err_code, const QString & err_text);

  void get_head_on_file_url(const QString & file_url);
  void get_head_on_file_url_success(head_downloader * sender);
  void get_head_on_file_url_error_occured(head_downloader * sender, QNetworkReply::NetworkError err_code, const QString & err_text);

  // returns error string
  QString validate_location(const QUrl & location) const;

  html_downloader * html_dl;
  head_downloader * head_dl;
  megaup_link_extractor * link_extractor;
  QTimer * wait_timer;

  download_item dl_item;
};

#endif // MEGAUP_SERVICE_H
