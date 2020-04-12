//#include "AutoTest.h"

//TEST_MAIN

#include "zd_logger_tests.h"
#include "private/file_logger_tests.h"
#include <QTest>

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);

  int status = 0;
  {
    zd_logger_tests tc;
    status |= QTest::qExec(&tc, argc, argv);
  }

  {
    file_logger_tests tc;
    status |= QTest::qExec(&tc, argc, argv);
  }

  return status;
}
