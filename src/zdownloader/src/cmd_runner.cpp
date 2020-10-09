#include "cmd_runner.h"
#include "app_settings.h"
#include "unpack.h"
#include "fn_scheduler.h"
#include <meta_object_ext.h>
#include <QCoreApplication>
#include <QTimer>
#include <QDir>
#include <QProcess>
#include <csignal>
#include <qdebugex.h>

cmd_runner::cmd_runner(app_settings * app_prefs)
  :app_set(app_prefs),
   zdown_net(new zdownloader_net_lib(false, this)),
   init_finished(false),
   shutdown_system_on_finish(false),
   all_downloads_finished(false),
   unpack_mod(new unpack(this)),
   start_download_sched(nullptr),
   stop_download_sched(nullptr)
{
  connect(zdown_net, &zdownloader_net_lib::init_finished, this, &cmd_runner::zdown_net_init_finished);
  connect(unpack_mod, &unpack::unpack_queue_finished, this, &cmd_runner::unpack_queue_finished, Qt::QueuedConnection);
  zdown_net->start();
}

cmd_runner::~cmd_runner()
{
  delete start_download_sched;
  delete stop_download_sched;
  delete app_set;

  qDebug() << "destructor cmd_runner";
}

void cmd_runner::unix_signal_received(int signal)
{
#ifndef SYSTEM_WINDOWS
  if(init_finished == false)
    return;

  switch(signal)
  {
    case SIGUSR1:
      reload_settings();
      break;
    case SIGUSR2:
      break;
    default:
      exit_app(false);
  }
#else
  Q_UNUSED(signal)
#endif
}

void cmd_runner::win_signal_received()
{
  if(init_finished == false)
    return;

  qDebug() << "win signal received";
  exit_app(false);
}

void cmd_runner::zdown_net_init_finished()
{
  init_finished = true;

  if(apply_settings())
  {
    connect(zdown_net->get_dl_mgr(), &download_manager::load_state_finished, this, &cmd_runner::load_state_finished, Qt::QueuedConnection);
    connect(zdown_net->get_dl_mgr(), &download_manager::download_finished, this, &cmd_runner::download_finished, Qt::QueuedConnection);
    connect(zdown_net->get_dl_mgr(), &download_manager::last_item_in_group_finished, this, &cmd_runner::downloader_last_item_in_group_finished, Qt::QueuedConnection);

    meta_object_ext::invoke_async(this, &cmd_runner::load_state);
  }
  else
  {
    exit_app(false);
  }
}

void cmd_runner::reload_settings()
{
  qDebug() << "RELOADING SETTINGS...";

  if(app_set)
  {
    app_set->load();
    apply_settings();
  }
}

