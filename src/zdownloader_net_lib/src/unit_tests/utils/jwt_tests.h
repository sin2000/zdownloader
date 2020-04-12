#ifndef JWT_TESTS_H
#define JWT_TESTS_H

#include "../../utils/jwt.h"
#include <QObject>
#include <QTest>

class jwt_tests : public QObject
{
  Q_OBJECT
private slots:
  void compose_returns_correct_val()
  {
    jwt j;
    j.set_iss("761326798069-r5mljlln1rd4lrbhg75efgigp36m78j5@developer.gserviceaccount.com");
    j.set_scope("https://www.googleapis.com/auth/devstorage.readonly");
    j.set_aud("https://oauth2.googleapis.com/token");
    QByteArray c = j.compose();
    //int y = 4;
  }
};

#endif // JWT_TESTS_H
