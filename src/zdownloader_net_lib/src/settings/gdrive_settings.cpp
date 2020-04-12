#include "gdrive_settings.h"

QString gdrive_settings::validate() const
{
  if(use_gdrive_api)
  {
    if(google_service_account_json_file.isEmpty())
      return "google_service_account_json_file is empty";

    if(json_file_error.isEmpty() == false)
      return json_file_error;

    if(client_email.isEmpty())
      return "client_email from json file is empty";

    if(private_key.isEmpty())
      return "private_key from json file is empty";
  }

  return "";
}
