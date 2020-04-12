#include "logged_nam.h"
#include "empty_cookie_jar.h"
#include <meta_object_ext.h>
#include <QNetworkReply>
#include <QSslKey>
#include <qdebugex.h>

logged_nam::logged_nam(QObject * parent)
  :QNetworkAccessManager(parent)
{
  connect(this, &QNetworkAccessManager::sslErrors, this, &logged_nam::ssl_errors);
  //connect(this, &QNetworkAccessManager::finished, this, &logged_nam::network_finished);

  setCookieJar(new empty_cookie_jar);
}

//void logged_nam::network_finished(QNetworkReply * reply)
//{
//  if(reply->error() != QNetworkReply::NoError)
//  {
//    qDebug() << "HTTP INFO:" << reply->errorString() << "for url:" << reply->url().toString();
//  }
//}

void logged_nam::ssl_errors(QNetworkReply * reply, const QList<QSslError> & errors)
{
  qDebug() << reply->url() << "ssl errors:" << errors;
}
