#ifndef LINKS_CHECKER_SETTINGS_H
#define LINKS_CHECKER_SETTINGS_H

#include <QString>

class links_checker_settings
{
public:
  QString validate() const;

  int max_parallel_link_checks = 5;
  int max_seconds_before_check_link_again_on_error = 10;
};

#endif // LINKS_CHECKER_SETTINGS_H
