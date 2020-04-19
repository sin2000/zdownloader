#include "segment_metadata.h"

segment_metadata::segment_metadata()
  :start_pos(0),
   end_pos(0),
   segment_finished(false)
{
}

segment_metadata::segment_metadata(qint64 seg_start_pos, qint64 seg_end_pos)
  :start_pos(seg_start_pos),
   end_pos(seg_end_pos),
   segment_finished(false)
{
}
