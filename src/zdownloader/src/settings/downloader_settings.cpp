#include "downloader_settings.h"

QString downloader_settings::validate() const
{
  int min = 1;
  int max = 20;
  if(max_parallel_file_downloads < min || max_parallel_file_downloads > max)
  {
    return "max_parallel_file_downloads must be: from " + QString::number(min) + " to " + QString::number(max);
  }

  min = 1;
  max = 20;
  if(max_segments_per_file < min || max_segments_per_file > max)
  {
    return "max_segments_per_file must be: from " + QString::number(min) + " to " + QString::number(max);
  }

  min = 1;
  max = 100;
  if(min_segment_size_mb < min || min_segment_size_mb > max)
  {
    return "min_segment_size_mb must be: from " + QString::number(min) + " to " + QString::number(max);
  }

  min = 128;
  max = 51200;
  if(segment_network_read_buffer_kb < min || segment_network_read_buffer_kb > max)
  {
    return "segment_network_read_buffer_kb must be: from " + QString::number(min) + " to " + QString::number(max);
  }

  min = 128;
  max = 20480;
  if(segment_file_write_buffer_kb < min || segment_file_write_buffer_kb > max)
  {
    return "segment_file_write_buffer_kb must be: from " + QString::number(min) + " to " + QString::number(max);
  }

  min = 0;
  max = 3600;
  if(min_seconds_before_next_download < min || min_seconds_before_next_download > max)
  {
    return "min_seconds_before_next_download must be: from " + QString::number(min) + " to " + QString::number(max);
  }

  min = 1;
  max = 3600;
  if(max_seconds_before_next_download < min || max_seconds_before_next_download > max)
  {
    return "max_seconds_before_next_download must be: from " + QString::number(min) + " to " + QString::number(max);
  }

  if(max_seconds_before_next_download <= min_seconds_before_next_download)
    return "max_seconds_before_next_download must be greater than min_seconds_before_next_download";

  min = 10;
  max = 1800000;
  if(next_download_increase_step_msecs < min || next_download_increase_step_msecs > max)
  {
    return "max_seconds_before_next_download must be: from " + QString::number(min) + " to " + QString::number(max);
  }

  min = 1;
  max = 3600;
  if(next_download_increase_reset_after_seconds < min || next_download_increase_reset_after_seconds > max)
  {
    return "next_download_increase_reset_after_seconds must be: from " + QString::number(min) + " to " + QString::number(max);
  }

  min = 1;
  max = 3600;
  if(progress_check_interval_seconds < min || progress_check_interval_seconds > max)
  {
    return "progress_check_interval_seconds must be: from " + QString::number(min) + " to " + QString::number(max);
  }

  min = 1;
  max = 3600;
  if(speed_check_interval_seconds < min || speed_check_interval_seconds > max)
  {
    return "speed_check_interval_seconds must be: from " + QString::number(min) + " to " + QString::number(max);
  }

  min = 1;
  max = 120;
  if(user_reconnect_interval_minutes < min || user_reconnect_interval_minutes > max)
  {
    return "user_reconnect_interval_minutes must be: from " + QString::number(min) + " to " + QString::number(max);
  }

  min = 1;
  max = 9000;
  if(min_avg_speed_before_reconnect_mbps < min || min_avg_speed_before_reconnect_mbps > max)
  {
    return "min_avg_speed_before_reconnect_mbps must be: from " + QString::number(min) + " to " + QString::number(max);
  }

  return "";
}
