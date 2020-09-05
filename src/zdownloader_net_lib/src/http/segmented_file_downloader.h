#ifndef SEGMENTED_FILE_DOWNLOADER_H
#define SEGMENTED_FILE_DOWNLOADER_H

#include "../model/segment_metadata.h"
#include <QObject>
#include <QNetworkReply>

class QNetworkAccessManager;
class download_item;
class file_downloader2;
class QFile;

class segmented_file_downloader : public QObject
{
  Q_OBJECT
public:
  segmented_file_downloader(QNetworkAccessManager * global_nam = nullptr, QObject * parent = nullptr);
  ~segmented_file_downloader();

  void start_download(download_item * item);
  void set_download_dir(const QString & dir_path);
  void set_max_download_segments(int segments);
  void set_min_segment_size(int bytes);
  void set_segment_network_read_buffer_size(int bytes);
  void set_segment_write_buffer_size(int bytes);
  double get_current_download_speed_mbps() const;
  QString get_current_progress_msg() const;
  qint64 get_ramaining_bytes() const;
  void update_remaining_bytes();
  void save_and_abort_download(bool emit_finished_signal = false);
  download_item * get_curr_dl_item() const;

signals:
  void download_finished(const QString & error_text, const QString & url);

private:
  enum setup_file_error {
    file_error_already_exists,
    file_error_could_not_open_readwrite,
    file_error_could_not_resize,
    file_error_no_error
  };

  setup_file_error setup_file(QString * error);
  void unsetup_file();
  void remove_file();
  void compute_seg_metadata_list();
  void prepare_downloaders();
  void start_next_segment_download();
  void download_segment_started(file_downloader2 * sender);
  void download_segment_success(file_downloader2 * sender);
  void download_segment_error(file_downloader2 * sender, const QString & error_text, QNetworkReply::NetworkError error_code);
  int get_first_pending_downloader_idx() const;

  download_item * curr_dl_item;
  QNetworkAccessManager * nam;
  QString download_dir;
  int max_segments;
  int active_segments;
  int finished_segments;
  int min_segment_size_bytes;
  QList<file_downloader2 *> downloaders;
  QList<segment_metadata> seg_metadata_list;
  QFile * shared_file;
  bool all_seg_error_state;
  int segment_read_buffer_size_bytes;
  int segment_write_buffer_size_bytes;
};

#endif // SEGMENTED_FILE_DOWNLOADER_H
