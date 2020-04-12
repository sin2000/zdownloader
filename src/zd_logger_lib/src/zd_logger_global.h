#ifndef ZD_LOGGER_GLOBAL_H
#define ZD_LOGGER_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef ZD_STATIC_LINKAGE
  #if defined(UNIT_TEST) && defined(ZD_LOGGER_LIBRARY)
    #define ZD_LOGGERSHARED_EXPORT
  #else
    #ifdef ZD_LOGGER_LIBRARY
      #define ZD_LOGGERSHARED_EXPORT Q_DECL_EXPORT
    #else
      #define ZD_LOGGERSHARED_EXPORT Q_DECL_IMPORT
    #endif
  #endif
#else
  #define ZD_LOGGERSHARED_EXPORT
#endif

#endif // ZD_LOGGER_GLOBAL_H
