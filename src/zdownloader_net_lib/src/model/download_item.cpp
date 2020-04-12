#include "download_item.h"

download_item::download_item()
  :status(download_status_pending),
   file_size_bytes(-1),
   group_id(-1)
{
}

const QString &download_item::get_link() const
{
  return link;
}

void download_item::set_link(const QString & lnk)
{
  link = lnk;
  if(initial_link.isEmpty())
    initial_link = link;
}

const QString & download_item::get_direct_download_link() const
{
  return download_link;
}

void download_item::set_direct_download_link(const QString & lnk)
{
  download_link = lnk;
}

const QString & download_item::get_filename() const
{
  return filename;
}

void download_item::set_filename(const QString & fn)
{
  filename = fn;
}

download_item::download_status download_item::get_status() const
{
  return status;
}

void download_item::set_status(download_item::download_status stat)
{
  status = stat;
}

bool download_item::set_status_from_char(const QChar & ch)
{
  switch(ch.toLatin1())
  {
    case 'F':
      status = download_status_finished;
      break;
    case 'P':
      status = download_status_pending;
      break;
    case 'X':
      status = download_status_finished_already_exists;
      break;
    case 'R':
      status = download_status_remote_file_does_not_exists;
      break;
    default:
      status = download_status_pending;
      return false;
  }

  return true;
}

QChar download_item::get_status_as_char() const
{
  switch(status)
  {
    case download_status_finished:
      return 'F';

    case download_status_pending:
      return 'P';

    case download_status_finished_already_exists:
      return 'X';

    case download_status_remote_file_does_not_exists:
      return 'R';
  }

  return 'P';
}

QString download_item::to_string() const
{
  return filename + QChar::Tabulation + initial_link + QChar::Tabulation + QString::number(group_id);
}

QString download_item::to_string_finished() const
{
  return get_status_as_char() + QChar::Tabulation + filename + QChar::Tabulation + initial_link;
}

qint64 download_item::get_file_size_bytes() const
{
  return file_size_bytes;
}

void download_item::set_file_size_bytes(qint64 size)
{
  file_size_bytes = size;
}

qint64 download_item::get_segment_end_pos(int segment_idx) const
{
  if(segment_idx < segment_ends.size())
    return segment_ends.at(segment_idx);

  return 0;
}

void download_item::set_segment_end_pos(int segment_idx, qint64 segment_end_pos)
{
  if(segment_idx < segment_ends.size())
    segment_ends[segment_idx] = segment_end_pos;
}

void download_item::set_segment_ends_size(int size)
{
  segment_ends.fill(0, size);
}

int download_item::get_segment_ends_size() const
{
  return segment_ends.size();
}

void download_item::clear_segment_ends()
{
  segment_ends.clear();
}

bool download_item::is_segment_ends_empty() const
{
  return segment_ends.isEmpty();
}

const QVector<qint64> & download_item::get_segment_ends() const
{
  return segment_ends;
}

void download_item::set_segment_ends(const QVector<qint64> & seg_ends)
{

  segment_ends = seg_ends;
}

int download_item::get_group_id() const
{
  return group_id;
}

void download_item::set_group_id(int id)
{
  group_id = id;
}

const QString & download_item::get_initial_link() const
{
  return initial_link;
}

const QByteArray &download_item::get_access_token() const
{
  return access_token;
}

void download_item::set_access_token(const QByteArray & token)
{
  access_token = token;
}
