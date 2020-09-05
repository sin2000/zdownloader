#include "multi_link_checker.h"
#include "zippyshare/zippy_service.h"
#include "megaup/megaup_service.h"
#include "gdrive/gdrive_service.h"
#include <QTimer>
#include <QUrl>
#include <qdebugex.h>

multi_link_checker::multi_link_checker(QNetworkAccessManager * global_nam, QObject * parent)
  :QObject(parent),
   nam(global_nam),
   max_parallel_link_checks(5),
   max_ms_before_check_link_again_on_error(10 * 1000),
   download_connections(0),
   download_restart_timer(new QTimer(this)),
   zippyshare_fetch_file_size(true),
   use_gdrive_api(false)
{
  download_restart_timer->setSingleShot(true);
  download_restart_timer->setInterval(max_ms_before_check_link_again_on_error);
  connect(download_restart_timer, &QTimer::timeout, this, &multi_link_checker::download_next_link_info);
}

multi_link_checker::~multi_link_checker()
{
  qDeleteAll(services);
}

void multi_link_checker::download_links_info(const QStringList & src_links)
{
  download_items.clear();
  download_connections = 0;
  queue.clear();
  queue.add(src_links);

  download_next_link_info();
}

void multi_link_checker::set_zippyshare_fetch_file_size(bool enabled)
{
  zippyshare_fetch_file_size = enabled;
}

void multi_link_checker::set_use_gdrive_api(bool gdrive_api_enabled)
{
  use_gdrive_api = gdrive_api_enabled;
}

void multi_link_checker::set_max_parallel_link_checks(int max)
{
  max_parallel_link_checks = max;
}

void multi_link_checker::set_max_ms_before_check_link_again_on_error(int ms)
{
  max_ms_before_check_link_again_on_error = ms;
}

void multi_link_checker::download_next_link_info()
{
  if(queue.has_pending_items() == false && queue.has_working_items() == false)
  {
    download_restart_timer->stop();
    emit download_links_info_finished(download_items);
    return;
  }

  if(queue.has_pending_items() == false)
    return;

  if(download_connections >= max_parallel_link_checks)
    return;

  ++download_connections;

  const QUrl url = queue.get_next();
  const QString host = url.host();

  service * dl_service = nullptr;
  if(host.endsWith("zippyshare.com"))
    dl_service = new zippy_service(nam, zippyshare_fetch_file_size);
  else
  {
    if(host.endsWith("megaup.net"))
      dl_service = new megaup_service(nam);
    else
    {
      if(host.endsWith("drive.google.com"))
      {
        auto * serv = new gdrive_service(nam);
        serv->set_use_gdrive_api(use_gdrive_api);
        dl_service = serv;
      }
    }
  }

  if(dl_service)
  {
    services.insert(dl_service);
    connect(dl_service, &service::fetch_download_info_success, this, &multi_link_checker::download_next_link_info_success);
    connect(dl_service, &service::fetch_download_info_error, this, &multi_link_checker::download_next_link_info_error);
    dl_service->fetch_download_info(url.toString());

    download_restart_timer->start(0);
  }
}

void multi_link_checker::download_next_link_info_success(const download_item & info, service * sender)
{
  --download_connections;

  download_items.append(info);
  queue.finish(info.get_link());

  download_restart_timer->start(0);

  sender->deleteLater();
  services.remove(sender);
}

void multi_link_checker::download_next_link_info_error(const QString & error, service::fetch_error error_code, service * sender)
{
  --download_connections;
  if(error_code == service::network_error || error_code == service::unknown_error)
  {
    queue.stop(sender->get_base_url());
    qDebug() << "Restarting download link info. Error:" << error;
    download_restart_timer->start(max_ms_before_check_link_again_on_error);
  }
  else
  {
    qDebug() << "multi_link_checker -" << error << "[Link:" << sender->get_base_url() + "]";

    download_item item;
    item.set_link(sender->get_base_url());
    set_dl_item_status(&item, error_code);
    download_items.append(item);

    queue.finish(sender->get_base_url());
    download_restart_timer->start(0);
  }

  sender->deleteLater();
  services.remove(sender);
}

void multi_link_checker::set_dl_item_status(download_item * item, service::fetch_error fetch_err) const
{
  switch(fetch_err)
  {
    case service::file_does_not_exists:
      item->set_status(download_item::download_status_remote_file_does_not_exists);
      break;
    case service::not_found_404_error:
      item->set_status(download_item::download_status_remote_file_not_found);
      break;

    default:
      break;
  }
}
