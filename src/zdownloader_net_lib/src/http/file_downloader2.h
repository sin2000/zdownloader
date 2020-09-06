#ifndef FILE_DOWNLOADER2_H
#define FILE_DOWNLOADER2_H

#include "abstract_get_request.h"

class QElapsedTimer;
class QFile;

class file_downloader2 : public abstract_get_request
{
  Q_OBJECT
public:
  file_downloader2(QNetworkAccessManager * global_nam = nullptr, QObject * parent = nullptr);
  ~file_downloader2();

  void set_download_start_pos(qint64 bytes);
  void set_download_end_pos(qint64 bytes);
  void set_downloaded_bytes_already(qint64 bytes);
  void set_file_name(const QString & name);
  void set_shared_file(QFile * file);
  void set_read_buffer_size(int bytes);
  void set_write_buffer_size(int bytes);

  double get_current_download_speed_bps() const; // bytes per second
  double get_current_download_speed_mbps() const;
  qint64 get_bytes_received_and_on_disk() const;
  qint64 get_last_file_write_cursor_pos() const;
  void save_and_abort_download();

signals:
  void download_started(file_downloader2 * sender);
  void download_success(file_downloader2 * sender);
  void download_error(file_downloader2 * sender, const QString & error_text, QNetworkReply::NetworkError error_code);

private:
  void download() final;
  void download_progress(qint64 bytes_received, qint64 bytes_total);
  void metadata_changed();
  void operation_finished() final;
  void reply_read_chunk_and_save();
  void reply_read_all_and_save(bool read_all);

  QFile * shared_file;
  QString file_name;
  qint64 bytes_received_last;
  qint64 bytes_received_and_on_disk;
  qint64 download_start_pos_bytes;
  qint64 download_end_pos_bytes;
  qint64 file_write_cursor;

  QElapsedTimer * download_time;
  QTimer * reply_read_chunk_and_save_timer;
  int write_buffer_size;
  QByteArray read_buffer;
  bool wrong_content_type;
  bool has_download_started;
  int read_buffer_size;
};

#endif // FILE_DOWNLOADER2_H
