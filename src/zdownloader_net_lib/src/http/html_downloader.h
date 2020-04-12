#ifndef HTML_DOWNLOADER_H
#define HTML_DOWNLOADER_H

#include "abstract_get_request.h"

class html_downloader : public abstract_get_request
{
  Q_OBJECT
public:
  html_downloader(QNetworkAccessManager * global_nam = nullptr, QObject * parent = nullptr);

signals:
  void success(html_downloader * sender, const QString & html);
  void error_occured(html_downloader * sender, QNetworkReply::NetworkError err_code, const QString & err_text = "");

private:
  void download() final;
  void operation_finished() final;
};

#endif // HTML_DOWNLOADER_H
