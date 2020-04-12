#ifndef EMPTY_COOKIE_JAR_H
#define EMPTY_COOKIE_JAR_H

#include <QNetworkCookieJar>

class empty_cookie_jar : public QNetworkCookieJar
{
public:
  empty_cookie_jar(QObject * parent = nullptr);

  bool insertCookie(const QNetworkCookie & cookie) final;
  bool updateCookie(const QNetworkCookie & cookie) final;
};

#endif // EMPTY_COOKIE_JAR_H
