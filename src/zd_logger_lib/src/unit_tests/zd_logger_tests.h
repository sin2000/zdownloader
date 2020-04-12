#ifndef ZD_LOGGER_TESTS_H
#define ZD_LOGGER_TESTS_H

#include "zd_logger.h"
#include <QObject>
#include <QTest>
#include <QDebug>
#include <QThread>

class zd_logger_tests : public QObject
{
  Q_OBJECT

private slots:
  void test_case()
  {
    zd_logger::inst().install_logger("./");

    qDebug() << "dupa";
    qDebug() << "dupa2";
    qDebug() << "dupa3";

    zd_logger::inst().uninstall_logger();

    QCOMPARE("a", "a");
  }
};

#endif // ZD_LOGGER_TESTS_H
