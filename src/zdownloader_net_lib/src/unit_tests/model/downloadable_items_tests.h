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

  void sort_and_add_behaves_correctly()
  {
    download_item item, item2, item3;
    item.set_status_from_char('P');
    item2.set_status_from_char('P');
    item3.set_status_from_char('P');
    item.set_link("a");
    item2.set_link("b");
    item3.set_link("c");
    item.set_filename("Game.Overlay.Renderer64.part01.rar");
    item2.set_filename("Game.Overlay.Renderer64.part02.rar");
    item3.set_filename("Something.part01.rar");

    const QList<download_item> items = { item2, item, item3 };

    downloadable_items dl_items;
    dl_items.sort_and_add(items);

    download_item * cur_item1 = dl_items.get_next();
    download_item * cur_item2 = dl_items.get_next();
    download_item * cur_item3 = dl_items.get_next();

    QCOMPARE(cur_item1->get_group_id(), 1);
    QCOMPARE(cur_item2->get_group_id(), 1);
    QCOMPARE(cur_item3->get_group_id(), 2);
  }

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
