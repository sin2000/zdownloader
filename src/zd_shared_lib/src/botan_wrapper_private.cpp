#include "botan_wrapper_private.h"
#include <botan/system_rng.h>

botan_wrapper_private::botan_wrapper_private()
  :rng(new Botan::System_RNG)
{
}

botan_wrapper_private::~botan_wrapper_private()
{
  delete rng;
}

Botan::RandomNumberGenerator * botan_wrapper_private::get_rng() const
{
  return rng;
}
