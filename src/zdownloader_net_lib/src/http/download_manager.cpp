#include "download_manager.h"
#include "link_manager.h"
#include "dl_item_downloader.h"
#include "pre_post_job.h"
#include "abstract_http_request.h"
#include "../model/download_item.h"
#include "../settings/finished_downloads_settings.h"
#include "../settings/gdrive_settings.h"
#include "../utils/jwt.h"
#include <meta_object_ext.h>
#include <increasing_timer.h>
#include <string_utils.h>
#include <QTimer>
#include <qdebugex.h>

download_manager::download_manager(QNetworkAccessManager * global_nam, QObject * parent)
  :QObject(parent),
   max_download_connections(2),
   download_connections(0),
   nam(global_nam),
   lnk_mgr(new link_manager(global_nam, this)),
   start_next_download_timer(new increasing_timer(this)),
   check_download_speed_timer(new QTimer(this)),
   check_progress_timer(new QTimer(this)),
   check_remaining_stats_timer(new QTimer(this)),
   user_reconnect_timer(new QTimer(this)),
   average_speed(0.0),
   average_speed_counter(0),
   min_avg_speed_before_reconnect_mbps(20),
   max_segments_per_file(1),
   min_segment_size_bytes(4),
   segment_network_read_buffer_bytes(1024 * 1024),
   segment_file_write_buffer_bytes(1024 * 1024),
   dl_pre_post_job(nullptr)
{
  connect(lnk_mgr, &link_manager::load_state_finished, this, &download_manager::load_state_finished);
  connect(lnk_mgr, &link_manager::update_download_item_finished, this, &download_manager::refresh_link_info_finished);

  start_next_download_timer->set_reset_after(8000);
  start_next_download_timer->set_increase_range(0, 7000);
  start_next_download_timer->set_increase_step(250);
  connect(start_next_download_timer, &increasing_timer::timeout, this, &download_manager::start_next_download);

  check_download_speed_timer->setInterval(5000);
  check_download_speed_timer->setSingleShot(true);
  connect(check_download_speed_timer, &QTimer::timeout, this, &download_manager::check_download_speed);

  check_progress_timer->setInterval(5000);
  check_progress_timer->setSingleShot(true);
  connect(check_progress_timer, &QTimer::timeout, this, &download_manager::check_progress);

  check_remaining_stats_timer->setInterval(60 * 1000);
  check_remaining_stats_timer->setSingleShot(true);
  connect(check_remaining_stats_timer, &QTimer::timeout, this, &download_manager::check_remaining_stats);

  user_reconnect_timer->setInterval(30 * 1000);
  user_reconnect_timer->setSingleShot(true);
  connect(user_reconnect_timer, &QTimer::timeout, this, &download_manager::reconnect_if_needed);
}

download_manager::~download_manager()
{
  for(auto dl : downloaders)
    dl->save_and_abort_download();

  lnk_mgr->save_state(download_dir);

  start_next_download_timer->stop();
  check_download_speed_timer->stop();
  check_progress_timer->stop();
  user_reconnect_timer->stop();
  qDeleteAll(downloaders);
}

void download_manager::invoke_load_state(const finished_downloads_settings & fin_dl_settings)
{
  meta_object_ext::invoke_async(this, &download_manager::load_state, fin_dl_settings);
}

void download_manager::invoke_start()
{
  meta_object_ext::invoke_async(this, &download_manager::start);
}

void download_manager::invoke_stop()
{
  meta_object_ext::invoke_async(this, &download_manager::stop);
}

void download_manager::invoke_set_download_directory(const QString & dir)
{
  meta_object_ext::invoke_async(this, &download_manager::set_download_directory, dir);
}

void download_manager::invoke_set_download_lists_dir(const QString &dir)
{
  meta_object_ext::invoke_async(this, &download_manager::set_download_lists_dir, dir);
}

void download_manager::invoke_set_links_file_dir(const QString & dir)
{
  meta_object_ext::invoke_async(this, &download_manager::set_links_file_dir, dir);
}

void download_manager::invoke_set_max_parallel_connections(int max)
{
  meta_object_ext::invoke_async(this, &download_manager::set_max_parallel_connections, max);
}

void download_manager::invoke_set_user_reconnect_interval(int minutes)
{
  meta_object_ext::invoke_async(this, &download_manager::set_user_reconnect_interval, minutes);
}

void download_manager::invoke_set_speed_check_interval(int seconds)
{
  meta_object_ext::invoke_async(this, &download_manager::set_speed_check_interval, seconds);
}

