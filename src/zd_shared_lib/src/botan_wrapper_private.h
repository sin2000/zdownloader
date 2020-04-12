#ifndef BOTAN_WRAPPER_PRIVATE_H
#define BOTAN_WRAPPER_PRIVATE_H 

#include <botan/rng.h>
#include <botan/rsa.h>

class QByteArray;

class botan_wrapper_private
{
public:
  botan_wrapper_private();
  ~botan_wrapper_private();

  Botan::RandomNumberGenerator * get_rng() const;

private:
  Botan::RandomNumberGenerator * rng;
};

#endif // BOTAN_WRAPPER_PRIVATE_H
