#include "botan_wrapper_tests.h"
#include "string_utils_tests.h"
#include <QTest>

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);

  int status = 0;

  {
    botan_wrapper_tests tc;
    status |= QTest::qExec(&tc, argc, argv);
  }

  {
    string_utils_tests tc;
    status |= QTest::qExec(&tc, argc, argv);
  }

  return status;
}
