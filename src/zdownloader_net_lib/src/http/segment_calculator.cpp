#include "segment_calculator.h"

void segment_calculator::compute(qint64 file_size_bytes, int min_segment_size_bytes, int max_segments)
{
  int segment_count = max_segments ? max_segments : 1;
  qint64 segment_size_bytes = file_size_bytes / segment_count;
  if(segment_size_bytes < min_segment_size_bytes)
  {    
    segment_count = static_cast<int>(file_size_bytes / min_segment_size_bytes);
    if(segment_count <= 1)
    {
      segment_count = 1;
      segment_size_bytes = file_size_bytes;
    }
    else
    {
      segment_size_bytes = min_segment_size_bytes;
    }
  }

  seg_metadata_list.clear();
  seg_metadata_list.reserve(segment_count);
  // first segment
  qint64 pos = segment_size_bytes - 1;
  seg_metadata_list.append(segment_metadata(0, pos));
  // middle segments
  const int almost_segments = segment_count - 1;
  for(int i = 1; i < almost_segments; ++i)
  {
    segment_metadata meta;
    meta.start_pos = pos + 1;
    pos += segment_size_bytes;
    meta.end_pos = pos;
    seg_metadata_list.append(meta);
  }
  // last segment
  if(segment_count > 1)
  {
    seg_metadata_list.append(segment_metadata(++pos, file_size_bytes - 1));
  }
}

const QList<segment_metadata> & segment_calculator::get_segment_metadata_list()
{
  return seg_metadata_list;
}
