#include "finished_downloads_settings.h"


QString finished_downloads_settings::validate() const
{
  int min = 256;
  int max = 512000;
  if(log_max_rotate_kb < min || log_max_rotate_kb > max)
    return "log_max_rotate_kb must be: from " + QString::number(min) + " to " + QString::number(max);

  min = 1;
  max = 9;
  if(log_max_archive_files < min || log_max_archive_files > max)
    return "log_max_archive_files must be: from " + QString::number(min) + " to " + QString::number(max);

  return "";
}
