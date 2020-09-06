message("zd_shared loaded")

#DEFINES += DEBUG_PROXY
DEFINES += BOTAN_NO_DEPRECATED_WARNINGS
DEFINES += BOTAN_RSA_SIGN_ONLY

CONFIG += inc_botan_wrapper
CONFIG += zd_static_linkage
CONFIG += win_64bit # comment to use 32bit windows libraries

DEFINES += WIN32_LEAN_AND_MEAN \
           QT_DEPRECATED_WARNINGS \
           QT_RESTRICTED_CAST_FROM_ASCII \
           QT_USE_FAST_CONCATENATION \
           QT_USE_FAST_OPERATOR_PLUS \
           QT_USE_QSTRINGBUILDER \
           QT_MESSAGELOGCONTEXT

CONFIG -= debug_and_release
CONFIG += c++14

inc_js_utils {
    QT += script
}

zd_static_linkage {
    DEFINES += ZD_STATIC_LINKAGE
}

CONFIG(debug, debug|release) {
    DEFINES += ZD_DEBUG

    DESTDIR = ./bin_debug
    OBJECTS_DIR = ./build/dbg
    UI_DIR = ./build/dbg
    MOC_DIR = ./build/dbg
    RCC_DIR = ./build/dbg

    win32:QMAKE_CXX = D:\CppUtils\clcache\clcache.exe
    unix {
        QMAKE_CC = ccache gcc
        QMAKE_CXX = ccache g++
    }
}
else {
    CONFIG += ltcg
    DEFINES += ZD_RELEASE

    DESTDIR = ./bin_release
    OBJECTS_DIR = ./build/release
    UI_DIR = ./build/release
    MOC_DIR = ./build/release
    RCC_DIR = ./build/release
}

win32 {
    DEFINES += SYSTEM_WINDOWS

    # Enable Level 4 compiler warnings
    QMAKE_CXXFLAGS_WARN_ON -= -W3
    QMAKE_CXXFLAGS_WARN_ON += -W4
    # Hush some known Qt warnings
    QMAKE_CXXFLAGS += -wd4127
    #-wd4512 -wd4189
    QMAKE_CXXFLAGS_RELEASE += -GL

    QMAKE_LFLAGS_RELEASE += /LTCG

    QMAKE_CXXFLAGS_DEBUG -= -Zi
    QMAKE_CXXFLAGS_DEBUG += -Z7
    QMAKE_LFLAGS_DEBUG += -incremental
}

unix {
    DEFINES += SYSTEM_UNIX    
    QMAKE_CXXFLAGS += -Wpedantic -Wshadow -ftrapv -Wall

    # release
    #QMAKE_CXXFLAGS_RELEASE += -finline-functions -fstack-protector-strong
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE += -O3 -finline-functions
    #QMAKE_CXXFLAGS_RELEASE += -fstack-protector -Wformat -Wformat-security -Werror=format-security -D_FORTIFY_SOURCE=2 -fPIE
    #QMAKE_LFLAGS_RELEASE += -Wl,-z,relro -Wl,-z,now
    #QMAKE_LFLAGS_RELEASE += -static-libgcc -static-libstdc++
}

PLATFORM = $$find(QMAKESPEC, "linux-odroid-c2-g\+\+")
count(PLATFORM, 1) {
      CONFIG += arm64
}
PLATFORM = $$find(QMAKESPEC, "linux-rasp-armhf-g\+\+")
count(PLATFORM, 1) {
      CONFIG += arm32
}
PLATFORM = $$find(QMAKESPEC, "linux-g\+\+")
count(PLATFORM, 1) {
      CONFIG += amd64
}