void download_manager::invoke_set_min_avg_speed_before_reconnect(int mbps)
{
  meta_object_ext::invoke_async(this, &download_manager::set_min_avg_speed_before_reconnect, mbps);
}

void download_manager::invoke_set_user_agent_for_https_connections(const QString & user_agent)
{
  meta_object_ext::invoke_async(this, &download_manager::set_user_agent_for_https_connections, user_agent);
}

void download_manager::invoke_set_max_seconds_before_abort_inactive_connection(int seconds)
{
  meta_object_ext::invoke_async(this, &download_manager::set_max_seconds_before_abort_inactive_connection, seconds);
}

void download_manager::invoke_set_max_parallel_link_checks(int max)
{
  meta_object_ext::invoke_async(this, &download_manager::set_max_parallel_link_checks, max);
}

void download_manager::invoke_set_max_seconds_before_check_link_again_on_error(int seconds)
{
  meta_object_ext::invoke_async(this, &download_manager::set_max_seconds_before_check_link_again_on_error, seconds);
}

void download_manager::invoke_set_max_segments_per_file(int segments)
{
  meta_object_ext::invoke_async(this, &download_manager::set_max_segments_per_file, segments);
}

void download_manager::invoke_set_min_segment_size_mb(int mbytes)
{
  meta_object_ext::invoke_async(this, &download_manager::set_min_segment_size_mb, mbytes);
}

void download_manager::invoke_set_segment_network_read_buffer_kb(int kbytes)
{
  meta_object_ext::invoke_async(this, &download_manager::set_segment_network_read_buffer_kb, kbytes);
}

void download_manager::invoke_set_segment_file_write_buffer_kb(int kbytes)
{
  meta_object_ext::invoke_async(this, &download_manager::set_segment_file_write_buffer_kb, kbytes);
}

void download_manager::invoke_set_next_download_timer_params(int min_seconds, int max_seconds, int increase_step_msecs,
                                                             int increase_reset_after_seconds)
{
  meta_object_ext::invoke_async(this, &download_manager::set_next_download_timer_params, min_seconds, max_seconds,
                                increase_step_msecs, increase_reset_after_seconds);
}

void download_manager::invoke_set_progress_check_interval(int seconds)
{
  meta_object_ext::invoke_async(this, &download_manager::set_progress_check_interval, seconds);
}

void download_manager::invoke_set_gdrive_settings(const gdrive_settings & gd_prefs)
{
  meta_object_ext::invoke_async(this, &download_manager::set_gdrive_settings, gd_prefs);
}

void download_manager::set_download_directory(const QString & dir)
{
  download_dir = dir;
}

void download_manager::set_download_lists_dir(const QString & dir)
{
  download_lists_dir = dir;
}

void download_manager::set_links_file_dir(const QString & dir)
{
  lnk_mgr->set_links_file_dir(dir);
}

void download_manager::set_max_parallel_connections(int max)
{
  max_download_connections = max;
}

void download_manager::set_user_reconnect_interval(int minutes)
{
  user_reconnect_timer->setInterval(minutes * 60 * 1000);
}

void download_manager::set_speed_check_interval(int seconds)
{
  check_download_speed_timer->setInterval(seconds * 1000);
}

void download_manager::set_min_avg_speed_before_reconnect(int mbps)
{
  min_avg_speed_before_reconnect_mbps = mbps;
}

void download_manager::set_user_agent_for_https_connections(const QString & user_agent)
{
  abstract_http_request::set_user_agent(user_agent);
}

void download_manager::set_max_seconds_before_abort_inactive_connection(int seconds)
{
  abstract_http_request::set_max_seconds_before_abort_inactive_connection(seconds);
}

void download_manager::set_max_parallel_link_checks(int max)
{
  lnk_mgr->set_max_parallel_link_checks(max);
}

void download_manager::set_max_seconds_before_check_link_again_on_error(int seconds)
{
  lnk_mgr->set_max_seconds_before_check_link_again_on_error(seconds);
}

void download_manager::set_max_segments_per_file(int segments)
{
  max_segments_per_file = segments;
}

void download_manager::set_min_segment_size_mb(int mbytes)
{
  min_segment_size_bytes = mbytes * 1024 * 1024;
}

void download_manager::set_segment_network_read_buffer_kb(int kbytes)
{
  segment_network_read_buffer_bytes = kbytes * 1024;
}

void download_manager::set_segment_file_write_buffer_kb(int kbytes)
{
  segment_file_write_buffer_bytes = kbytes * 1024;
}

void download_manager::set_next_download_timer_params(int min_seconds, int max_seconds, int increase_step_msecs,
                                                      int increase_reset_after_seconds)
{
  start_next_download_timer->set_increase_range(min_seconds * 1000, max_seconds * 1000);
  start_next_download_timer->set_increase_step(increase_step_msecs);
  start_next_download_timer->set_reset_after(increase_reset_after_seconds * 1000);
}

