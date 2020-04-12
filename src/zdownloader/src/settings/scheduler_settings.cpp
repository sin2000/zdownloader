#include "scheduler_settings.h"

QString scheduler_settings::validate() const
{
  if(scheduler_enabled == false)
    return "";

  if(start_download_at.isValid() == false)
    return "start_download_at is invalid";

  if(stop_download_at.isValid() == false)
    return "stop_download_at is invalid";

  if(start_download_at == stop_download_at)
    return "start_download_at is same as stop_download_at";

  return "";
}
