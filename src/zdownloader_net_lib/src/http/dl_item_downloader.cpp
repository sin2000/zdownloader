#include "dl_item_downloader.h"
#include "../model/download_item.h"

dl_item_downloader::dl_item_downloader(QNetworkAccessManager * global_nam, QObject * parent)
  :segmented_file_downloader(global_nam, parent)
{
  connect(this, &segmented_file_downloader::download_finished, this, &dl_item_downloader::file_download_finished);
}

void dl_item_downloader::start_download_item(download_item * item, int max_seg, int min_seg_size_bytes,
                                             int seg_network_read_buffer_bytes, int seg_write_buffer_size_bytes)
{
  current_item = item;
  set_max_download_segments(max_seg);
  set_min_segment_size(min_seg_size_bytes);
  set_segment_network_read_buffer_size(seg_network_read_buffer_bytes);
  set_segment_write_buffer_size(seg_write_buffer_size_bytes);
  start_download(item);
}

void dl_item_downloader::file_download_finished(const QString & error_text, const QString & /*url*/)
{
  QString err_txt;
  if(current_item->get_status() != download_item::download_status_finished && current_item->get_status() != download_item::download_status_pending)
    err_txt = "";
  else
    err_txt = error_text;

  emit item_download_finished(this, err_txt, current_item);
}
