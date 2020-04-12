#ifndef FINISHED_DOWNLOADS_SETTINGS_H
#define FINISHED_DOWNLOADS_SETTINGS_H

#include "../zdownloader_net_lib_global.h"
#include <QString>

class ZD_NET_LIBSHARED_EXPORT finished_downloads_settings
{
public:
  QString validate() const;

  bool log_to_file_enabled = true;
  QString timestamp_format;
  int log_max_rotate_kb = 4096;
  int log_max_archive_files = 3;
};

#endif // FINISHED_DOWNLOADS_SETTINGS_H
