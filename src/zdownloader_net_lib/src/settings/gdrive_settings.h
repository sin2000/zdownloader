#ifndef GDRIVE_SETTINGS_H
#define GDRIVE_SETTINGS_H

#include "../zdownloader_net_lib_global.h"
#include <QString>

class ZD_NET_LIBSHARED_EXPORT gdrive_settings
{
public:
  QString validate() const;

  bool use_gdrive_api = false;
  QString google_service_account_json_file;
  QString client_email;
  QString private_key;
  QString json_file_error;
};

#endif // GDRIVE_SETTINGS_H
