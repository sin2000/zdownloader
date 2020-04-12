#include "file_logger.h"
#include <meta_object_ext.h>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QLockFile>
#include <QDebug>

const QString file_logger::archive_placeholder = "%archive_counter%";

file_logger::file_logger(const QString & /*name*/, bool use_lock_file, bool use_qdebug)
  :cur_log_file(nullptr),
   lock_file(nullptr),
   enabled_lock_file(use_lock_file),
   enabled_qdebug(use_qdebug),
   archive_counter(1),
   max_archive_files(3),
   max_rotate_bytes(3 * 1024 * 1024) // 3MB
{
}

file_logger::~file_logger()
{
  if(enabled_qdebug)
    qDebug() << "destructor file_logger";

  close_log();
}

void file_logger::set_directory_path(const QString & directory_path)
{
  dir_path = directory_path;
}

QString file_logger::get_filename_from_pattern(int arch_counter, bool alt_name)
{
  if(fn_pattern.isEmpty())
    fn_pattern = "applog_" + archive_placeholder + ".log";

  QString placeholder_value;
  if(alt_name)
  {
    const QFileInfo fi = QFileInfo(fn_pattern);
    fn_pattern = fi.baseName() + "_" + QString::number(QDateTime::currentMSecsSinceEpoch()) + "." + fi.suffix();
  }

  QString p = fn_pattern;
  p.replace(archive_placeholder, QString::number(arch_counter));

  return p;
}

void file_logger::set_filename_pattern(const QString & filename_pattern)
{
  fn_pattern = filename_pattern;
}

void file_logger::set_max_rotate_bytes(int bytes)
{
  max_rotate_bytes = bytes;
}

void file_logger::set_max_archive_files(int max_files)
{
  max_archive_files = max_files;
}

bool file_logger::open_log()
{
  if(!cur_log_file && dir_path.isEmpty() == false)
  {
    QString full_file_path = QDir(dir_path).absoluteFilePath(get_filename_from_pattern(1));

    if(enabled_lock_file)
    {
      lock_file = new QLockFile(full_file_path + ".lock");
      lock_file->setStaleLockTime(0);
      if(lock_file->tryLock() == false)
        full_file_path = QDir(dir_path).absoluteFilePath(get_filename_from_pattern(1, true));
    }

    cur_log_file = new QFile(full_file_path);
    if(cur_log_file->open(QIODevice::Append | QIODevice::Text | QIODevice::ReadWrite))
    {
      return true;
    }
  }

  return false;
}

void file_logger::close_log()
{
  if(cur_log_file)
  {
    delete lock_file;
    lock_file = nullptr;
    delete cur_log_file;
    cur_log_file = nullptr;
  }
}

bool file_logger::rotate_log()
{
  if(cur_log_file)
  {
    if(cur_log_file->size() >= max_rotate_bytes)
    {
      close_log();

      QDir dir(dir_path);
      QString last_archive_file_path = dir.absoluteFilePath(get_filename_from_pattern(max_archive_files));
      QFile::remove(last_archive_file_path);
      for(int i = max_archive_files - 1; i >= 1; --i)
      {
        QString curr_file_path = dir.absoluteFilePath(get_filename_from_pattern(i));
        QString renamed_file_path = dir.absoluteFilePath(get_filename_from_pattern(i + 1));
        QFile::rename(curr_file_path, renamed_file_path);
      }

      open_log();

      return true;
    }
  }

  return false;
}

void file_logger::save_log(const QByteArray & text, bool flush, bool * log_rotated)
{
  if(cur_log_file)
  {
    cur_log_file->write(text + '\n');
    if(flush)
      cur_log_file->flush();
  }

  const bool rotated = rotate_log();
  if(log_rotated)
    *log_rotated = rotated;
}

void file_logger::save_log(const QString & text, bool flush)
{
  save_log(text.toUtf8(), flush);
}

bool file_logger::is_valid() const
{
  return cur_log_file != nullptr;
}

QByteArray file_logger::read_first_line() const
{
  QByteArray line;
  if(cur_log_file)
  {
    qint64 pos = cur_log_file->pos();

    if(cur_log_file->seek(0))
    {
      line = cur_log_file->readLine();
      if(line.endsWith("\n"))
        line.remove(line.size() - 1, 1);

      cur_log_file->seek(pos);
    }
  }

  return line;
}

qint64 file_logger::get_current_log_size() const
{
  if(cur_log_file)
    return cur_log_file->size();

  return -1;
}
