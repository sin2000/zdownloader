#ifndef SCHEDULER_SETTINGS_H
#define SCHEDULER_SETTINGS_H

#include <QDateTime>

class scheduler_settings
{
public:
  QString validate() const;

  bool scheduler_enabled = true;
  QDateTime start_download_at;
  QDateTime stop_download_at;
};

#endif // SCHEDULER_SETTINGS_H
