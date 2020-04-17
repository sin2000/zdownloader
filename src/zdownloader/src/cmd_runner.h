#ifndef CMD_RUNNER_H
#define CMD_RUNNER_H

#include "app_settings.h"
#include <zdownloader_net_lib.h>
#include <QObject>

class unpack;
class fn_scheduler;

class cmd_runner : public QObject
{
  Q_OBJECT
public:
  cmd_runner(app_settings * app_prefs);
  ~cmd_runner();
  void unix_signal_received(int signal);
  void win_signal_received();

private:
  void zdown_net_init_finished();
  void reload_settings();
  bool apply_settings();
  bool apply_unpacklog_settings();
  bool apply_scheduler_settings();
  bool apply_gdrive_settings();
  void set_application_proxy(const net_proxy_settings & proxy_prefs);

  void load_state();
  void load_state_finished(const QString & error_text);
  void start_downloads();
  void scheduler_stop_triggered();
  void download_finished(const QString & error_text);
  void downloader_last_item_in_group_finished(const QString & filename);
  void unpack_queue_finished();
  void schedule_system_shutdown();

  void exit_app(bool allow_schedule_shutdown);
  void stop_zd_net();
  void zdown_net_finished();

  app_settings * app_set;
  const QString log_dir = "./logs";
  zdownloader_net_lib * zdown_net;

  bool init_finished;
  bool shutdown_system_on_finish;
  bool all_downloads_finished;
  unpack * unpack_mod;

  fn_scheduler * start_download_sched;
  fn_scheduler * stop_download_sched;
};

#endif // CMD_RUNNER_H
