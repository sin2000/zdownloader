#include <QTest>
#include "model/downloadable_items_tests.h"
#include "http/zippy_link_extractor_tests.h"
#include "http/link_manager_tests.h"
#include "http/segment_calculator_tests.h"
#include "utils/jwt_tests.h"
#include "http/gdrive/gdrive_api_tests.h"

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);

  int status = 0;
//  {
//    downloadable_items_tests tc;
//    status |= QTest::qExec(&tc, argc, argv);
//  }

  {
    zippy_link_extractor_tests tc;
    status |= QTest::qExec(&tc, argc, argv);
  }

//  {
//    link_manager_tests tc;
//    status |= QTest::qExec(&tc, argc, argv);
//  }

  {
    segment_calculator_tests tc;
    status |= QTest::qExec(&tc, argc, argv);
  }

//  {
//    jwt_tests tc;
//    status |= QTest::qExec(&tc, argc, argv);
//  }

//  {
//    gdrive_api_tests tc;
//    status |= QTest::qExec(&tc, argc, argv);
//  }

  return status;
}
