#include "unpack.h"
#include <meta_object_ext.h>
#include <string_utils.h>
#include <zd_logger.h>
#include <QDateTime>
#include <QFile>
#include <QThread>
#include <QFileInfo>
#include <QDirIterator>
#include <QTimer>
#include <QRegularExpression>
#include <qdebugex.h>

unpack::unpack(QObject * parent)
  :QObject(parent),
   unpack_process(new QProcess(this)),
   current_unpack_app(rar_unpacker),
   unpack_pwd_idx(0),
   is_shutdown(false),
   is_running(false),
   fatal_error_occured(false),
   unpack_logger(nullptr)
{
  connect(unpack_process, &QProcess::errorOccurred, this, &unpack::process_error);
  connect(unpack_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &unpack::process_finished);
  connect(unpack_process, &QProcess::readyReadStandardOutput, this, &unpack::read_std_output);
}

unpack::~unpack()
{
  if(unpack_logger)
  {
    unpack_logger->invoke_disable(false);
    zd_logger::inst().text_log_mgr->remove_object(unpacklog_base_filename);
  }
}

void unpack::set_settings(const unpack_settings & settings)
{
  unpack_prefs = settings;
}

void unpack::set_log_settings(const unpacklog_settings & log_settings)
{
  log_prefs = log_settings;
  if(log_prefs.log_to_file_enabled && unpack_logger == nullptr)
  {
    unpack_logger = zd_logger::inst().text_log_mgr->add_object(unpacklog_base_filename);
    unpack_logger->invoke_set_timestamp_format(log_prefs.timestamp_format);
    unpack_logger->invoke_set_rotate_settings(log_prefs.log_max_archive_files, log_prefs.log_max_rotate_kb * 1024);
    unpack_logger->invoke_enable(log_prefs.log_directory, unpacklog_base_filename, false);
  }
}

void unpack::set_working_directory(const QString & dir)
{
  working_dir = dir;
}

void unpack::add_to_queue(const QString & filename)
{
  unpack_queue.enqueue(filename);
  save_queue();
}

void unpack::load_queue()
{
  if(unpack_prefs.unpack_after_download == false)
    return;

  QFile queue_file(unpack_queue_filename);
  if(queue_file.exists() == false)
    return;

  if(queue_file.open(QIODevice::ReadOnly | QIODevice::Text) == false)
  {
    qDebug() << "Could not open file for read:" << unpack_queue_filename;
    return;
  }

  const QString content = QString::fromUtf8(queue_file.readAll());
  queue_file.close();

  const QStringList filenames = content.split(QChar::LineFeed, QString::SkipEmptyParts);
  for(const auto & fn : filenames)
    unpack_queue.enqueue(fn);
}

void unpack::save_queue()
{
  if(unpack_queue.isEmpty())
  {
    if(QFile::exists(unpack_queue_filename))
    {
      if(QFile::remove(unpack_queue_filename) == false)
        qDebug() << "Could not remove file:" << unpack_queue_filename;
    }

    return;
  }

  QFile queue_file(unpack_queue_filename);
  if(queue_file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text) == false)
  {
    qDebug() << "Could not open file for write:" << unpack_queue_filename;
    return;
  }

  for(const auto & item : unpack_queue)
  {
    const QString line = item + "\n";
    queue_file.write(line.toUtf8());
  }
}

void unpack::start_unpack_first_item()
{  
  if(unpack_queue.isEmpty())
  {
    qDebug() << "UNPACK: WARNING - start_unpack_first_item - unpack_queue is empty";
    return;
  }

  if(is_running)
  {
    //qDebug() << "UNPACK: WARNING - start_unpack_first_item - running already";
    return;
  }

  unpack_pwd_idx = 0;
  if(setup_unpack_process(unpack_queue.head()))
  {
    start_unpack_process();
  }
  else
  {
    const QString msg = "UNPACK: setup_unpack_process failed - type of archive not found in file: " + unpack_queue.head();
    qDebug() << msg;
    if(unpack_logger)
      unpack_logger->invoke_log_text(QDateTime::currentMSecsSinceEpoch(), msg);
    meta_object_ext::invoke_async(this, &unpack::unpack_next_item);
  }
}

void unpack::stop_unpack()
{
  if(is_running)
  {
    is_shutdown = true;
    unpack_process->terminate();
    if(unpack_process->waitForFinished(10000) == false)
    {
      unpack_process->kill();
      unpack_process->waitForFinished(10000);
    }
  }
}

const unpack_settings & unpack::settings() const
{
  return unpack_prefs;
}

bool unpack::is_queue_empty() const
{
  return unpack_queue.isEmpty();
}

bool unpack::has_fatal_error_occured() const
{
  return fatal_error_occured;
}