bool cmd_runner::apply_settings()
{
  qDebug() << "SETTINGS BEGIN";
  const QString general_val_msg = app_set->general.validate();
  if(general_val_msg.isEmpty() == false)
  {
    qDebug() << "settings error:";
    qDebug() << general_val_msg;
    return false;
  }

  qDebug() << "Common:"
           << "\n" << "download_directory:" << app_set->general.download_directory
           << "\n" << "user_agent_for_https_connections:" << app_set->general.user_agent_for_https_connections
           << "\n" << "shutdown_system_on_finish:" << app_set->general.shutdown_system_on_finish
           << "\n" << "max_seconds_before_reset_inactive_connection:" << app_set->general.max_seconds_before_reset_inactive_connection
           << "\n" << "download_lists_directory:" << app_set->general.download_lists_directory
           << "\n" << "links_file_directory:" << app_set->general.links_file_directory;

  zdown_net->get_dl_mgr()->invoke_set_download_directory(app_set->general.download_directory);
  zdown_net->get_dl_mgr()->invoke_set_user_agent_for_https_connections(app_set->general.user_agent_for_https_connections);
  zdown_net->get_dl_mgr()->invoke_set_max_seconds_before_abort_inactive_connection(app_set->general.max_seconds_before_reset_inactive_connection);
  zdown_net->get_dl_mgr()->invoke_set_download_lists_dir(app_set->general.download_lists_directory);
  zdown_net->get_dl_mgr()->invoke_set_links_file_dir(app_set->general.links_file_directory);

  const QString link_chk_val_msg = app_set->link_checker.validate();
  if(link_chk_val_msg.isEmpty() == false)
  {
    qDebug() << "settings error:";
    qDebug() << link_chk_val_msg;
    return false;
  }

  qDebug() << "LinkChecker:"
           << "\n" << "max_parallel_link_checks:" << app_set->link_checker.max_parallel_link_checks
           << "\n" << "max_seconds_before_check_link_again_on_error:" << app_set->link_checker.max_seconds_before_check_link_again_on_error;

  zdown_net->get_dl_mgr()->invoke_set_max_parallel_link_checks(app_set->link_checker.max_parallel_link_checks);
  zdown_net->get_dl_mgr()->invoke_set_max_seconds_before_check_link_again_on_error(app_set->link_checker.max_seconds_before_check_link_again_on_error);

  const QString downloader_val_msg = app_set->downloader.validate();
  if(downloader_val_msg.isEmpty() == false)
  {
    qDebug() << "settings error:";
    qDebug() << downloader_val_msg;
    return false;
  }

  qDebug() << "Downloader:"
           << "\n" << "max_parallel_file_downloads:" << app_set->downloader.max_parallel_file_downloads
           << "\n" << "max_segments_per_file:" << app_set->downloader.max_segments_per_file
           << "\n" << "min_segment_size_mb:" << app_set->downloader.min_segment_size_mb
           << "\n" << "segment_network_read_buffer_kb:" << app_set->downloader.segment_network_read_buffer_kb
           << "\n" << "segment_file_write_buffer_kb:" << app_set->downloader.segment_file_write_buffer_kb
           << "\n" << "min_seconds_before_next_download:" << app_set->downloader.min_seconds_before_next_download
           << "\n" << "max_seconds_before_next_download:" << app_set->downloader.max_seconds_before_next_download
           << "\n" << "next_download_increase_step_msecs:" << app_set->downloader.next_download_increase_step_msecs
           << "\n" << "next_download_increase_reset_after_seconds:" << app_set->downloader.next_download_increase_reset_after_seconds
           << "\n" << "progress_check_interval_seconds:" << app_set->downloader.progress_check_interval_seconds
           << "\n" << "speed_check_interval_seconds:" << app_set->downloader.speed_check_interval_seconds
           << "\n" << "user_reconnect_interval_minutes:" << app_set->downloader.user_reconnect_interval_minutes
           << "\n" << "min_avg_speed_before_reconnect_mbps:" << app_set->downloader.min_avg_speed_before_reconnect_mbps;


  zdown_net->get_dl_mgr()->invoke_set_max_parallel_connections(app_set->downloader.max_parallel_file_downloads);
  zdown_net->get_dl_mgr()->invoke_set_max_segments_per_file(app_set->downloader.max_segments_per_file);
  zdown_net->get_dl_mgr()->invoke_set_min_segment_size_mb(app_set->downloader.min_segment_size_mb);
  zdown_net->get_dl_mgr()->invoke_set_segment_network_read_buffer_kb(app_set->downloader.segment_network_read_buffer_kb);
  zdown_net->get_dl_mgr()->invoke_set_segment_file_write_buffer_kb(app_set->downloader.segment_file_write_buffer_kb);
  zdown_net->get_dl_mgr()->invoke_set_next_download_timer_params(app_set->downloader.min_seconds_before_next_download,
                                                                          app_set->downloader.max_seconds_before_next_download,
                                                                          app_set->downloader.next_download_increase_step_msecs,
                                                                          app_set->downloader.next_download_increase_reset_after_seconds);
  zdown_net->get_dl_mgr()->invoke_set_progress_check_interval(app_set->downloader.progress_check_interval_seconds);
  zdown_net->get_dl_mgr()->invoke_set_speed_check_interval(app_set->downloader.speed_check_interval_seconds);
  zdown_net->get_dl_mgr()->invoke_set_user_reconnect_interval(app_set->downloader.user_reconnect_interval_minutes);
  zdown_net->get_dl_mgr()->invoke_set_min_avg_speed_before_reconnect(app_set->downloader.min_avg_speed_before_reconnect_mbps);

  const QString out_msgs_val_msg = app_set->out_msgs.validate();
  if(out_msgs_val_msg.isEmpty() == false)
  {
    qDebug() << "OutputMessages settings error:";
    qDebug() << out_msgs_val_msg;
    return false;
  }

  qDebug() << "OutputMessages:"
           << "\n" << "log_to_file_enabled:" << app_set->out_msgs.log_to_file_enabled
           << "\n" << "log_directory:" << app_set->out_msgs.log_directory
           << "\n" << "timestamp_format:" << app_set->out_msgs.timestamp_format
           << "\n" << "log_max_rotate_kb:" << app_set->out_msgs.log_max_rotate_kb
           << "\n" << "log_max_archive_files:" << app_set->out_msgs.log_max_archive_files;

  const QString fin_dls_val_msg = app_set->fin_dls.validate();
  if(fin_dls_val_msg.isEmpty() == false)
  {
    qDebug() << "FinishedDownloads settings error:";
    qDebug() << fin_dls_val_msg;
    return false;
  }

  qDebug() << "FinishedDownloads:"
           << "\n" << "log_to_file_enabled:" << app_set->fin_dls.log_to_file_enabled
           << "\n" << "timestamp_format:" << app_set->fin_dls.timestamp_format
           << "\n" << "log_max_rotate_kb:" << app_set->fin_dls.log_max_rotate_kb
           << "\n" << "log_max_archive_files:" << app_set->fin_dls.log_max_archive_files;

  qDebug() << "Unpack:"
           << "\n" << "unpack_after_download:" << app_set->unpack.unpack_after_download
           << "\n" << "pause_download_before_unpack:" << app_set->unpack.pause_download_before_unpack
           << "\n" << "delete_archive_files_after_unpack:" << app_set->unpack.delete_archive_files_after_unpack
           << "\n" << "unrar_binary:" << app_set->unpack.unrar_binary
           << "\n" << "7z_binary:" << app_set->unpack.seven_zip_binary
           << "\n" << "use_7z_to_unpack_rar_files:" << app_set->unpack.use_7z_to_unpack_rar_files;

  const QString unpack_pwds_val_msg = app_set->unpack.validate();
  if(unpack_pwds_val_msg.isEmpty() == false)
  {
    qDebug() << "UnpackPasswords settings error:";
    qDebug() << unpack_pwds_val_msg;
    return false;
  }

  qDebug() << "UnpackPasswords:"
           << "\n" << app_set->unpack.passwords;

  if(apply_unpacklog_settings() == false)
    return false;

  qDebug() << "NetworkProxy:"
           << "\n" << "enable_network_proxy:" << app_set->net_proxy.enable_network_proxy
           << "\n" << "proxy_type:" << ((app_set->net_proxy.proxy_type == QNetworkProxy::Socks5Proxy) ? QString("socks5") : QString("http"))
           << "\n" << "hostname:" << app_set->net_proxy.hostname
           << "\n" << "port:" << app_set->net_proxy.port
           << "\n" << "username:" << app_set->net_proxy.username
           << "\n" << "password:" << app_set->net_proxy.password;

  // ======
  if(apply_scheduler_settings() == false)
    return false;

  if(apply_gdrive_settings() == false)
    return false;

  qDebug() << "SETTINGS END";

  shutdown_system_on_finish = app_set->general.shutdown_system_on_finish;
  unpack_mod->set_settings(app_set->unpack);
  unpack_mod->set_working_directory(app_set->general.download_directory);
  set_application_proxy(app_set->net_proxy);

  return true;
}

