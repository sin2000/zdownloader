#ifndef MULTI_LINK_CHECKER_H
#define MULTI_LINK_CHECKER_H

#include "../model/download_item.h"
#include "../model/links_queue.h"
#include "service.h"
#include <QObject>
#include <QStringList>

class QNetworkAccessManager;
class html_downloader;
class QTimer;

class multi_link_checker : public QObject
{  
  Q_OBJECT
public:
  multi_link_checker(QNetworkAccessManager * global_nam, QObject * parent = nullptr);
  ~multi_link_checker();

  void download_links_info(const QStringList & src_links);
  void set_zippyshare_fetch_file_size(bool enabled);
  void set_use_gdrive_api(bool gdrive_api_enabled);
  void set_max_parallel_link_checks(int max);
  void set_max_ms_before_check_link_again_on_error(int ms);

signals:
  void download_links_info_finished(const QList<download_item> & infos);

private:
  void download_next_link_info();
  void download_next_link_info_success(const download_item & info, service * sender);
  void download_next_link_info_error(const QString & error, service::fetch_error error_code, service * sender);
  void set_dl_item_status(download_item * item, service::fetch_error fetch_err) const;

  QNetworkAccessManager * nam;
  QList<download_item> download_items;
  links_queue queue;

  int max_parallel_link_checks;
  int max_ms_before_check_link_again_on_error;
  int download_connections;
  QTimer * download_restart_timer;

  QSet<service *> services;
  bool zippyshare_fetch_file_size;
  bool use_gdrive_api;
};

#endif // MULTI_LINK_CHECKER_H
