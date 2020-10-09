#include "app_settings.h"
#include <botan_wrapper.h>
#include <QSettings>
#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <qdebugex.h>

void app_settings::load()
{
  QSettings s(settings_file_path, QSettings::IniFormat);
  s.setIniCodec("UTF-8");

  load_general_settings(&s);
  load_links_checker_settings(&s);
  load_downloader_settings(&s);
  load_output_messages_settings(&s);
  load_finished_downloads_settings(&s);
  load_unpack_settings(&s);
  load_net_proxy_settings(&s);
  load_scheduler_settings(&s);
  load_gdrive_settings(&s);
  load_upacklog_settings(&s);
}

void app_settings::load_general_settings(QSettings * s)
{
  s->beginGroup("Common");
  general.download_directory = s->value("download_directory").toString();
  general.user_agent_for_https_connections = s->value("user_agent_for_https_connections").toString();
  general.shutdown_system_on_finish = s->value("shutdown_system_on_finish").toBool();
  general.max_seconds_before_reset_inactive_connection = s->value("max_seconds_before_reset_inactive_connection").toInt();
  general.download_lists_directory = s->value("download_lists_directory").toString();
  general.links_file_directory = s->value("links_file_directory", ".").toString();
  s->endGroup();

  general.correct_dirs();
}

void app_settings::load_links_checker_settings(QSettings * s)
{
  s->beginGroup("LinkChecker");
  link_checker.max_parallel_link_checks = s->value("max_parallel_link_checks").toInt();
  link_checker.max_seconds_before_check_link_again_on_error = s->value("max_seconds_before_check_link_again_on_error").toInt();
  s->endGroup();
}

void app_settings::load_downloader_settings(QSettings * s)
{
  s->beginGroup("Downloader");
  downloader.max_parallel_file_downloads = s->value("max_parallel_file_downloads").toInt();
  downloader.max_segments_per_file = s->value("max_segments_per_file").toInt();
  downloader.min_segment_size_mb = s->value("min_segment_size_mb").toInt();
  downloader.segment_network_read_buffer_kb = s->value("segment_network_read_buffer_kb").toInt();
  downloader.segment_file_write_buffer_kb = s->value("segment_file_write_buffer_kb").toInt();
  downloader.min_seconds_before_next_download = s->value("min_seconds_before_next_download").toInt();
  downloader.max_seconds_before_next_download = s->value("max_seconds_before_next_download").toInt();
  downloader.next_download_increase_step_msecs = s->value("next_download_increase_step_msecs").toInt();
  downloader.next_download_increase_reset_after_seconds = s->value("next_download_increase_reset_after_seconds").toInt();
  downloader.progress_check_interval_seconds = s->value("progress_check_interval_seconds").toInt();
  downloader.speed_check_interval_seconds = s->value("speed_check_interval_seconds").toInt();
  downloader.user_reconnect_interval_minutes = s->value("user_reconnect_interval_minutes").toInt();
  downloader.min_avg_speed_before_reconnect_mbps = s->value("min_avg_speed_before_reconnect_mbps").toInt();
  s->endGroup();
}

void app_settings::load_output_messages_settings(QSettings * s)
{
  s->beginGroup("OutputMessages");
  out_msgs.log_to_file_enabled = s->value("log_to_file_enabled").toBool();
  out_msgs.log_directory = s->value("log_directory").toString();
  out_msgs.timestamp_format = s->value("timestamp_format").toString();
  out_msgs.log_max_rotate_kb = s->value("log_max_rotate_kb").toInt();
  out_msgs.log_max_archive_files = s->value("log_max_archive_files").toInt();
  s->endGroup();

  out_msgs.correct_dirs();
}

void app_settings::load_finished_downloads_settings(QSettings *s)
{
  s->beginGroup("FinishedDownloads");
  fin_dls.log_to_file_enabled = s->value("log_to_file_enabled").toBool();
  fin_dls.timestamp_format = s->value("timestamp_format").toString();
  fin_dls.log_max_rotate_kb = s->value("log_max_rotate_kb").toInt();
  fin_dls.log_max_archive_files = s->value("log_max_archive_files").toInt();
  s->endGroup();
}

