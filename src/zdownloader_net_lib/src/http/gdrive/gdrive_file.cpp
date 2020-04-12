#include "gdrive_file.h"

gdrive_file::gdrive_file(const QString & file_name, const QString & file_id, qint64 size_bytes)
  :name(file_name),
   id(file_id),
   size(size_bytes)
{
}
