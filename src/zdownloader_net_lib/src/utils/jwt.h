#ifndef JWT_H
#define JWT_H

#include <QString>

class jwt
{
public:
  jwt();
  void set_iss(const QString & value);
  void set_scope(const QString & value);
  void set_aud(const QString & value);
  void set_private_key_pem(const QString & pem);
  QByteArray compose() const;

private:
  QString iss;
  QString scope;
  QString aud;
  QString priv_key_pem;
};

#endif // JWT_H
