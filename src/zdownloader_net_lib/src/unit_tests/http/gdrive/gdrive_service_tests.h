#ifndef GDRIVE_SERVICE_TESTS_H
#define GDRIVE_SERVICE_TESTS_H

#include "../../../http/gdrive/gdrive_service.h"
#include <QTest>

class gdrive_service_tests : public QObject
{
  Q_OBJECT
private slots:
  void get_file_id_from_gdrive_url_returns_correct_id()
  {

  }
};

#endif // GDRIVE_SERVICE_TESTS_H
