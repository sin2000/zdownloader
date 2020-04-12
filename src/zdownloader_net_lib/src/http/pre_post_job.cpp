#include "pre_post_job.h"
#include "gdrive/gdrive_api.h"
#include "../model/download_item.h"
#include "gdrive/gdrive_file.h"
#include "gdrive/gdrive_service.h"
#include <qdebugex.h>

pre_post_job::pre_post_job(QObject * parent)
  :QObject(parent),
   gapi(new gdrive_api(this)),
   in_use(false),
   dl_item(nullptr),
   access_token_success_func(nullptr),
   remaining_files(0)
{
  connect(gapi, &gdrive_api::request_access_token_finished, this, &pre_post_job::request_access_token_finished);
  connect(gapi, &gdrive_api::list_files_finished, this, &pre_post_job::list_files_finished);
  connect(gapi, &gdrive_api::copy_file_finished, this, &pre_post_job::copy_file_finished);
}

pre_post_job::~pre_post_job()
{
  gapi->stop_access_token_timer();
}

void pre_post_job::abort_all_connections()
{
  gapi->abort_all_connections();
  in_use = false;
  dl_item = nullptr;
  access_token_success_func = nullptr;
}

void pre_post_job::start_pre_job(download_item * item)
{
  if(is_gdrive_url(item->get_link()) == false)
  {
    emit pre_job_finished(item, true, false);
    return;
  }
  if(in_use)
  {
    emit pre_job_finished(item, false, true);
    return;
  }

  dl_item = item;
  src_file_id = gdrive_service::get_file_id_from_gdrive_url(dl_item->get_link());
  in_use = true;

  access_token_success_func = &pre_post_job::remove_gdrive_files;
  if(gapi->has_access_token_expired())
    gapi->request_access_token();
  else
    (this->*access_token_success_func)();
}

void pre_post_job::start_post_job(download_item * item)
{
  if(in_use == false)
  {
    emit post_job_finished(item);
    return;
  }

  if(item != dl_item)
  {
    qDebug() << "WARNING: start_post_job - item != dl_item";
    emit post_job_finished(item);
  }
  else
  {
    access_token_success_func = &pre_post_job::post_job_remove_file;
    if(gapi->has_access_token_expired())
      gapi->request_access_token();
    else
      (this->*access_token_success_func)();
  }
}

void pre_post_job::set_jwt_header(const jwt & header)
{
  gapi->set_jwt_header(header);
}

bool pre_post_job::reset_in_use(download_item * item)
{
  if(item == dl_item)
  {
    dl_item->set_link("https://drive.google.com/file/d/" + src_file_id);
    in_use = false;
    dl_item = nullptr;
    return true;
  }

  return false;
}

void pre_post_job::set_access_token(download_item * item)
{
  if(in_use && item == dl_item)
  {
    item->set_access_token(gapi->get_access_token());
  }
}

void pre_post_job::request_access_token_finished(const QString & /*access_token*/)
{  
  (this->*access_token_success_func)();
}

void pre_post_job::remove_gdrive_files()
{
  gapi->list_files();
}

void pre_post_job::list_files_finished(bool success, const QList<gdrive_file> & files)
{
  if(success == false)
  {
    end_pre_job_with_error();
    return;
  }

  remaining_files = files.size();
  if(files.isEmpty())
  {
    remove_file_finished(true);
  }
  else
  {
    disconnect(gapi, &gdrive_api::remove_file_finished, this, nullptr);
    connect(gapi, &gdrive_api::remove_file_finished, this, &pre_post_job::remove_file_finished);

    for(auto & file : files)
      gapi->remove_file(file.id);
  }
}

void pre_post_job::remove_file_finished(bool success)
{
  --remaining_files;
  if(remaining_files <= 0)
  {
    if(success == false)
    {
      end_pre_job_with_error();
    }
    else
    {
      gapi->copy_file(src_file_id);
    }
  }
}

void pre_post_job::copy_file_finished(const QString & dup_file_id)
{
  if(dup_file_id.isEmpty())
  {
    end_pre_job_with_error();
    return;
  }

  shared_file_id = dup_file_id;
  //gapi->share_file(shared_file_id);
  dl_item->set_link("https://drive.google.com/file/d/" + shared_file_id);
  emit pre_job_finished(dl_item, true, false);
}

void pre_post_job::end_pre_job_with_error()
{
  in_use = false;
  dl_item->set_link("https://drive.google.com/file/d/" + src_file_id);
  emit pre_job_finished(dl_item, false, false);
}

void pre_post_job::post_job_remove_file()
{
  disconnect(gapi, &gdrive_api::remove_file_finished, this, nullptr);
  connect(gapi, &gdrive_api::remove_file_finished, this, &pre_post_job::post_job_remove_file_finished);
  gapi->remove_file(shared_file_id);
}

void pre_post_job::post_job_remove_file_finished(bool /*success*/)
{
  emit post_job_finished(dl_item);
}

bool pre_post_job::is_gdrive_url(const QUrl & url) const
{
  return url.host().endsWith("drive.google.com");
}
