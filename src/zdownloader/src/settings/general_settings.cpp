#include "general_settings.h"

QString general_settings::validate() const
{
  const int min = 10;
  const int max = 12 * 60 * 60;

  if(max_seconds_before_reset_inactive_connection < min
     || max_seconds_before_reset_inactive_connection > max)
  {
    return "max_seconds_before_reset_inactive_connection must be: from " + QString::number(min) + " to " + QString::number(max);
  }

  return "";
}

void general_settings::correct_dirs()
{
  if(download_directory.isEmpty())
    download_directory = ".";
  if(download_lists_directory.isEmpty())
    download_lists_directory = ".";
  if(links_file_directory.isEmpty())
    links_file_directory = ".";
}