bool cmd_runner::apply_unpacklog_settings()
{
  const QString val_msg = app_set->unpacklog.validate();
  if(val_msg.isEmpty() == false)
  {
    qDebug() << "UnpackLog settings error:";
    qDebug() << val_msg;
    return false;
  }
  if(app_set->unpacklog.log_to_file_enabled)
  {
    qDebug() << "UnpackLog:"
             << "\n" << "log_to_file_enabled:" << app_set->unpacklog.log_to_file_enabled
             << "\n" << "log_directory:" << app_set->unpacklog.log_directory
             << "\n" << "timestamp_format:" << app_set->unpacklog.timestamp_format
             << "\n" << "log_max_rotate_kb:" << app_set->unpacklog.log_max_rotate_kb
             << "\n" << "log_max_archive_files:" << app_set->unpacklog.log_max_archive_files;

    unpack_mod->set_log_settings(app_set->unpacklog);
  }

  return true;
}

bool cmd_runner::apply_scheduler_settings()
{
  const QString sched_val_msg = app_set->scheduler.validate();
  if(sched_val_msg.isEmpty() == false)
  {
    qDebug() << "Scheduler settings error:";
    qDebug() << sched_val_msg;
    return false;
  }
  if(app_set->scheduler.scheduler_enabled)
  {
    qDebug() << "Scheduler:"
             << "\n" << "start_download_at:" << app_set->scheduler.start_download_at.time()
             << "\n" << "stop_download_at:" << app_set->scheduler.stop_download_at.time();

    if(start_download_sched == nullptr)
      start_download_sched = new fn_scheduler;
    if(stop_download_sched == nullptr)
      stop_download_sched = new fn_scheduler;

    start_download_sched->set_activation_time(app_set->scheduler.start_download_at.time());
    stop_download_sched->set_activation_time(app_set->scheduler.stop_download_at.time());

    connect(start_download_sched, &fn_scheduler::activated, this, &cmd_runner::start_downloads, Qt::UniqueConnection);
    connect(stop_download_sched, &fn_scheduler::activated, this, &cmd_runner::scheduler_stop_triggered, Qt::UniqueConnection);
  }

  return true;
}

bool cmd_runner::apply_gdrive_settings()
{
  const QString val_msg = app_set->gdrive.validate();
  if(val_msg.isEmpty() == false)
  {
    qDebug() << "GDrive settings error:";
    qDebug() << val_msg;
    return false;
  }
  if(app_set->gdrive.use_gdrive_api)
  {
    qDebug() << "GDrive:"
             << "\n" << "use_gdrive_api:" << app_set->gdrive.use_gdrive_api
             << "\n" << "google_service_account_json_file:" << app_set->gdrive.google_service_account_json_file;


    zdown_net->get_dl_mgr()->invoke_set_gdrive_settings(app_set->gdrive);
  }

  return true;
}

