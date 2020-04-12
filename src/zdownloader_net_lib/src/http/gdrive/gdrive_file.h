#ifndef GDRIVE_FILE_H
#define GDRIVE_FILE_H

#include <QString>

class gdrive_file
{
public:
  gdrive_file(const QString & file_name, const QString & file_id, qint64 size_bytes = -1);

  QString name;
  QString id;
  qint64 size;
};

#endif // GDRIVE_FILE_H
