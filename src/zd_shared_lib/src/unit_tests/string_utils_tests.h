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

  void bytes_to_human_readable_string_returns_correct_value()
  {
    QString ret = string_utils::bytes_to_human_readable_string(0);

    QCOMPARE(ret, "0.00B");
  }

  void bytes_to_human_readable_string_returns_correct_value2()
  {
    QString ret = string_utils::bytes_to_human_readable_string(1);

    QCOMPARE(ret, "1.00B");
  }

  void bytes_to_human_readable_string_returns_correct_value3()
  {
    QString ret = string_utils::bytes_to_human_readable_string(1023);

    QCOMPARE(ret, "1023.00B");
  }

  void bytes_to_human_readable_string_returns_correct_value4()
  {
    QString ret = string_utils::bytes_to_human_readable_string(1024);

    QCOMPARE(ret, "1.00KB");
  }

  void bytes_to_human_readable_string_returns_correct_value5()
  {
    QString ret = string_utils::bytes_to_human_readable_string(1728);

    QCOMPARE(ret, "1.69KB");
  }

  void bytes_to_human_readable_string_returns_correct_value6()
  {
    QString ret = string_utils::bytes_to_human_readable_string(7077888);

    QCOMPARE(ret, "6.75MB");
  }

  void bytes_to_human_readable_string_returns_correct_value7()
  {
    QString ret = string_utils::bytes_to_human_readable_string(Q_INT64_C(27*1024*1024*1024));

    QCOMPARE(ret, "27.00GB");
  }

  void bytes_to_human_readable_string_returns_correct_value8()
  {
    QString ret = string_utils::bytes_to_human_readable_string(Q_INT64_C(1855425871872));

    QCOMPARE(ret, "1.69TB");
  }

  void remove_all_whitespace_returns_correct()
  {
    QString ret = string_utils::remove_all_whitespace(" test   two\n\r\n\tTHREE");

    QCOMPARE(ret, "testtwoTHREE");
  }
};

#endif // STRING_UTILS_TESTS_H