void download_manager::set_progress_check_interval(int seconds)
{
  check_progress_timer->setInterval(seconds * 1000);
}

void download_manager::set_gdrive_settings(const gdrive_settings & gd_prefs)
{
  if(gd_prefs.use_gdrive_api && dl_pre_post_job == nullptr)
  {
    dl_pre_post_job = new pre_post_job(this);
    connect(dl_pre_post_job, &pre_post_job::pre_job_finished, this, &download_manager::pre_job_finished);

    jwt jwt_header;
    jwt_header.set_aud("https://oauth2.googleapis.com/token");
    jwt_header.set_scope("https://www.googleapis.com/auth/drive");
    jwt_header.set_iss(gd_prefs.client_email);
    jwt_header.set_private_key_pem(gd_prefs.private_key);
    dl_pre_post_job->set_jwt_header(jwt_header);
  }
}

void download_manager::load_state(const finished_downloads_settings & fin_dl_settings)
{
  if(dl_pre_post_job)
    lnk_mgr->set_use_gdrive_api(true);
  lnk_mgr->load_state(download_dir, download_lists_dir, fin_dl_settings);
}

void download_manager::stop()
{
  if(dl_pre_post_job)
    dl_pre_post_job->abort_all_connections();
  lnk_mgr->abort_dl_item_updater_connections();

  for(auto dl : downloaders)
  {
    lnk_mgr->get_download_queue().stop(dl->get_curr_dl_item());
    dl->save_and_abort_download();
  }

  start_next_download_timer->stop();
  check_download_speed_timer->stop();
  check_progress_timer->stop();
  check_remaining_stats_timer->stop();
  user_reconnect_timer->stop();

  qDeleteAll(downloaders);
  downloaders.clear();
  qDebug() << "Download stopped";
}

void download_manager::start()
{
  qDebug() << "Download started";
  if(lnk_mgr->get_download_queue().has_pending_items() == false)
  {
    qDebug() << "No pending items.";
    emit download_finished("");
    return;
  }
  check_remaining_stats();

  user_reconnect_timer->start();

  start_next_download();
}

void download_manager::start_next_download()
{
  if(lnk_mgr->get_download_queue().has_pending_items() == false)
  {
    qDebug() << "No pending items.";
    return;
  }

  refresh_link_info();
}

void download_manager::refresh_link_info()
{
  if(download_connections >= max_download_connections)
  {
    qDebug() << "refresh link connections limit";
    return;
  }

  ++download_connections;
  download_item * item = lnk_mgr->get_download_queue().get_next();
  if(dl_pre_post_job)
  {
    dl_pre_post_job->start_pre_job(item);
  }
  else
  {
    qDebug() << "Working items:" << download_connections;
    lnk_mgr->update_download_item(item);
  }
}

void download_manager::pre_job_finished(download_item * item, bool success, bool should_pause_item)
{
  if(should_pause_item)
  {
    lnk_mgr->get_download_queue().pause(item);
    qDebug() << "Item PAUSED:" << item->get_link();
    --download_connections;
    start_next_download_timer->start(0);
  }
  else
  {
    if(success)
    {
      qDebug() << "Working items:" << download_connections;
      dl_pre_post_job->set_access_token(item);
      lnk_mgr->update_download_item(item);
    }
    else
    {
      file_download_finished(nullptr, "pre_job error", item);
    }
  }
}

void download_manager::refresh_link_info_finished(service::fetch_error error_code, const QString & error_text, download_item * item)
{
  if(error_text.isEmpty() == false)
  {
    if(error_code == service::file_does_not_exists)
    {
      item->set_status(download_item::download_status_remote_file_does_not_exists);
      file_download_finished(nullptr, "", item);
    }
    else
      file_download_finished(nullptr, error_text, item);

    return;
  }

  start_file_download(item);
}

void download_manager::start_file_download(download_item * item)
{
  if(dl_pre_post_job)
    dl_pre_post_job->set_access_token(item);

  dl_item_downloader * downloader = new dl_item_downloader(nam);
  downloaders.insert(downloader);
  // do not remove Qt::QueuedConnection - crash occur if removed
  connect(downloader, &dl_item_downloader::item_download_finished, this, &download_manager::file_download_finished, Qt::QueuedConnection);
  downloader->set_download_dir(download_dir);
  downloader->start_download_item(item, max_segments_per_file, min_segment_size_bytes,
                                  segment_network_read_buffer_bytes, segment_file_write_buffer_bytes);
  check_download_speed_timer->start();
  check_progress_timer->start();

  const int restart_msec = start_next_download_timer->start();
  qDebug() << "NEW DOWNLOAD after" << restart_msec << "ms";
}

