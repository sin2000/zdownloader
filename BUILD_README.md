## How to build zdownloader
### Requirements
- Qt 5.14
- Botan >= 2.14
- On Windows: MSVC 2017
- On Linux: g++ (at least with c++11 support)

## Preparations
- Compile botan library like:  
`python configure.py --minimized-build --enable-modules=system_rng,rsa,aes_ni,emsa_pkcs1 && make`
- Copy botan includes and library to `zd_shared_lib/src/lib/botan/`

## Build
- Go to src/ directory
- run:  
`qmake zd.pro && make`
