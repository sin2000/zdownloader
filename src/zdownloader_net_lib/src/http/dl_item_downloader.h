#ifndef DL_ITEM_DOWNLOADER_H
#define DL_ITEM_DOWNLOADER_H

#include "segmented_file_downloader.h"

class download_item;

class dl_item_downloader : public segmented_file_downloader
{
  Q_OBJECT
public:
  dl_item_downloader(QNetworkAccessManager * global_nam = nullptr, QObject * parent = nullptr);
  void start_download_item(download_item * item, int max_seg, int min_seg_size_bytes,
                           int seg_network_read_buffer_bytes, int seg_write_buffer_size_bytes);

signals:
  void item_download_finished(dl_item_downloader * sender, const QString & error_text, download_item * item);

private:
  void file_download_finished(const QString & error_text, const QString & url);

  download_item * current_item;
};

#endif // DL_ITEM_DOWNLOADER_H