bool unpack::setup_unpack_process(const QString & filename)
{
  curr_archive_filename = filename;
  QString archive_filepath;
  QString archive_dir;
  QString archive_pwd = unpack_prefs.passwords.value(0, "-");
  QFileInfo file_info(filename);
  const QString file_ext = file_info.suffix();
  const QString prev_suffix = QFileInfo(file_info.completeBaseName()).suffix();
  if(file_ext == "rar")
  {
    if(prev_suffix.startsWith("part"))
    {      
      archive_dir = QFileInfo(file_info.completeBaseName()).completeBaseName();
      const QString part = ".part*.rar";
      archive_filepath =  archive_dir + part;
      remove_filename_filter = escape_name_filter(archive_dir) + part;
    }
    else
    {
      archive_dir = file_info.completeBaseName();
      archive_filepath = filename;
      remove_filename_filter = escape_name_filter(archive_filepath);
    }

    archive_filepath = working_dir + "/" + archive_filepath;
    archive_dir = working_dir + "/" + archive_dir;
    set_unrar_program(QStringList{"x", "-p" + archive_pwd, "-ai", "-y", "-c-", "-o+", archive_filepath, archive_dir + "/"});
    return true;
  }

  if(file_ext == "7z" || prev_suffix == "7z" || file_ext == "zip")
  {
    if(prev_suffix == "7z")
    {
      archive_dir = QFileInfo(file_info.completeBaseName()).completeBaseName();
      const QString part = ".7z.*";
      archive_filepath = archive_dir + part;
      remove_filename_filter = escape_name_filter(archive_dir) + part;
    }
    else
    {
      archive_dir = file_info.completeBaseName();
      archive_filepath = filename;
      remove_filename_filter = escape_name_filter(archive_filepath);
    }

    archive_filepath = working_dir + "/" + archive_filepath;
    archive_dir = working_dir + "/" + archive_dir;
    set_7z_program(QStringList{"x", "-p" + archive_pwd, "-aoa", "-y", "-o"+ archive_dir + "/", archive_filepath});
    return true;
  }

  return false;
}

void unpack::set_unrar_program(const QStringList & prog_args)
{
  if(unpack_prefs.unrar_binary.isEmpty())
  {
#ifdef SYSTEM_UNIX
    unpack_process->setProgram("unrar");
#else
    unpack_process->setProgram("C:\\Program Files\\WinRAR\\UnRar.exe");
#endif
  }
  else
  {
    unpack_process->setProgram(unpack_prefs.unrar_binary);
  }

  unpack_process->setArguments(prog_args);
  current_unpack_app = rar_unpacker;
}

void unpack::set_7z_program(const QStringList & prog_args)
{
  if(unpack_prefs.seven_zip_binary.isEmpty())
  {
#ifdef SYSTEM_UNIX
    unpack_process->setProgram("7z");
#else
    unpack_process->setProgram("C:\\Program Files\\7-zip\\7z.exe");
#endif
  }
  else
  {
    unpack_process->setProgram(unpack_prefs.seven_zip_binary);
  }

  unpack_process->setArguments(prog_args);
  current_unpack_app = seven_zip_unpacker;
}

void unpack::start_unpack_process()
{
  qDebug() << "UNPACK started: " + curr_archive_filename;
  if(unpack_logger)
    unpack_logger->invoke_log_text(QDateTime::currentMSecsSinceEpoch(),
                                   "================================================================================\n"
                                   "UNPACK started: " + unpack_process->program() + " " + unpack_process->arguments().join(' '));
  is_running = true;
  unpack_process->start(QIODevice::ReadOnly | QIODevice::Text);
}

void unpack::read_std_output()
{
  const QString content = last_text + QString::fromUtf8(unpack_process->readAllStandardOutput());
  last_text.clear();
  QStringList lines = content.split('\n', QString::SkipEmptyParts);
  if(content.endsWith('\n') == false && lines.isEmpty() == false
     && content.endsWith('%') == false)
  {
    last_text = lines.last();
    lines.removeLast();
  }

  for(auto & line : lines)
  {
    line.remove(QChar('\b'));
    line = line.simplified();
    bool show_line = true;
    if(line.size() <= 3 && line.endsWith('%'))
    {
      const int percent = line.chopped(1).toInt();
      if((percent != 0 && (percent % 5) == 0) == false)
        show_line = false;
    }
    if(line.contains("error", Qt::CaseInsensitive))
      show_line = true;

    if(line.isEmpty() == false && show_line && line != "OK")
    {
      if(unpack_logger)
        unpack_logger->invoke_log_text(QDateTime::currentMSecsSinceEpoch(), line);
    }
  }
}

