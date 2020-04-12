#include "unpack_settings.h"

QString unpack_settings::validate() const
{
  if(unpack_passwords_size_error)
    return "UnpackPasswords not readed. Size must be from 0 to 20";

  return "";
}
