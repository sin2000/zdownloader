include(zd_shared.pri)

DEFINES += ZD_SHARED_LIB_LIBRARY

QT -= gui

CONFIG(debug, debug|release) {
    TARGET = zd_shared_libd
}
else {
    TARGET = zd_shared_lib
}

TEMPLATE = lib

# BOTAN SHARED LIBRARY =========================================================================================
inc_botan_wrapper {
    unix:amd64 {
        LIBS += -L$$PWD/../zd_shared_lib/src/lib/botan/linux64/lib/ -lbotan-2
        INCLUDEPATH += $$PWD/../zd_shared_lib/src/lib/botan/linux64/include
        DEPENDPATH += $$PWD/../zd_shared_lib/src/lib/botan/linux64/include
    }
    unix:arm64 {
        LIBS += -L$$PWD/../zd_shared_lib/src/lib/botan/arm64/lib/ -lbotan-2
        INCLUDEPATH += $$PWD/../zd_shared_lib/src/lib/botan/arm64/include
        DEPENDPATH += $$PWD/../zd_shared_lib/src/lib/botan/arm64/include
    }
    unix:arm32 {
        LIBS += -L$$PWD/../zd_shared_lib/src/lib/botan/arm32/lib/ -lbotan-2
        INCLUDEPATH += $$PWD/../zd_shared_lib/src/lib/botan/arm32/include
        DEPENDPATH += $$PWD/../zd_shared_lib/src/lib/botan/arm32/include
    }
    win32 {
        win_64bit {
            LIBS += -L$$PWD/../zd_shared_lib/src/lib/botan/win64/lib/ -lbotan
            INCLUDEPATH += $$PWD/../zd_shared_lib/src/lib/botan/win64/include
            DEPENDPATH += $$PWD/../zd_shared_lib/src/lib/botan/win64/include
        }
        else {
            LIBS += -L$$PWD/../zd_shared_lib/src/lib/botan/win32/lib/ -lbotan
            INCLUDEPATH += $$PWD/../zd_shared_lib/src/lib/botan/win32/include
            DEPENDPATH += $$PWD/../zd_shared_lib/src/lib/botan/win32/include
        }
    }
}
#QMAKE_LFLAGS += -Wl,-rpath,"."
# BOTAN SHARED LIBRARY END =====================================================================================

INCLUDEPATH += ./src

SOURCES += \
    src/meta_object_ext.cpp \
    src/string_utils.cpp \
    src/increasing_timer.cpp

HEADERS += \
    src/meta_object_ext.h \
    src/zd_shared_lib_global.h \
    src/name_to_object_binder.h \
    src/string_utils.h \
    src/increasing_timer.h

inc_botan_wrapper {
    SOURCES += \
        src/botan_wrapper.cpp \
        src/botan_wrapper_private.cpp

    HEADERS += \
        src/botan_wrapper.h \
        src/botan_wrapper_private.h
}

unit_test {
  message("UnitTest MODE ENABLED")

  QT += testlib script
  TEMPLATE = app

  SOURCES +=
  HEADERS +=

  DEFINES += UNIT_TEST
  HEADERS += \
    src/unit_tests/*.h

  SOURCES += src/unit_tests/*.cpp
}
else {
    inc_js_utils {
        SOURCES +=
        HEADERS +=
    }

    zd_static_linkage {
        CONFIG += staticlib
    }
}
