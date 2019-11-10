# zdownloader
Zdownloader is a lightweight download manager running from command line.  
Zdownloader was written in C++ and supports downloading files by HTTPS from services such as:
- ZippyShare,
- Google Drive,
- MegaUp(megaup.net).

![screenshot](https://github.com/sin2000/zdownloader/blob/master/images/zd_running_pub.png)

## Features
- Can download multiple files at a time.
- It can resume interrupted downloads.
- Can check multiple links at a time (currently 5).
- It can reconnect after specific time if download speed is too low.

## Requirements (for x86-64)
`ldd zdownloader`:
- linux-vdso.so.1
- libQt5Network.so.5 
- libQt5Core.so.5
- libpthread.so.0
- libstdc++.so.6
- libgcc_s.so.1
- libc.so.6
- libz.so.1
- libdl.so.2
- libm.so.6
- /lib64/ld-linux-x86-64.so.2

#### General
- Linux kernel >= 3.2.0
- glibc(libc6) >= 2.25
- OpenSSL >= 1.1.0
- Qt5 >= 5.13.0  
I have attached precompiled Qt libs in lib directories (license: GNU LGPL version 3, Qt source code: https://code.qt.io).

#### Windows
- you probably need Visual C++ 2017 redistributables x64: https://aka.ms/vs/16/release/vc_redist.x64.exe
I have attached precompiled Qt libs in win7_8_10-x64 directory. License: GNU LGPL version 3, Qt source code: https://code.qt.io.
I have attached precompiled OpenSSL libs in win7_8_10-x64 directory - https://www.openssl.org.

## Usage
**links.txt** file:
- create links.txt file in zdownloader directory,
- you can add URLs to links.txt file: one line -> one URL,
- at the beginning zdownloader checks and sorts the links,
- checked and sorted links are saved to download_list.txt file,
- you can add '--' chars to append links as one group,
- you can add encoded links.

### Example of links.txt file:
```
https://www73.zippyshare.com/v/0deWxCol/file.html
https%3A%2F%2Fwww73.zippyshare.com%2Fv%2F0deWxCol%2Ffile.html
--
https://drive.google.com/file/d/2M4RhNINIETzt-a6tbkdwLTBS9CAqM-WD
--
https://megaup.net/3Pwaa/something.rar
```

### download_list.txt file:
- download_list.txt file is created after links checks,
- download list has format:
```
P file_name URL
```
Three fields separated by tab character.  
First field contains P or F character which is Pending or Finished.  
- You can edit(eg. change order) download_list.txt when zdownloader is not running,
- If remote file does not exists then zdownloader skips that file.

You can run zdownloader by  
`./start_zd.sh`  
from zdownloader directory.

To stop zdownloader press Ctrl-C or send SIGINT or SIGTERM signal.

#### Settings are stored in zdownloader.ini file:
```ini
download_directory=/media/downloads
max_parallel_connections=2
speed_check_interval_seconds=8
user_reconnect_interval_minutes=5
min_avg_speed_before_reconnect_mbps=12
user_agent_for_https_connections="Mozilla/5.0 (Windows NT 6.1; rv:60.0) Gecko/20100101 Firefox/60.0"
shutdown_system_on_finish=false
```

Zdownloader can power off system on download finish. For that zdownloader uses  
- for Linux: `sudo shutdown --no-wall -P +2` command which should execute without any passwords,
- for Windows: `shutdown /s /t 120` command.

#### applog_\*.log files:
Zdownloader saves everything from standard output to rotated log files:  
`applog_1.log ... applog_4.log.`  
Every applog_\*.log file can take about 4MB in size.

## Download:
- for Linux x86-64: [DOWNLOAD](https://github.com/sin2000/zdownloader/releases/download/1.0/zdownloader_linux-x86-64_bin.zip)
- for Linux ARM gnueabihf (eg. Raspberry Pi >= 3): [DOWNLOAD](https://github.com/sin2000/zdownloader/releases/download/1.0/zdownloader_arm-linux-gnueabihf_bin.zip)
- for Linux ARM aarch64 (eg. Odroid C2): [DOWNLOAD](https://github.com/sin2000/zdownloader/releases/download/1.0/zdownloader_aarch64-linux-gnu_bin.zip)
- fro Windows 7, 8, 10 x64: [DOWNLOAD](https://github.com/sin2000/zdownloader/releases/download/1.0/win7_8_10-x64.zip)
