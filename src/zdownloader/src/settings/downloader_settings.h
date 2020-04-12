#ifndef DOWNLOADER_SETTINGS_H
#define DOWNLOADER_SETTINGS_H

#include <QString>

class downloader_settings
{
public:
  QString validate() const;

  int max_parallel_file_downloads = 1;
  int max_segments_per_file = 1;
  int min_segment_size_mb = 4;
  int segment_network_read_buffer_kb = 1024;
  int segment_file_write_buffer_kb = 1024;
  int min_seconds_before_next_download = 0;
  int max_seconds_before_next_download = 7;
  int next_download_increase_step_msecs = 250;
  int next_download_increase_reset_after_seconds = 8;
  int progress_check_interval_seconds = 15;
  int speed_check_interval_seconds = 8;
  int user_reconnect_interval_minutes = 4;
  int min_avg_speed_before_reconnect_mbps = 13;
};

#endif // DOWNLOADER_SETTINGS_H
