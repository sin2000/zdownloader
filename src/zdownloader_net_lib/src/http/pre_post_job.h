#ifndef PRE_POST_JOB_H
#define PRE_POST_JOB_H

#include <QObject>

class download_item;
class gdrive_api;
class gdrive_file;
class jwt;

class pre_post_job : public QObject
{
  Q_OBJECT
public:
  pre_post_job(QObject * parent = nullptr);
  ~pre_post_job();
  void abort_all_connections();
  void start_pre_job(download_item * item);
  void start_post_job(download_item * item);
  void set_jwt_header(const jwt & header);
  bool reset_in_use(download_item * item);
  void set_access_token(download_item * item);

signals:
  void pre_job_finished(download_item * item, bool success, bool should_pause_item);
  void post_job_finished(download_item * item);

private:
  void request_access_token_finished(const QString & access_token);
  void remove_gdrive_files();
  void list_files_finished(bool success, const QList<gdrive_file> & files);
  void remove_file_finished(bool success);
  void copy_file_finished(const QString & dup_file_id);
  void end_pre_job_with_error();

  void post_job_remove_file();
  void post_job_remove_file_finished(bool success);

  bool is_gdrive_url(const QUrl & url) const;

  gdrive_api * gapi;
  bool in_use;
  download_item * dl_item;
  void (pre_post_job::*access_token_success_func)(void);
  int remaining_files;
  QString shared_file_id;
  QString src_file_id;
};

#endif // PRE_POST_JOB_H
