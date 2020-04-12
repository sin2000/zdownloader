#ifndef NET_PROXY_SETTINGS_H
#define NET_PROXY_SETTINGS_H

#include <QString>
#include <QNetworkProxy>

class net_proxy_settings
{
public:
  bool enable_network_proxy = false;
  QNetworkProxy::ProxyType proxy_type = QNetworkProxy::Socks5Proxy;
  QString hostname;
  quint16 port = 8000;
  QString username;
  QString password;
};

#endif // NET_PROXY_SETTINGS_H
