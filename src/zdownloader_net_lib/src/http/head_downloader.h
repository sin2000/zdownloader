#ifndef HEAD_DOWNLOADER_H
#define HEAD_DOWNLOADER_H

#include "abstract_get_request.h"

class head_downloader : public abstract_get_request
{
  Q_OBJECT
public:
  head_downloader(QNetworkAccessManager * global_nam = nullptr, QObject * parent = nullptr);
  void set_use_get_method(bool enabled);

signals:
  void success(head_downloader * sender);
  void error_occured(head_downloader * sender, QNetworkReply::NetworkError error_code, const QString & err_text = "");

private:
  void download() final;
  void metadata_changed();
  void operation_finished() final;
  void get_operation_finished();

  bool use_get_method;
};

#endif // HEAD_DOWNLOADER_H
