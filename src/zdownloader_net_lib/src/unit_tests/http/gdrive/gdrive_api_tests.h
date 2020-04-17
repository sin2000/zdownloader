#ifndef GDRIVE_API_TESTS_H
#define GDRIVE_API_TESTS_H

#include "../../../http/gdrive/gdrive_api.h"
#include "../../../utils/jwt.h"
#include "../../../http/gdrive/gdrive_file.h"
#include <QNetworkProxy>
#include <QTest>

class gdrive_api_tests : public QObject
{
  Q_OBJECT
private slots:
  void request_access_token_test()
  {
    jwt jwt_header;
    jwt_header.set_aud("https://oauth2.googleapis.com/token");
    jwt_header.set_scope("https://www.googleapis.com/auth/drive");

    jwt_header.set_iss("somesome-acc@drapiproj.iam.gserviceaccount.com");
    jwt_header.set_private_key_pem("-----BEGIN PRIVATE KEY-----\nMII... CUTTED LOL");

    QSslConfiguration ssl_conf = QSslConfiguration::defaultConfiguration();
    ssl_conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    QSslConfiguration::setDefaultConfiguration(ssl_conf);
    QNetworkProxy proxy(QNetworkProxy::Socks5Proxy, "127.0.0.1", 8889);
    QNetworkProxy::setApplicationProxy(proxy);

    gdrive_api gapi;
    gapi.set_jwt_header(jwt_header);
    connect(&gapi, &gdrive_api::request_access_token_finished, [&](const QString & /*acc_token*/) {
      connect(&gapi, &gdrive_api::file_info_finished, [&](QNetworkReply::NetworkError , const gdrive_file & /*info*/) {
        //gapi.remove_file("0B1MVW1mFO2zmZHVRWEQ3Rkc3SVE");
//        for(auto & f : files)
//        {
//          qDebug() << "FILE:" << f.name << "ID:" << f.id;
//        }
      });

      gapi.file_info("0B1MVW1mFO2zmZHVRWEQ3Rkc3SVE");
    });

    gapi.request_access_token();

    //gapi.share_file("0B1MVW1mFO2zmZHVRWEQ3Rkc3SVE");



    QEventLoop loop;
    //connect(this, &net_thread::init_finished, &loop, &QEventLoop::quit, Qt::QueuedConnection);

    loop.exec();
  }
};

#endif // GDRIVE_API_TESTS_H