void unpack::process_error(QProcess::ProcessError error)
{
  qDebug() << "UNPACK error:" << error << "Unpack program:" << unpack_process->program();
  if(unpack_logger)
    unpack_logger->invoke_log_text(QDateTime::currentMSecsSinceEpoch(),
                                   "UNPACK error: " + QString::number(error) + " Unpack program: " + unpack_process->program());
  is_running = false;
  last_text.clear();
  fatal_error_occured = true;
  emit unpack_queue_finished();
}

void unpack::process_finished(int exit_code, QProcess::ExitStatus exit_status)
{
  const QString exit_code_str = exit_code_to_string(exit_code);
  qDebug() << "UNPACK finished. Exit code:" << exit_code_str << "Exit status:" << exit_status;
  if(unpack_logger)
    unpack_logger->invoke_log_text(QDateTime::currentMSecsSinceEpoch(), "UNPACK finished. Exit code: " + exit_code_str);
  is_running = false;
  last_text.clear();
  if(is_shutdown || exit_status == QProcess::CrashExit)
    return;

  if(current_unpack_app == rar_unpacker)
    rar_unpack_finished(exit_code);
  else
    seven_zip_unpack_finished(exit_code);
}

void unpack::rar_unpack_finished(int exit_code)
{
  switch(exit_code)
  {
    case rar_success_exit:
      remove_unpacked_archive_files();
      meta_object_ext::invoke_async(this, &unpack::unpack_next_item);
      break;

    case rar_badpwd_exit:
      if(set_next_unpack_pwd())
        meta_object_ext::invoke_async(this, &unpack::start_unpack_process);
      else
        meta_object_ext::invoke_async(this, &unpack::unpack_next_item);
      break;

    default:
      meta_object_ext::invoke_async(this, &unpack::unpack_next_item);
      break;
  }
}

void unpack::seven_zip_unpack_finished(int exit_code)
{
  switch(exit_code)
  {
    case seven_zip_normal_exit:
      remove_unpacked_archive_files();
      meta_object_ext::invoke_async(this, &unpack::unpack_next_item);
      break;

    case seven_zip_fatal_error_exit:
      if(set_next_unpack_pwd())
        meta_object_ext::invoke_async(this, &unpack::start_unpack_process);
      else
        meta_object_ext::invoke_async(this, &unpack::unpack_next_item);
      break;

    default:
      //meta_object_ext::invoke_async(this, &unpack::unpack_next_item);
      QTimer::singleShot(0, this, &unpack::unpack_next_item);
      break;
  }
}

bool unpack::set_next_unpack_pwd()
{
  ++unpack_pwd_idx;
  QStringList args = unpack_process->arguments();
  if(unpack_pwd_idx < unpack_prefs.passwords.size() && args.size() > 1)
  {
    const QString pwd = unpack_prefs.passwords.at(unpack_pwd_idx);
    args[1] = "-p" + pwd;
    unpack_process->setArguments(args);
    return true;
  }

  return false;
}

void unpack::unpack_next_item()
{
  if(unpack_queue.isEmpty() == false)
  {
    unpack_queue.removeFirst();
    save_queue();

    if(unpack_queue.isEmpty())
      emit unpack_queue_finished();
    else
      start_unpack_first_item();
  }
}

void unpack::remove_unpacked_archive_files()
{
  if(unpack_queue.isEmpty() || unpack_prefs.delete_archive_files_after_unpack == false)
    return;

  QDirIterator it(working_dir, QStringList(remove_filename_filter), QDir::Files);
  while (it.hasNext())
  {
    const QString filepath = it.next();
    QString log_msg;
    if(QFile::remove(filepath))
      log_msg = "Delete file " + filepath;
    else
      log_msg = "Could not delete file: " + filepath;

    if(unpack_logger)
      unpack_logger->invoke_log_text(QDateTime::currentMSecsSinceEpoch(), log_msg);
  }
}

QString unpack::escape_name_filter(const QString & name) const
{
  QString tmp = name;
  const QSet<QChar> special_chars { '[', ']', '?', '*'};

  int pos = 0;
  while((pos = string_utils::find_first_of(special_chars, tmp, pos)) != -1)
  {
    const QChar c = tmp.at(pos);
    tmp.replace(pos, 1, "[" + c + "]");
    pos += 3;
  }

  return tmp;
}

QString unpack::exit_code_to_string(int exit_code) const
{
  QString ret = QString::number(exit_code);
  if(current_unpack_app == rar_unpacker)
  {
    switch(exit_code)
    {
      case rar_success_exit:
        return ret + "(Success)";

      case rar_badpwd_exit:
        return ret + "(Bad password)";

      default:
        return ret;
    }
  }
  else
  {
    switch(exit_code)
    {
      case seven_zip_normal_exit:
        return ret + "(Success)";

      case seven_zip_fatal_error_exit:
        return ret + "(Bad password or fatal error)";

      default:
        return ret;
    }
  }
}
