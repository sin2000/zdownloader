#ifndef zdownloader_net_lib_global_H
#define zdownloader_net_lib_global_H

#include <QtCore/qglobal.h>

#ifndef ZD_STATIC_LINKAGE
  #if defined(UNIT_TEST) && defined(ZDOWNLOADER_NET_LIB)
    #define ZD_NET_LIBSHARED_EXPORT
  #else
    #ifdef ZDOWNLOADER_NET_LIB
      #define ZD_NET_LIBSHARED_EXPORT Q_DECL_EXPORT
    #else
      #define ZD_NET_LIBSHARED_EXPORT Q_DECL_IMPORT
    #endif
  #endif
#else
  #define ZD_NET_LIBSHARED_EXPORT
#endif

#endif // zdownloader_net_lib_global_H
