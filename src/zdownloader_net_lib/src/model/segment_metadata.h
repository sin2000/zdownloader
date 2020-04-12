#ifndef SEGMENT_METADATA_H
#define SEGMENT_METADATA_H

#include <QtGlobal>

class segment_metadata
{
public:
  segment_metadata();
  segment_metadata(qint64 seg_start_pos, qint64 seg_end_pos);

  qint64 start_pos;
  qint64 end_pos;
  bool segment_finished;
};

#endif // SEGMENT_METADATA_H
