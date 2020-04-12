#ifndef GENERAL_SETTINGS_H
#define GENERAL_SETTINGS_H

#include <QString>

class general_settings
{
public:
  QString validate() const;
  void correct_dirs();

  QString download_directory;
  QString user_agent_for_https_connections;
  bool shutdown_system_on_finish = false;
  int max_seconds_before_reset_inactive_connection = 60;
  QString download_lists_directory;
  QString links_file_directory;
};

#endif // GENERAL_SETTINGS_H
