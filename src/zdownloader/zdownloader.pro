include(../zd_shared_lib/zd_shared.pri)

QT += network
QT -= gui

CONFIG += console
CONFIG -= app_bundle

CONFIG(debug, debug|release) {
    TARGET = zdownloaderd
}
else {
    TARGET = zdownloader
    QMAKE_POST_LINK=$(STRIP) -s $(TARGET)
}

# ZDOWNLOADER NET LIBRARY ============================================================================================
CONFIG(release, debug|release): LIBS += -L$$PWD/../zdownloader_net_lib/bin_release/ -lzdownloader_net_lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../zdownloader_net_lib/bin_debug/ -lzdownloader_net_libd

INCLUDEPATH += $$PWD/../zdownloader_net_lib/src
DEPENDPATH += $$PWD/../zdownloader_net_lib/src
# ZDOWNLOADER NET LIBRARY END ========================================================================================

# ZD LOGGER LIBRARY =========================================================================================
CONFIG(release, debug|release): LIBS += -L$$PWD/../zd_logger_lib/bin_release/ -lzd_logger_lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../zd_logger_lib/bin_debug/ -lzd_logger_libd

INCLUDEPATH += $$PWD/../zd_logger_lib/src
DEPENDPATH += $$PWD/../zd_logger_lib/src
# ZD LOGGER LIBRARY END =====================================================================================

# ZD SHARED LIBRARY =========================================================================================
CONFIG(release, debug|release): LIBS += -L$$PWD/../zd_shared_lib/bin_release/ -lzd_shared_lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../zd_shared_lib/bin_debug/ -lzd_shared_libd

INCLUDEPATH += $$PWD/../zd_shared_lib/src
DEPENDPATH += $$PWD/../zd_shared_lib/src
# ZD SHARED LIBRARY END =====================================================================================

# BOTAN SHARED LIBRARY =========================================================================================
inc_botan_wrapper {
    unix:amd64 {
        LIBS += -L$$PWD/../zd_shared_lib/src/lib/botan/linux64/lib/ -lbotan-2
    }
    unix:arm64 {
        LIBS += -L$$PWD/../zd_shared_lib/src/lib/botan/arm64/lib/ -lbotan-2
    }
    unix:arm32 {
        LIBS += -L$$PWD/../zd_shared_lib/src/lib/botan/arm32/lib/ -lbotan-2
    }
    win32 {
        win_64bit {
            LIBS += -L$$PWD/../zd_shared_lib/src/lib/botan/win64/lib/ -lbotan
        }
        else {
            LIBS += -L$$PWD/../zd_shared_lib/src/lib/botan/win32/lib/ -lbotan
        }
    }
}
# BOTAN SHARED LIBRARY END =====================================================================================

INCLUDEPATH += ./src

SOURCES += \
    src/fn_scheduler.cpp \
    src/main.cpp \
    src/cmd_runner.cpp \
    src/app_settings.cpp \
    src/settings/downloader_settings.cpp \
    src/settings/general_settings.cpp \
    src/settings/links_checker_settings.cpp \
    src/settings/net_proxy_settings.cpp \
    src/settings/output_messages_settings.cpp \
    src/settings/scheduler_settings.cpp \
    src/settings/unpack_settings.cpp \
    src/settings/unpacklog_settings.cpp \
    src/unpack.cpp

HEADERS += \
    src/cmd_runner.h \
    src/app_settings.h \
    src/fn_scheduler.h \
    src/settings/downloader_settings.h \
    src/settings/general_settings.h \
    src/settings/links_checker_settings.h \
    src/settings/net_proxy_settings.h \
    src/settings/output_messages_settings.h \
    src/settings/scheduler_settings.h \
    src/settings/unpack_settings.h \
    src/settings/unpacklog_settings.h \
    src/unpack.h

OTHER_FILES += data/zdownloader.ini

unix {
SOURCES += \
    src/unix_sigwatch.cpp

HEADERS += \
    src/unix_sigwatch.h
}

win32 {
SOURCES += \
    src/win_sigwatch.cpp

HEADERS += \
    src/win_sigwatch.h
}
