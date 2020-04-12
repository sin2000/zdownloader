#ifndef FILE_LOGGER_TESTS_H
#define FILE_LOGGER_TESTS_H

#include <QTest>
#include <QObject>
#include <QDebug>
#include "private/file_logger.h"

class file_logger_tests : public QObject
{
  Q_OBJECT

private slots:
  void get_filename_from_pattern_returns_correct_result()
  {
    QString expected = "applog_123.log";
    file_logger fl;

    QString result = fl.get_filename_from_pattern(123);

    QCOMPARE(result, expected);
  }

  void open_log_when_opened_returns_false()
  {
    bool expected = false;
    file_logger fl;
    fl.set_directory_path("./");
    fl.open_log();

    bool result = fl.open_log();

    QCOMPARE(result, expected);
  }

  void open_log_when_empty_dirpath_returns_false()
  {
    bool expected = false;
    file_logger fl;
    fl.set_directory_path("");

    bool result = fl.open_log();

    QCOMPARE(result, expected);
  }

  void save_log_writes_to_file()
  {
    file_logger fl;
    fl.set_directory_path("./");
    fl.set_filename_pattern("testlog_%archive_counter%.log");
    fl.set_max_rotate_bytes(6);
    fl.open_log();

    fl.save_log("a");
    fl.save_log("b");

    fl.save_log("c");
    fl.save_log("d");

    fl.save_log("e");
    fl.save_log("f");

    fl.save_log("g");
    fl.save_log("h");
  }

  void save_log_writes_to_multi_file()
  {
    file_logger fl("", true);
    fl.set_directory_path("./");
    fl.set_filename_pattern("multitestlog_%archive_counter%.log");
    fl.set_max_rotate_bytes(6);
    fl.open_log();

    fl.save_log("a");
    fl.save_log("b");

    file_logger f2("", true);
    f2.set_directory_path("./");
    f2.set_filename_pattern("multitestlog_%archive_counter%.log");
    f2.set_max_rotate_bytes(6);
    f2.open_log();

    f2.save_log("a");
    f2.save_log("b");

    f2.save_log("c");
    f2.save_log("d");

    f2.save_log("e");
    f2.save_log("f");

    f2.save_log("g");
    f2.save_log("h");
  }
};

//DECLARE_TEST(file_logger_tests)

#endif // FILE_LOGGER_TESTS_H
