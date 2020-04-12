#include "empty_cookie_jar.h"
#include <QNetworkCookie>

empty_cookie_jar::empty_cookie_jar(QObject * parent)
  :QNetworkCookieJar(parent)
{
}

bool empty_cookie_jar::insertCookie(const QNetworkCookie & /*cookie*/)
{
  return true;
}

bool empty_cookie_jar::updateCookie(const QNetworkCookie & /*cookie*/)
{
  return true;
}
