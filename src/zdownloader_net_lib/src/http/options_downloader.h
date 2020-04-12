#ifndef OPTIONS_DOWNLOADER_H
#define OPTIONS_DOWNLOADER_H

#include "abstract_get_request.h"

class options_downloader : public abstract_get_request
{
  Q_OBJECT
public:
  options_downloader(QNetworkAccessManager * global_nam = nullptr, QObject * parent = nullptr);

signals:
  void success(options_downloader * sender);
  void error_occured(options_downloader * sender, QNetworkReply::NetworkError error_code, const QString & err_text = "");

private:
  void download() final;
  void operation_finished() final;
};

#endif // OPTIONS_DOWNLOADER_H
