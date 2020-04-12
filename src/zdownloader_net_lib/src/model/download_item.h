#ifndef DOWNLOAD_ITEM_H
#define DOWNLOAD_ITEM_H

#include <QString>
#include <QVector>

class download_item
{
public:
  enum download_status {
    download_status_pending,
    download_status_finished,

    download_status_finished_already_exists,
    download_status_remote_file_does_not_exists
  };

  download_item();
  const QString & get_link() const;
  void set_link(const QString & lnk);
  const QString & get_direct_download_link() const;
  void set_direct_download_link(const QString & lnk);
  const QString & get_filename() const;
  void set_filename(const QString & fn);

  download_status get_status() const;
  void set_status(download_status stat);
  bool set_status_from_char(const QChar & ch);
  QChar get_status_as_char() const;
  QString to_string() const;
  QString to_string_finished() const;

  qint64 get_file_size_bytes() const;
  void set_file_size_bytes(qint64 size);

  // segmented download - contains positions in file where each segment ends
  qint64 get_segment_end_pos(int segment_idx) const;
  void set_segment_end_pos(int segment_idx, qint64 segment_end_pos);
  void set_segment_ends_size(int size);
  int get_segment_ends_size() const;
  void clear_segment_ends();
  bool is_segment_ends_empty() const;
  const QVector<qint64> & get_segment_ends() const;
  void set_segment_ends(const QVector<qint64> & seg_ends);

  int get_group_id() const;
  void set_group_id(int id);

  const QString & get_initial_link() const;

  const QByteArray & get_access_token() const;
  void set_access_token(const QByteArray & token);

private:
  QString link;
  QString download_link;
  QString filename;
  download_status status;
  qint64 file_size_bytes;

  QVector<qint64> segment_ends;

  int group_id;
  QString initial_link;
  QByteArray access_token;
};

#endif // DOWNLOAD_ITEM_H
