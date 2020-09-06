#ifndef DOWNLOAD_MANAGER_H
#define DOWNLOAD_MANAGER_H

#include "../zdownloader_net_lib_global.h"
#include "service.h"
#include <QObject>
#include <QSet>

class QNetworkAccessManager;
class link_manager;
class download_item;
class QTimer;
class dl_item_downloader;
class increasing_timer;
class finished_downloads_settings;
class pre_post_job;
class gdrive_settings;

class ZD_NET_LIBSHARED_EXPORT download_manager : public QObject
{
  Q_OBJECT
public:
  download_manager(QNetworkAccessManager * global_nam, QObject * parent = nullptr);
  ~download_manager();

  void invoke_load_state(const finished_downloads_settings & fin_dl_settings);
  void invoke_start();
  void invoke_stop();
  void invoke_set_download_directory(const QString & dir);
  void invoke_set_download_lists_dir(const QString & dir);
  void invoke_set_links_file_dir(const QString & dir);
  void invoke_set_max_parallel_connections(int max);
  void invoke_set_user_reconnect_interval(int minutes);
  void invoke_set_speed_check_interval(int seconds);
  void invoke_set_min_avg_speed_before_reconnect(int mbps);
  void invoke_set_user_agent_for_https_connections(const QString & user_agent);
  void invoke_set_max_seconds_before_abort_inactive_connection(int seconds);
  void invoke_set_max_parallel_link_checks(int max);
  void invoke_set_max_seconds_before_check_link_again_on_error(int seconds);
  void invoke_set_max_segments_per_file(int segments);
  void invoke_set_min_segment_size_mb(int mbytes);
  void invoke_set_segment_network_read_buffer_kb(int kbytes);
  void invoke_set_segment_file_write_buffer_kb(int kbytes);
  void invoke_set_next_download_timer_params(int min_seconds, int max_seconds, int increase_step_msecs, int increase_reset_after_seconds);
  void invoke_set_progress_check_interval(int seconds);
  void invoke_set_gdrive_settings(const gdrive_settings & gd_prefs);

signals:
  void load_state_finished(const QString & error_text);
  void download_finished(const QString & error_text);
  void last_item_in_group_finished(const QString & filename);

private:
  void set_download_directory(const QString & dir);
  void set_download_lists_dir(const QString & dir);
  void set_links_file_dir(const QString & dir);
  void set_max_parallel_connections(int max);
  void set_user_reconnect_interval(int minutes);
  void set_speed_check_interval(int seconds);
  void set_min_avg_speed_before_reconnect(int mbps);
  void set_user_agent_for_https_connections(const QString & user_agent);
  void set_max_seconds_before_abort_inactive_connection(int seconds);
  void set_max_parallel_link_checks(int max);
  void set_max_seconds_before_check_link_again_on_error(int seconds);
  void set_max_segments_per_file(int segments);
  void set_min_segment_size_mb(int mbytes);
  void set_segment_network_read_buffer_kb(int kbytes);
  void set_segment_file_write_buffer_kb(int kbytes);
  void set_next_download_timer_params(int min_seconds, int max_seconds, int increase_step_msecs, int increase_reset_after_seconds);
  void set_progress_check_interval(int seconds);
  void set_gdrive_settings(const gdrive_settings & gd_prefs);

  void load_state(const finished_downloads_settings & fin_dl_settings);
  void stop();
  void start();
  void start_next_download();
  void refresh_link_info();
  void pre_job_finished(download_item * item, bool success, bool should_pause_item);
  void refresh_link_info_finished(download_item * item);
  void start_file_download(download_item * item);
  void file_download_finished(dl_item_downloader * sender, const QString & err_text, download_item * item);
  void file_download_success(dl_item_downloader * sender, download_item * item);
  void check_download_speed();
  void check_progress();
  void check_remaining_stats();
  void reconnect_if_needed();
  double calc_rolling_average(double last_avg, double new_sample, int count);

  int max_download_connections;
  int download_connections;
  QNetworkAccessManager * nam;
  link_manager * lnk_mgr;
  increasing_timer * start_next_download_timer;
  QTimer * check_download_speed_timer;
  QTimer * check_progress_timer;
  QTimer * check_remaining_stats_timer;
  QTimer * user_reconnect_timer;

  QSet<dl_item_downloader *> downloaders;
  QString download_dir;
  QString download_lists_dir;
  double average_speed;
  int average_speed_counter;
  int min_avg_speed_before_reconnect_mbps;
  int max_segments_per_file;
  int min_segment_size_bytes;
  int segment_network_read_buffer_bytes;
  int segment_file_write_buffer_bytes;

  pre_post_job * dl_pre_post_job;
};

#endif // DOWNLOAD_MANAGER_H
