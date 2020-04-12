#ifndef DOWNLOADABLE_ITEMS_TESTS_H
#define DOWNLOADABLE_ITEMS_TESTS_H

#include "../../model/downloadable_items.h"
#include "../../model/download_item.h"
#include <QTest>

class downloadable_items_tests : public QObject
{
  Q_OBJECT
private slots:
//  void load_from_file_behaves_correctly()
//  {
//    downloadable_items dl_items;
//    dl_items.load_from_file();
//  }

//  void sort_and_add_behaves_correctly()
//  {
//    download_item item, item2;
//    item.set_status_from_char('P');
//    item.filename = "dodany01.rar";
//    item.link = "link-do-serwisu";
//    item2.set_status_from_char('P');
//    item2.filename = "dodany02.rar";
//    item2.link = "link-do-serw2";

//    const QList<download_item> items = { item2, item };

//    downloadable_items dl_items;
//    dl_items.sort_and_add(items);

//    download_item cur_item;
//    const bool cur_item_success = dl_items.get_first(&cur_item);

//    QCOMPARE(cur_item_success, true);
//    QCOMPARE(cur_item.filename, QString("aaa.part02.rar"));
//    QCOMPARE(cur_item.link, QString("https://www41.zippyshare.com/v/pierwszy/file.html"));
//    QCOMPARE(cur_item.status, download_item::download_status_pending);
//  }

//  void remove_first_behaves_correctly()
//  {
//    downloadable_items dl_items;

//    download_item cur_item;
//    dl_items.remove_first();
//    const bool cur_item_success = dl_items.get_first(&cur_item);

//    QCOMPARE(cur_item_success, true);
//    QCOMPARE(cur_item.filename, QString("bbb.part01.rar"));
//    QCOMPARE(cur_item.link, QString("https://www120.zippyshare.com/v/drugi/file.html"));
//    QCOMPARE(cur_item.status, download_item::download_status_pending);
//  }

  //  void to_url_query_when_empty_returns_correct_results()
//  {
//    scp_user_login_input input;

//    QString result = input.to_url_query().toString(QUrl::FullyEncoded);

//    QCOMPARE(result, QString("pubk=eeeee"));
//  }
};

#endif // DOWNLOADABLE_ITEMS_TESTS_H
