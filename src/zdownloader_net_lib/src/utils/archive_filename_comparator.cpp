#include "archive_filename_comparator.h"
#include <QFileInfo>

bool archive_filename_comparator::compare(const QString & prev_filename, const QString & curr_filename) const
{
  const QFileInfo prev_fi(prev_filename);
  const QFileInfo curr_fi(curr_filename);
  const QString prev_ext = prev_fi.suffix();
  const QString curr_ext = curr_fi.suffix();
  const QFileInfo prev_without_ext = prev_fi.completeBaseName();
  const QFileInfo curr_without_ext = curr_fi.completeBaseName();
  const QString prev_subsuffix = prev_without_ext.suffix();
  const QString curr_subsuffix = curr_without_ext.suffix();

  if(prev_ext == "rar" && curr_ext == "rar"
     && prev_subsuffix.startsWith("part") && curr_subsuffix.startsWith("part")
     && prev_without_ext.completeBaseName() == curr_without_ext.completeBaseName())
  {
    return true;
  }

  if(prev_subsuffix == "7z" && curr_subsuffix == "7z"
     && prev_without_ext.completeBaseName() == curr_without_ext.completeBaseName())
  {
    return true;
  }

  if(prev_filename == curr_filename)
    return true;

  return false;
}