void cmd_runner::set_application_proxy(const net_proxy_settings & proxy_prefs)
{
#ifdef DEBUG_PROXY
  QNetworkProxy proxy(QNetworkProxy::Socks5Proxy, "127.0.0.1", 8889);
  QNetworkProxy::setApplicationProxy(proxy);
#else
  if(proxy_prefs.enable_network_proxy)
  {
    QNetworkProxy proxy(proxy_prefs.proxy_type, proxy_prefs.hostname, proxy_prefs.port, proxy_prefs.username, proxy_prefs.password);
    QNetworkProxy::setApplicationProxy(proxy);
  }
  else
  {
    QNetworkProxy::setApplicationProxy(QNetworkProxy(QNetworkProxy::NoProxy));
  }
#endif
}

void cmd_runner::load_state()
{
  qDebug() << "Loading state";

  unpack_mod->load_queue();
  if(zdown_net->get_dl_mgr())
    zdown_net->get_dl_mgr()->invoke_load_state(app_set->fin_dls);

  delete app_set;
  app_set = nullptr;
}

void cmd_runner::load_state_finished(const QString & /*error_text*/)
{
  qDebug() << "Load state finished";

  if(start_download_sched)
    start_download_sched->start();
  else
    QTimer::singleShot(0, this, &cmd_runner::start_downloads);
}

void cmd_runner::start_downloads()
{
  if(zdown_net->get_dl_mgr() == nullptr)
    return;

  if(unpack_mod->settings().unpack_after_download)
  {
    if(unpack_mod->is_queue_empty() == false)
    {
      if(unpack_mod->settings().pause_download_before_unpack == false)
        zdown_net->get_dl_mgr()->invoke_start();

      meta_object_ext::invoke_async(unpack_mod, &unpack::start_unpack_first_item);
      return;
    }
  }

  zdown_net->get_dl_mgr()->invoke_start();

  if(stop_download_sched)
  {
    stop_download_sched->start();
    start_download_sched->deleteLater();
    start_download_sched = nullptr;
  }
}

void cmd_runner::scheduler_stop_triggered()
{
  if(shutdown_system_on_finish)
    schedule_system_shutdown();

  exit_app(false);
}

void cmd_runner::download_finished(const QString & /*error_text*/)
{
  all_downloads_finished = true;

  if(unpack_mod->is_queue_empty() || unpack_mod->has_fatal_error_occured())
    exit_app(true);
  else
    qDebug() << "UNPACK: in progress...";
}

void cmd_runner::downloader_last_item_in_group_finished(const QString & filename)
{
  if(unpack_mod->settings().unpack_after_download)
  {
    if(unpack_mod->settings().pause_download_before_unpack && zdown_net->get_dl_mgr())
      zdown_net->get_dl_mgr()->invoke_stop();

    unpack_mod->add_to_queue(filename);
    meta_object_ext::invoke_async(unpack_mod, &unpack::start_unpack_first_item);
  }
}

void cmd_runner::unpack_queue_finished()
{
  if(all_downloads_finished)
  {
    exit_app(true);
    return;
  }

  if(unpack_mod->settings().pause_download_before_unpack && zdown_net->get_dl_mgr())
    zdown_net->get_dl_mgr()->invoke_start();
}

void cmd_runner::schedule_system_shutdown()
{
  qDebug() << "Scheduling system shutdown";

#ifndef SYSTEM_WINDOWS
  const int exit_code = QProcess::execute("sudo", { "shutdown", "--no-wall", "-P", "+2" });
#else
  const int exit_code = QProcess::execute("shutdown", { "/s", "/t", "120"});
#endif
  qDebug() << "Scheduling system shutdown exit code:" << exit_code;
}

void cmd_runner::exit_app(bool allow_schedule_shutdown)
{  
  if(init_finished == false)
    return;

  init_finished = false; // allow only one exit_app
  qDebug() << "Exiting...";
  unpack_mod->stop_unpack();

  if(shutdown_system_on_finish && all_downloads_finished && allow_schedule_shutdown)
  {
    schedule_system_shutdown();
  }

  QTimer::singleShot(0, this, &cmd_runner::stop_zd_net);
}

void cmd_runner::stop_zd_net()
{
  connect(zdown_net, &zdownloader_net_lib::finished, this, &cmd_runner::zdown_net_finished, Qt::QueuedConnection);
  zdown_net->quit();
  zdown_net->wait();
}

void cmd_runner::zdown_net_finished()
{
  zdown_net->deleteLater();
  QTimer::singleShot(0, qApp, &QCoreApplication::quit);
}

