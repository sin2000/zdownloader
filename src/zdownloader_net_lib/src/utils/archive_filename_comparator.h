#ifndef ARCHIVE_FILENAME_COMPARATOR_H
#define ARCHIVE_FILENAME_COMPARATOR_H

#include <QString>

class archive_filename_comparator
{
public:
  bool compare(const QString & prev_filename, const QString & curr_filename) const;
};

#endif // ARCHIVE_FILENAME_COMPARATOR_H
