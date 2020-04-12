#include "links_checker_settings.h"

QString links_checker_settings::validate() const
{
  int min = 1;
  int max = 20;
  if(max_parallel_link_checks < min
     || max_parallel_link_checks > max)
  {
    return "max_parallel_link_checks must be: from " + QString::number(min) + " to " + QString::number(max);
  }

  min = 1;
  max = 3600;
  if(max_seconds_before_check_link_again_on_error < min
     || max_seconds_before_check_link_again_on_error > max)
  {
    return "max_seconds_before_check_link_again_on_error must be: from " + QString::number(min) + " to " + QString::number(max);
  }

  return "";
}
