#ifndef OUTPUT_MESSAGES_SETTINGS_H
#define OUTPUT_MESSAGES_SETTINGS_H

#include <QString>

class output_messages_settings
{
public:
  QString validate() const;
  void correct_dirs();

  bool log_to_file_enabled = true;
  QString log_directory;
  QString timestamp_format;
  int log_max_rotate_kb = 4096;
  int log_max_archive_files = 3;
};

#endif // OUTPUT_MESSAGES_SETTINGS_H
