#ifndef ZD_SHARED_LIB_GLOBAL_H
#define ZD_SHARED_LIB_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef ZD_STATIC_LINKAGE
  #if defined(UNIT_TEST) && defined(ZD_SHARED_LIB_LIBRARY)
    #define ZD_SHARED_LIB_EXPORT
  #else
    #ifdef ZD_SHARED_LIB_LIBRARY
      #define ZD_SHARED_LIB_EXPORT Q_DECL_EXPORT
    #else
      #define ZD_SHARED_LIB_EXPORT Q_DECL_IMPORT
    #endif
  #endif
#else
  #define ZD_SHARED_LIB_EXPORT
#endif

#endif // ZD_SHARED_LIB_GLOBAL_H
