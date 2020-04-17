#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include "zd_shared_lib_global.h"
#include <QSet>

class ZD_SHARED_LIB_EXPORT string_utils
{
public:
  string_utils();
  //example QSet<QChar>() << '0' << '1' << '2';
  static int find_first_of(const QSet<QChar> & chars, const QString & text, int from = 0);
  static QString pull_string(const QString & src, const QString & start, const QString & end = "");
  static QString seconds_to_string(qint64 seconds);
  static QString bytes_to_human_readable_string(qint64 bytes);
};

#endif // STRING_UTILS_H
