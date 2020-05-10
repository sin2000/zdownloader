# Configuration directives:
# UNIT_TEST - indicates unit test mode

include(../zd_shared_lib/zd_shared.pri)

DEFINES += ZDOWNLOADER_NET_LIB

QT += network script
QT -= gui

CONFIG(debug, debug|release) {
    TARGET = zdownloader_net_libd
}
else {
    TARGET = zdownloader_net_lib
}

contains(QT, gui) {
    message("GUI ENABLED");
    DEFINES += ZD_GUI_ENABLED
}
else {
    message("GUI DISABLED");
}

TEMPLATE = lib

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

INCLUDEPATH += ./src

SOURCES += \
    src/empty_cookie_jar.cpp \
    src/http/del_request.cpp \
    src/http/dl_item_downloader.cpp \
    src/http/download_item_updater.cpp \
    src/http/file_downloader2.cpp \
    src/http/gdrive/gdrive_api.cpp \
    src/http/gdrive/gdrive_file.cpp \
    src/http/gdrive/gdrive_service.cpp \
    src/http/get_request.cpp \
    src/http/head_downloader.cpp \
    src/http/megaup/megaup_link_extractor.cpp \
    src/http/megaup/megaup_service.cpp \
    src/http/multi_link_checker.cpp \
    src/http/options_downloader.cpp \
    src/http/post_request.cpp \
    src/http/pre_post_job.cpp \
    src/http/segment_calculator.cpp \
    src/http/segmented_file_downloader.cpp \
    src/http/service.cpp \
    src/http/zippyshare/zippy_link_extractor.cpp \
    src/http/zippyshare/zippy_service.cpp \
    src/model/links_queue.cpp \
    src/model/segment_metadata.cpp \
    src/settings/finished_downloads_settings.cpp \
    src/settings/gdrive_settings.cpp \
    src/utils/archive_filename_comparator.cpp \
    src/utils/jwt.cpp \
    src/zdownloader_net_lib.cpp \
    src/http/web_config.cpp \
    src/net_thread.cpp \
    src/http/abstract_get_request.cpp \
    src/http/abstract_http_request.cpp \
    src/http/abstract_post_request.cpp \
    src/logged_nam.cpp \
    src/http/html_downloader.cpp \
    src/http/download_manager.cpp \
    src/model/download_item.cpp \
    src/model/downloadable_items.cpp \
    src/http/link_manager.cpp

HEADERS += \
    src/empty_cookie_jar.h \
    src/http/del_request.h \
    src/http/dl_item_downloader.h \
    src/http/download_item_updater.h \
    src/http/file_downloader2.h \
    src/http/gdrive/gdrive_api.h \
    src/http/gdrive/gdrive_file.h \
    src/http/gdrive/gdrive_service.h \
    src/http/get_request.h \
    src/http/head_downloader.h \
    src/http/megaup/megaup_link_extractor.h \
    src/http/megaup/megaup_service.h \
    src/http/multi_link_checker.h \
    src/http/options_downloader.h \
    src/http/post_request.h \
    src/http/pre_post_job.h \
    src/http/segment_calculator.h \
    src/http/segmented_file_downloader.h \
    src/http/service.h \
    src/http/zippyshare/zippy_link_extractor.h \
    src/http/zippyshare/zippy_service.h \
    src/model/links_queue.h \
    src/model/segment_metadata.h \
    src/settings/finished_downloads_settings.h \
    src/settings/gdrive_settings.h \
    src/utils/archive_filename_comparator.h \
    src/utils/jwt.h \
    src/zdownloader_net_lib.h \
    src/zdownloader_net_lib_global.h \
    src/http/web_config.h \
    src/net_thread.h \
    src/http/abstract_get_request.h \
    src/http/abstract_http_request.h \
    src/http/abstract_post_request.h \
    src/logged_nam.h \
    src/http/html_downloader.h \
    src/http/download_manager.h \
    src/model/download_item.h \
    src/model/downloadable_items.h \
    src/http/link_manager.h

unit_test {
  message("UnitTest MODE ENABLED")

  QT += testlib
  TEMPLATE = app

  DEFINES += UNIT_TEST
  HEADERS += \
    #src/unit_tests/*.h \
    src/unit_tests/http/*.h \
    src/unit_tests/model/*.h \
    src/unit_tests/utils/*.h \
    src/unit_tests/http/gdrive/*.h

  SOURCES += src/unit_tests/*.cpp

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
    macx {
        LIBS += -L$$PWD/../zd_shared_lib/src/lib/botan/macx/lib/ -lbotan-2
    }
}
# BOTAN SHARED LIBRARY END =====================================================================================
}
else {
    zd_static_linkage {
        CONFIG += staticlib
    }
}

#message($$DEFINES)
message($$QT)
