#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>

class QNetworkAccessManager;
class download_item;

class service : public QObject
{
  Q_OBJECT
public:
  enum fetch_error {
    file_does_not_exists,
    network_error,
    unknown_error
  };

  service(QObject * parent = nullptr);

  void fetch_download_info(const QString & url);
  const QString & get_base_url() const;
  virtual void abort_operation() const = 0;

signals:
  void fetch_download_info_success(const download_item & info, service * sender);
  void fetch_download_info_error(const QString & error, service::fetch_error error_code, service * sender);

protected:
  virtual void fetch_info() = 0;

  QString base_url;
};

#endif // SERVICE_H
