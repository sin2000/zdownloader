#ifndef BOTAN_WRAPPER_H
#define BOTAN_WRAPPER_H

#include "zd_shared_lib_global.h"

class QByteArray;
class botan_wrapper_private;

class ZD_SHARED_LIB_EXPORT botan_wrapper
{
public:
  botan_wrapper();
  ~botan_wrapper();

  void rsa_sign(const QByteArray & input_data, const QString & privkey_pem, QByteArray * sign_b64_url) const;

private:
  botan_wrapper_private * priv;
};

#endif // BOTAN_WRAPPER_H
