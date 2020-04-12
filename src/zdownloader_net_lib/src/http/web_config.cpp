#include "web_config.h"

web_config & web_config::inst()
{
  static web_config instance;
  return instance;
}
