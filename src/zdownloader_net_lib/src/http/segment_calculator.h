#ifndef SEGMENT_CALCULATOR_H
#define SEGMENT_CALCULATOR_H

#include "../model/segment_metadata.h"
#include <QList>

class segment_calculator
{
public:
  void compute(qint64 file_size_bytes, int min_segment_size_bytes, int max_segments);
  const QList<segment_metadata> & get_segment_metadata_list();

private:
  QList<segment_metadata> seg_metadata_list;
};

#endif // SEGMENT_CALCULATOR_H
