#ifndef UNPACK_SETTINGS_H
#define UNPACK_SETTINGS_H

#include <QStringList>

class unpack_settings
{
public:
  QString validate() const;

  bool unpack_after_download = true;
  bool pause_download_before_unpack = false;
  bool delete_archive_files_after_unpack = true;
  QString unrar_binary;
  QString seven_zip_binary;
  QStringList passwords;
  bool unpack_passwords_size_error = false;
};

#endif // UNPACK_SETTINGS_H
