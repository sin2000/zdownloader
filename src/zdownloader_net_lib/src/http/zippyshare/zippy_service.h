#ifndef ZIPPY_SERVICE_H
#define ZIPPY_SERVICE_H

#include "../service.h"
#include "../../model/download_item.h"
#include <QNetworkReply>

class html_downloader;
class head_downloader;
class zippy_link_extractor;

class zippy_service : public service
{
  Q_OBJECT
public:
  zippy_service(QNetworkAccessManager * global_nam, bool fetch_file_size = true, QObject * parent = nullptr);
  ~zippy_service();
  void abort_operation() const final;

protected:
  void fetch_info() final;

private:
  void get_main_html(const QString & url);
  void get_main_html_success(html_downloader * sender, const QString & html);
  void get_main_html_error(html_downloader * sender, QNetworkReply::NetworkError err_code, const QString & err_text);

  void get_head(const QString & link);
  void get_head_success(head_downloader * sender);
  void get_head_error_occured(head_downloader * sender, QNetworkReply::NetworkError err_code, const QString & err_text);

  html_downloader * html_dl;
  head_downloader * head_dl;
  zippy_link_extractor * link_extractor;
  download_item dl_item;
  bool fetch_file_size_enabled;
};

#endif // ZIPPY_SERVICE_H
