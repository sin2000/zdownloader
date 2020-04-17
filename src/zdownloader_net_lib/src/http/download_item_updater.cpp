#include "download_item_updater.h"
#include "multi_link_checker.h"
#include "../model/download_item.h"
#include <qdebugex.h>

download_item_updater::download_item_updater(int max_ms_before_check_link_again_on_error, QNetworkAccessManager * global_nam, QObject * parent)
  :QObject(parent),
   link_checker(new multi_link_checker(global_nam, this)),
   updatable_item(nullptr)
{
  connect(link_checker, &multi_link_checker::download_links_info_success, this, &download_item_updater::download_links_info_success);
  connect(link_checker, &multi_link_checker::download_error, this, &download_item_updater::download_error);
  link_checker->set_max_ms_before_check_link_again_on_error(max_ms_before_check_link_again_on_error);
}

void download_item_updater::start_update(download_item * item)
{
  updatable_item = item;

  if(updatable_item->get_access_token().isEmpty() == false)
    link_checker->set_use_gdrive_api(true);

  if(updatable_item->get_file_size_bytes() > 0)
    link_checker->set_zippyshare_fetch_file_size(false);
  else
    link_checker->set_zippyshare_fetch_file_size(true);

  link_checker->download_links_info(QStringList(updatable_item->get_link()));
}

download_item * download_item_updater::get_updatable_item() const
{
  return updatable_item;
}

void download_item_updater::download_links_info_success(const QList<download_item> & infos)
{
  if(infos.isEmpty() == false)
  {
    const download_item & item = infos.first();
    //updatable_item->set_filename(item.get_filename());
    updatable_item->set_direct_download_link(item.get_direct_download_link());
    if(item.get_file_size_bytes() > 0)
      updatable_item->set_file_size_bytes(item.get_file_size_bytes());
  }
  else
  {
    qDebug() << "WARNING: links_infos is empty! This shouldnt happen. Application is broken";
  }

  emit update_finished(this, service::unknown_error, "", updatable_item);
}

void download_item_updater::download_error(service::fetch_error error_code, const QString & error_text)
{
  emit update_finished(this, error_code, error_text, updatable_item);
}
