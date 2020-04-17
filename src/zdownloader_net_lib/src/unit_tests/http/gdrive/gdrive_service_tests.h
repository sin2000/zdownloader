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
    QString url = "https://drive.google.com/file/d/0B1MVW1mFO2zmZHVRWEQ3Rkc3SVE";

    QString file_id = gdrive_service::get_file_id_from_gdrive_url(url);

    QCOMPARE(file_id, "0B1MVW1mFO2zmZHVRWEQ3Rkc3SVE");
  }

  void get_file_id_from_gdrive_url_returns_correct_id2()
  {
    QString url = "https://drive.google.com/file/d/0B1MVW1mFO2zmZHVRWEQ3Rkc3SVE/edit";

    QString file_id = gdrive_service::get_file_id_from_gdrive_url(url);

    QCOMPARE(file_id, "0B1MVW1mFO2zmZHVRWEQ3Rkc3SVE");
  }

  void get_file_id_from_gdrive_url_returns_correct_id3()
  {
    QString url = "https://drive.google.com/file/d/0B1MVW1mFO2zmZHVRWEQ3Rkc3SVE/view";

    QString file_id = gdrive_service::get_file_id_from_gdrive_url(url);

    QCOMPARE(file_id, "0B1MVW1mFO2zmZHVRWEQ3Rkc3SVE");
  }

  void get_file_id_from_gdrive_url_returns_correct_id4()
  {
    QString url = "https://drive.google.com/uc?id=0B1MVW1mFO2zmZHVRWEQ3Rkc3SVE";

    QString file_id = gdrive_service::get_file_id_from_gdrive_url(url);

    QCOMPARE(file_id, "0B1MVW1mFO2zmZHVRWEQ3Rkc3SVE");
  }

  void get_file_id_from_gdrive_url_returns_correct_id5()
  {
    QString url = "https://drive.google.com/open?id=0B1MVW1mFO2zmZHVRWEQ3Rkc3SVE";

    QString file_id = gdrive_service::get_file_id_from_gdrive_url(url);

    QCOMPARE(file_id, "0B1MVW1mFO2zmZHVRWEQ3Rkc3SVE");
  }

  void get_file_id_from_gdrive_url_returns_correct_id6()
  {
    QString url = "https://drive.google.com/uc?id=0B1MVW1mFO2zmZHVRWEQ3Rkc3SVE&export=download";

    QString file_id = gdrive_service::get_file_id_from_gdrive_url(url);

    QCOMPARE(file_id, "0B1MVW1mFO2zmZHVRWEQ3Rkc3SVE");
  }

  void get_file_id_from_gdrive_url_returns_empty_id()
  {
    QString url = "https://drive.google.com/corona";

    QString file_id = gdrive_service::get_file_id_from_gdrive_url(url);

    QCOMPARE(file_id, "");
  }
};

#endif // GDRIVE_SERVICE_TESTS_H
