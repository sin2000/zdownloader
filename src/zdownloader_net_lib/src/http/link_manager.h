#ifndef LINK_MANAGER_H
#define LINK_MANAGER_H

#include "../model/downloadable_items.h"
#include "multi_link_checker.h"
#include <QObject>
#include <QPointer>

class QNetworkAccessManager;
class download_item_updater;

class link_manager : public QObject
{
  Q_OBJECT
public:
  link_manager(QNetworkAccessManager * global_nam, QObject * parent = nullptr);
  ~link_manager();

  void load_state(const QString & download_dir, const QString & download_lists_dir, const finished_downloads_settings & fin_dl_settings);
  void save_state(const QString & download_dir);
  void update_download_item(download_item * item);
  downloadable_items & get_download_queue();
  void set_links_file_dir(const QString & dir);
  void set_max_parallel_link_checks(int max);
  void set_max_seconds_before_check_link_again_on_error(int seconds);
  void set_use_gdrive_api(bool gdrive_api_enabled);
  void abort_dl_item_updater_connections();

signals:
  void load_state_finished(const QString & error_text = "");
  void update_download_item_finished(service::fetch_error error_code, const QString & error_text, download_item * item);

private:
  bool load_links_from_file();
  void parse_file_content(const QString & content);
  bool validate_link(const QString & link);
  void check_links_on_server();
  void check_next_links_on_server();
  void check_links_on_server_success(const QList<download_item> & infos);
  void check_links_on_server_error(service::fetch_error error_code, const QString & error_text);

  void download_item_updater_finished(download_item_updater * sender, service::fetch_error error_code, const QString & error_text, download_item * item);

  void load_segments_ends(const QString & download_dir);
  void save_segments_ends(const QString & download_dir) const;

  const QString links_file_name = "links.txt";
  QString links_file_dir;
  QString links_file_path;
  QList<QStringList> links_in_groups;
  int curr_links_group_idx;

  QNetworkAccessManager * nam;
  QPointer<multi_link_checker> link_checker;
  bool link_checker_in_use;
  int max_parallel_link_checks;
  int max_ms_before_check_link_again_on_error;
  bool use_gdrive_api;

  downloadable_items download_queue;
  QSet<download_item_updater *> di_updaters;
};

#endif // LINK_MANAGER_H
