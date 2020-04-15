#ifndef UNPACK_H
#define UNPACK_H

#include "settings/unpack_settings.h"
#include "settings/unpacklog_settings.h"
#include <QQueue>
#include <QProcess>
#include <QObject>

class text_file_logger;

class unpack : public QObject
{
  Q_OBJECT
public:
  unpack(QObject * parent = nullptr);
  ~unpack();
  void set_settings(const unpack_settings & settings);
  void set_log_settings(const unpacklog_settings & log_settings);
  void set_working_directory(const QString & dir);
  void add_to_queue(const QString & filename);
  void load_queue();
  void start_unpack_first_item();
  void stop_unpack();
  const unpack_settings & settings() const;
  bool is_queue_empty() const;
  bool has_fatal_error_occured() const;

signals:
  void unpack_queue_finished();

private:
  enum unpack_program {
    rar_unpacker,
    seven_zip_unpacker
  };
  enum rar_exit_code
  {
    rar_success_exit = 0,
    rar_badpwd_exit = 11,
    rar_userbreak_exit = 255
  };
  enum seven_zip_exit_code
  {
    seven_zip_normal_exit = 0,
    seven_zip_fatal_error_exit = 2,
  };

  void save_queue();
  bool setup_unpack_process(const QString & filename);
  void set_unrar_program(const QStringList & prog_args);
  void set_7z_program(const QStringList & prog_args);
  void start_unpack_process();
  void read_std_output();

  void process_error(QProcess::ProcessError error);
  void process_finished(int exit_code, QProcess::ExitStatus exit_status);
  void rar_unpack_finished(int exit_code);
  void seven_zip_unpack_finished(int exit_code);
  // return true if next password doesn't exists
  bool set_next_unpack_pwd();
  void unpack_next_item();
  void remove_unpacked_archive_files();
  QString escape_name_filter(const QString & name) const;
  QString exit_code_to_string(int exit_code) const;

  const QString unpack_queue_filename = "./unpack_queue.txt";
  unpack_settings unpack_prefs;
  unpacklog_settings log_prefs;
  QQueue<QString> unpack_queue;
  QProcess * unpack_process;
  QString last_text;
  QString curr_archive_filename;
  QString remove_filename_filter;
  QString working_dir;
  unpack_program current_unpack_app;
  int unpack_pwd_idx;
  bool is_shutdown;
  bool is_running;
  bool fatal_error_occured;
  const QString unpacklog_base_filename = "unpacklog";
  text_file_logger * unpack_logger;
};

#endif // UNPACK_H
