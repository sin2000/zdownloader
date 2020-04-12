#ifndef WEB_CONFIG_H
#define WEB_CONFIG_H

#include <QString>

class QObject;

class web_config
{
public:
  static web_config & inst();

  // HTTP User-Agent header
  // "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.106 Safari/537.36"
  const QString http_user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.106 Safari/537.36";
  const QString http_x_www_form_urlencoded = "application/x-www-form-urlencoded";

private:
  web_config() {}
  ~web_config() {}
  web_config(web_config const &);
  void operator=(web_config const &);
};

#endif // WEB_CONFIG_H
