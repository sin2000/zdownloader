#ifndef UNPACKLOG_SETTINGS_H
#define UNPACKLOG_SETTINGS_H

#include <QString>

class unpacklog_settings
{
public:
  QString validate() const;
  void correct_dirs();

  bool log_to_file_enabled = true;
  QString log_directory;
  QString timestamp_format;
  int log_max_rotate_kb = 512;
  int log_max_archive_files = 2;
};

#endif // UNPACKLOG_SETTINGS_H
