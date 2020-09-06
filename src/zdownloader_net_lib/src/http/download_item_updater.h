#ifndef DOWNLOAD_ITEM_UPDATER_H
#define DOWNLOAD_ITEM_UPDATER_H

#include "service.h"
#include <QObject>

class QNetworkAccessManager;
class multi_link_checker;
class download_item;

class download_item_updater : public QObject
{
  Q_OBJECT
public:
  download_item_updater(int max_ms_before_check_link_again_on_error, QNetworkAccessManager * global_nam, QObject * parent = nullptr);
  void start_update(download_item * item);
  download_item * get_updatable_item() const;

signals:
  void update_finished(download_item_updater * sender, download_item * updated_item);

private:
  void download_links_info_finished(const QList<download_item> & infos);

  multi_link_checker * link_checker;
  download_item * updatable_item;
};

#endif // DOWNLOAD_ITEM_UPDATER_H
