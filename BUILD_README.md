## How to build zdownloader
### Requirements
- Qt 5.14 (probably some versions below 5.14 will work to)
- Botan >= 2.14
- On Windows: MSVC 2017
- On Linux: g++ (at least with c++11 support)

## Preparations
- the most important build configuration is in the file [src/zd_shared_lib/zd_shared.pri](src/zd_shared_lib/zd_shared.pri)
- Compile botan library like:  
`python configure.py --minimized-build --enable-modules=system_rng,rsa,aes_ni,emsa_pkcs1 && make`
- Copy botan includes and library to `zd_shared_lib/src/lib/botan/`

## Building Qt
Qt for Linux x64 can be configured for build with minimal dependencies like this:
```
./configure \
-prefix /home/user/qt5_build/qt5_x64 \
-extprefix /home/user/qt5_build/qt5_x64 \
-release \
-opensource \
-ssl \
-openssl-runtime \
-no-eglfs \
-no-pch \
-confirm-license \
-strip \
-no-sqlite \
-no-gui \
-no-widgets \
-no-cups \
-no-fontconfig \
-no-gtk \
-no-opengles3 \
-no-xcb-xlib \
-nomake tests \
-nomake examples \
-no-compile-examples \
-no-icu \
-no-securetransport \
-no-opengl \
-qt-pcre \
-no-freetype \
-no-harfbuzz \
-no-dbus \
-qt-libpng \
-qt-libjpeg \
-skip qt3d \
-skip qtactiveqt \
-skip qtandroidextras \
-skip qtcanvas3d \
-skip qtcharts \
-skip qtconnectivity \
-skip qtdatavis3d \
-skip qtdeclarative \
-skip qtdoc \
-skip qtgamepad \
-skip qtgraphicaleffects \
-skip qtimageformats \
-skip qtlocation \
-skip qtmacextras \
-skip qtmultimedia \
-skip qtnetworkauth \
-skip qtpurchasing \
-skip qtquickcontrols \
-skip qtquickcontrols2 \
-skip qtremoteobjects \
-skip qtscxml \
-skip qtsensors \
-skip qtserialbus \
-skip qtspeech \
-skip qtsvg \
-skip qttools \
-skip qtvirtualkeyboard \
-skip qtwayland \
-skip qtwebchannel \
-skip qtwebengine \
-skip qtwebglplugin \
-skip qtwebview \
-skip qtx11extras \
-no-feature-appstore-compliant \
-no-feature-big_codecs \
-no-feature-cups \
-no-feature-freetype \
-no-feature-iconv \
-no-feature-imageformat_ppm \
-no-feature-imageformat_xbm \
-no-feature-bearermanagement \
-I/home/user/Qt/Tools/OpenSSL/binary/include
```

## Build
- Go to src/ directory
- run:  
`qmake zd.pro && make`

## Build for very old 32-bit CPUs without SSE2 instructions
- By default Qt requires CPU with SSE2 instructions. If you want run zdownloader on very old 32-bit CPU without SSE2 then you should rebuild Qt with `-no-sse2` switch.
- You also need to recompile botan library like this  
`python configure.py --minimized-build --disable-sse2 --enable-modules=system_rng,rsa,aes,emsa_pkcs1`  
 Probably you should use more `--disable...` switches from: https://botan.randombit.net/handbook/building.html
