#ifndef LOGGED_NAM_H
#define LOGGED_NAM_H

#include <QNetworkAccessManager>
#include <QNetworkReply>

class logged_nam : public QNetworkAccessManager
{
public:
  logged_nam(QObject * parent = nullptr);

private:
  //void network_finished(QNetworkReply * reply);
  void ssl_errors(QNetworkReply * reply, const QList<QSslError> & errors);
};

#endif // LOGGED_NAM_H