void app_settings::load_unpack_settings(QSettings * s)
{
  s->beginGroup("Unpack");
  unpack.unpack_after_download = s->value("unpack_after_download").toBool();
  unpack.pause_download_before_unpack = s->value("pause_download_before_unpack").toBool();
  unpack.delete_archive_files_after_unpack = s->value("delete_archive_files_after_unpack").toBool();
  unpack.unrar_binary = s->value("unrar_binary").toString();
  unpack.seven_zip_binary = s->value("7z_binary").toString();
  unpack.use_7z_to_unpack_rar_files = s->value("use_7z_to_unpack_rar_files").toBool();
  s->endGroup();

  unpack.passwords.clear();
  const int size = s->beginReadArray("UnpackPasswords");
  if(size > 20)
  {
    unpack.unpack_passwords_size_error = true;
    return;
  }

  unpack.unpack_passwords_size_error = false;
  for(int i = 0; i < size; ++i)
  {
    s->setArrayIndex(i);
    const QString val = s->value("pass").toString();
    if(val.isEmpty() == false)
      unpack.passwords.append(val);
  }
  s->endArray();
}

void app_settings::load_net_proxy_settings(QSettings * s)
{
  s->beginGroup("NetworkProxy");
  net_proxy.enable_network_proxy = s->value("enable_network_proxy").toBool();
  const QString proxy_type_str = s->value("proxy_type").toString();
  if(proxy_type_str == "socks5")
    net_proxy.proxy_type = QNetworkProxy::Socks5Proxy;
  else
  {
    if(proxy_type_str == "http")
      net_proxy.proxy_type = QNetworkProxy::HttpProxy;
  }
  net_proxy.hostname = s->value("hostname").toString();
  net_proxy.port = s->value("port").value<quint16>();
  net_proxy.username = s->value("username").toString();
  net_proxy.password = s->value("password").toString();
  s->endGroup();
}

void app_settings::load_scheduler_settings(QSettings * s)
{
  s->beginGroup("Scheduler");
  scheduler.scheduler_enabled = s->value("scheduler_enabled").toBool();
  scheduler.start_download_at = QDateTime::fromString(s->value("start_download_at").toString(), "hh:mm");
  scheduler.stop_download_at = QDateTime::fromString(s->value("stop_download_at").toString(), "hh:mm");
  s->endGroup();
}

void app_settings::load_gdrive_settings(QSettings * s)
{
  s->beginGroup("GDrive");
  gdrive.use_gdrive_api = s->value("use_gdrive_api").toBool();
  gdrive.google_service_account_json_file = s->value("google_service_account_json_file").toString();
  s->endGroup();

  if(gdrive.use_gdrive_api && gdrive.google_service_account_json_file.isEmpty() == false)
  {
    QFile file(gdrive.google_service_account_json_file);
    if(file.size() > (1 * 1024 * 1024))
    {
      gdrive.json_file_error = "Json file is too big(max 1MB) " + gdrive.google_service_account_json_file;
      return;
    }
    if(file.open(QIODevice::ReadOnly) == false)
    {
      gdrive.json_file_error = "Could not open json file " + gdrive.google_service_account_json_file;
      return;
    }

    const QByteArray file_content = file.readAll();
    const QJsonDocument jdoc = QJsonDocument::fromJson(file_content);
    const QJsonObject jobj = jdoc.object();
    gdrive.client_email = jobj.value("client_email").toString();
    gdrive.private_key = jobj.value("private_key").toString();
  }
}

void app_settings::load_upacklog_settings(QSettings * s)
{
  s->beginGroup("UnpackLog");
  unpacklog.log_to_file_enabled = s->value("log_to_file_enabled").toBool();
  unpacklog.log_directory = s->value("log_directory").toString();
  unpacklog.timestamp_format = s->value("timestamp_format").toString();
  unpacklog.log_max_rotate_kb = s->value("log_max_rotate_kb").toInt();
  unpacklog.log_max_archive_files = s->value("log_max_archive_files").toInt();
  s->endGroup();

  unpacklog.correct_dirs();
}
