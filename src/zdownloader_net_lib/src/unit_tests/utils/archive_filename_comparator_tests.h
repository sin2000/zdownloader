#ifndef ARCHIVE_FILENAME_COMPARATOR_TESTS_H
#define ARCHIVE_FILENAME_COMPARATOR_TESTS_H

#include "../../utils/archive_filename_comparator.h"
#include <QTest>

class archive_filename_comparator_tests : public QObject
{
  Q_OBJECT
private slots:
  void compare_returns_true()
  {
    QString f1 = "Game.Overlay.Renderer64.part01.rar";
    QString f2 = "Game.Overlay.Renderer64.part02.rar";

    archive_filename_comparator afc;
    bool ret = afc.compare(f1, f2);

    QCOMPARE(ret, true);
  }

  void compare_returns_true2()
  {
    QString f1 = "Game.Overlay.Renderer64.7z.001";
    QString f2 = "Game.Overlay.Renderer64.7z.002";

    archive_filename_comparator afc;
    bool ret = afc.compare(f1, f2);

    QCOMPARE(ret, true);
  }

  void compare_returns_false()
  {
    QString f1 = "Game.Overlay.Renderer64.part01.rar";
    QString f2 = "Game.Overlay.Renderer64.rar";

    archive_filename_comparator afc;
    bool ret = afc.compare(f1, f2);

    QCOMPARE(ret, false);
  }

  void compare_returns_false2()
  {
    QString f1 = "Renderer64.7z.001";
    QString f2 = "Renderer64.7z";

    archive_filename_comparator afc;
    bool ret = afc.compare(f1, f2);

    QCOMPARE(ret, false);
  }
};

#endif // ARCHIVE_FILENAME_COMPARATOR_TESTS_H
