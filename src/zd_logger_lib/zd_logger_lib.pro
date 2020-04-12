# Configuration directives:
# UNIT_TEST - indicates unit test mode

include(../zd_shared_lib/zd_shared.pri)

DEFINES += ZD_LOGGER_LIBRARY

QT -= gui

CONFIG(debug, debug|release) {
    TARGET = zd_logger_libd
}
else {
    TARGET = zd_logger_lib
}

TEMPLATE = lib

# ZD SHARED LIBRARY =========================================================================================
CONFIG(release, debug|release): LIBS += -L$$PWD/../zd_shared_lib/bin_release/ -lzd_shared_lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../zd_shared_lib/bin_debug/ -lzd_shared_libd

INCLUDEPATH += $$PWD/../zd_shared_lib/src
DEPENDPATH += $$PWD/../zd_shared_lib/src
# ZD SHARED LIBRARY END =====================================================================================

INCLUDEPATH += ./src

SOURCES += \
    src/zd_logger.cpp \
    src/private/file_logger.cpp \
    src/private/io_thread.cpp \
    src/private/app_logger.cpp \
    src/private/debug_logger.cpp \
    src/private/text_file_logger.cpp \
    src/private/text_logger_manager.cpp

HEADERS += \
    src/zd_logger.h \
    src/zd_logger_global.h \
    src/private/file_logger.h \
    src/private/msg_log_context.h \
    src/private/io_thread.h \
    src/private/app_logger.h \
    src/private/debug_logger.h \
    src/private/text_file_logger.h \
    src/private/text_logger_manager.h \
    src/qdebugex.h

unit_test {
  message("UnitTest MODE ENABLED")

  QT += testlib
  TEMPLATE = app

  DEFINES += UNIT_TEST
  HEADERS += \
    src/unit_tests/*.h \
    src/unit_tests/private/*.h

  SOURCES += src/unit_tests/*.cpp
}
else {
    zd_static_linkage {
        CONFIG += staticlib
    }
}
