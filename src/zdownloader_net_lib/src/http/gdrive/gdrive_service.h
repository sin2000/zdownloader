#ifndef GDRIVE_SERVICE_H
#define GDRIVE_SERVICE_H

#include "../service.h"
#include <QNetworkReply>

class head_downloader;
class gdrive_api;
class gdrive_file;

class gdrive_service : public service
{
public:
  gdrive_service(QNetworkAccessManager * global_nam, QObject * parent = nullptr);
  void abort_operation() const final;
  void set_use_gdrive_api(bool gdrive_api_enabled);

  static QString get_file_id_from_gdrive_url(const QString & gdrive_url);

protected:
  void fetch_info() final;

private:
  void get_head_with_confirm_code(const QString & url);
  void get_head_with_confirm_code_success(head_downloader * sender);
  void get_head_with_confirm_code_error(head_downloader * sender, QNetworkReply::NetworkError err_code, const QString & err_text);

  void get_head_with_download_link(const QString & url, const QString & cookies);
  void get_head_with_download_link_success(head_downloader * sender);
  void get_head_with_download_link_error(head_downloader * sender, QNetworkReply::NetworkError err_code, const QString & err_text);

  void get_direct_download_link(const QString & url);
  void get_direct_download_link_success(head_downloader * sender);
  void get_direct_download_link_error(head_downloader * sender, QNetworkReply::NetworkError err_code, const QString & err_text);

  QString construct_gdrive_download_link(const QString & id_of_file, const QString & confirm_code = "") const;
  // returns error string
  QString validate_location(const QUrl & location) const;

  void api_get_access_token();
  void api_get_access_token_finished(const QString & access_token);
  void api_get_file_info();
  void api_get_file_info_finished(QNetworkReply::NetworkError error, const gdrive_file & file_info);

  const QString gdrive_host = "drive.google.com";
  const QString gdocs_host = "docs.googleusercontent.com";
  head_downloader * head_dl;

  QString file_id;
  bool use_gdrive_api;
  gdrive_api * gapi;
};

#endif // GDRIVE_SERVICE_H
