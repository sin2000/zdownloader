#ifndef STRING_UTILS_TESTS_H
#define STRING_UTILS_TESTS_H

#include "../string_utils.h"
#include <QObject>
#include <QTest>

class string_utils_tests : public QObject
{
  Q_OBJECT
private slots:
  void seconds_to_string_test1()
  {
    qint64 seconds = 0;

    const QString ret = string_utils::seconds_to_string(seconds);

    QCOMPARE(ret, "0s");
  }

  void seconds_to_string_test2()
  {
    qint64 seconds = 59;

    const QString ret = string_utils::seconds_to_string(seconds);

    QCOMPARE(ret, "59s");
  }

  void seconds_to_string_test3()
  {
    qint64 seconds = 60;

    const QString ret = string_utils::seconds_to_string(seconds);

    QCOMPARE(ret, "1m0s");
  }

  void seconds_to_string_test4()
  {
    qint64 seconds = 61;

    const QString ret = string_utils::seconds_to_string(seconds);

    QCOMPARE(ret, "1m1s");
  }

  void seconds_to_string_test5()
  {
    qint64 seconds = 3600;

    const QString ret = string_utils::seconds_to_string(seconds);

    QCOMPARE(ret, "1h0m0s");
  }

  void seconds_to_string_test6()
  {
    qint64 seconds = 3662;

    const QString ret = string_utils::seconds_to_string(seconds);

    QCOMPARE(ret, "1h1m2s");
  }

  void seconds_to_string_test7()
  {
    qint64 seconds = 24 * 3600;

    const QString ret = string_utils::seconds_to_string(seconds);

    QCOMPARE(ret, "1d0h0m0s");
  }

  void seconds_to_string_test8()
  {
    qint64 seconds = (24 * 3600) + 3662;

    const QString ret = string_utils::seconds_to_string(seconds);

    QCOMPARE(ret, "1d1h1m2s");
  }

  void seconds_to_string_test9()
  {
    qint64 seconds = (24 * 3600) + 34624;

    const QString ret = string_utils::seconds_to_string(seconds);

    QCOMPARE(ret, "1d9h37m4s");
  }
};

#endif // STRING_UTILS_TESTS_H
