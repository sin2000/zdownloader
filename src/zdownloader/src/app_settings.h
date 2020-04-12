#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include "settings/general_settings.h"
#include "settings/links_checker_settings.h"
#include "settings/downloader_settings.h"
#include "settings/output_messages_settings.h"
#include "settings/unpack_settings.h"
#include "settings/net_proxy_settings.h"
#include "settings/scheduler_settings.h"
#include <settings/finished_downloads_settings.h>
#include <settings/gdrive_settings.h>

class QSettings;

class app_settings
{
public:
  void load();

  const QString settings_file_path = "./zdownloader.ini";

  general_settings general;
  links_checker_settings link_checker;
  downloader_settings downloader;
  output_messages_settings out_msgs;
  finished_downloads_settings fin_dls;
  unpack_settings unpack;
  net_proxy_settings net_proxy;
  scheduler_settings scheduler;
  gdrive_settings gdrive;

private:
  void load_general_settings(QSettings * s);
  void load_links_checker_settings(QSettings * s);
  void load_downloader_settings(QSettings * s);
  void load_output_messages_settings(QSettings * s);
  void load_finished_downloads_settings(QSettings * s);
  void load_unpack_settings(QSettings * s);
  void load_net_proxy_settings(QSettings * s);
  void load_scheduler_settings(QSettings * s);
  void load_gdrive_settings(QSettings * s);
};

#endif // APP_SETTINGS_H