void download_manager::file_download_finished(dl_item_downloader * sender, const QString & err_text, download_item * item)
{
  if(err_text.isEmpty() == false)
    qDebug() << err_text;

  --download_connections;
  qDebug() << "Working items:" << download_connections;

  if(download_connections == 0)
  {
    check_download_speed_timer->stop();
    check_progress_timer->stop();
  }

  if(err_text.isEmpty() == false)
  {
    if(dl_pre_post_job)
      dl_pre_post_job->reset_in_use(item);
    lnk_mgr->get_download_queue().stop(item);
    const int restart_msec = start_next_download_timer->start();
    qDebug() << "Restart download after" << restart_msec << "ms";

    if(sender)
    {
      downloaders.remove(sender);
      sender->deleteLater();
    }
    return;
  }

  file_download_success(sender, item);
}

void download_manager::file_download_success(dl_item_downloader * sender, download_item * item)
{
  if(dl_pre_post_job && dl_pre_post_job->reset_in_use(item))
    lnk_mgr->get_download_queue().resume_first_paused();

  const QString last_filename_in_group = lnk_mgr->get_download_queue().finish(item);
  switch(item->get_status())
  {
    case download_item::download_status_remote_file_does_not_exists:
      qDebug() << "SKIPPING download. Remote file does not exists:" << item->get_filename();
      break;

    case download_item::download_status_finished_already_exists:
      qDebug() << "SKIPPING download. File already exists:" << item->get_filename();
      break;

    default:
    {
      if(sender)
        qDebug() << sender->get_current_progress_msg();

      if(last_filename_in_group.isEmpty() == false)
        emit last_item_in_group_finished(last_filename_in_group);
    }
  }

  lnk_mgr->get_download_queue().delete_item(item);

  if(lnk_mgr->get_download_queue().has_pending_items() == false)
  {
    if(lnk_mgr->get_download_queue().has_working_items() == false)
    {
      start_next_download_timer->stop();
      user_reconnect_timer->stop();
      check_remaining_stats_timer->stop();
      qDebug() << "Download queue empty. All downloads finished.";
      emit download_finished("");
    }
  }
  else
  {
    qDebug() << "FINISHED - STARTING NEW";
    start_next_download_timer->start();
  }

  if(sender)
  {
    downloaders.remove(sender);
    sender->deleteLater();
  }
}

void download_manager::check_download_speed()
{
  double speed_sum = 0;

  for(const auto * dl : downloaders)
  {
    speed_sum += dl->get_current_download_speed_mbps();
  }

  ++average_speed_counter;
  average_speed = calc_rolling_average(average_speed, speed_sum, average_speed_counter);

  qDebug() << "speed sum:" << QString::number(speed_sum, 'f', 2) << "AVG SPEED:" << QString::number(average_speed, 'f', 2) << "Mb/s";

  check_download_speed_timer->start();
}

void download_manager::check_progress()
{
  for(const auto * dl : downloaders)
  {
    qDebug() << dl->get_current_progress_msg();
  }

  check_progress_timer->start();
}

void download_manager::check_remaining_stats()
{
  for(auto * dl : downloaders)
  {
    dl->update_remaining_bytes();
  }

  const qint64 remaining_bytes = lnk_mgr->get_download_queue().get_sum_of_remaining_bytes();

  qDebug() << "REMAINING STATS:" << string_utils::bytes_to_human_readable_string(remaining_bytes)
           << "| item(s) left:" << lnk_mgr->get_download_queue().get_all_items_count();

  check_remaining_stats_timer->start();
}

void download_manager::reconnect_if_needed()
{
  if(average_speed <= min_avg_speed_before_reconnect_mbps
     && downloaders.isEmpty() == false)
  {
    qDebug() << "RECONNECTING - Avg speed" << QString::number(average_speed, 'f', 2) << "<=" << min_avg_speed_before_reconnect_mbps;

    check_progress_timer->stop();
    for(auto dl : downloaders)
    {
      dl->save_and_abort_download(true);
    }
  }

  average_speed = 0.0;
  average_speed_counter = 0;

  user_reconnect_timer->start();
}

double download_manager::calc_rolling_average(double last_avg, double new_sample, int count)
{
  if(count != 0)
  {
    last_avg -= last_avg / count;
    last_avg += new_sample / count;
  }
  else
  {
    last_avg = 0.0;
  }

  return last_avg;
}
