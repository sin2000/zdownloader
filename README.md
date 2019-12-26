## Changelog
- 2019-12-26 v1.0 -> v1.1 - [LINK](https://github.com/sin2000/zdownloader/blob/master/CHANGELOG.md)

# zdownloader
Zdownloader is a lightweight download manager running from command line.  
Zdownloader was written in C++ and supports downloading files by HTTPS from services such as:
- ZippyShare,
- Google Drive,
- MegaUp(megaup.net).

![screenshot](https://github.com/sin2000/zdownloader/blob/master/images/zd_running_pub.png)

## Features
- Can download multiple files at a time.
- Supports segmented downloading.
- It can resume interrupted downloads.
- Can check multiple links at a time.
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
- I have attached precompiled Qt libs in win7_8_10-x64 directory. License: GNU LGPL version 3, Qt source code: https://code.qt.io.
- I have attached precompiled OpenSSL libs in win7_8_10-x64 directory - https://www.openssl.org.

## Usage
**links.txt** file:
- create links.txt file in zdownloader directory,
- you can add URLs to links.txt file: one line -> one URL,
- at the beginning zdownloader checks and sorts the links,
- checked and sorted links are saved to download_list.txt file,
- you can add '--' chars to append links as one group,
- you can add encoded links,
- you can put line comments - just add hash(#) character before comment.

### Example of links.txt file:
```
# this is comment
https://www73.zippyshare.com/v/0deWxCol/file.html
https%3A%2F%2Fwww73.zippyshare.com%2Fv%2F0deWxCol%2Ffile.html
--
https://drive.google.com/file/d/2M4RhNINIETzt-a6tbkdwLTBS9CAqM-WD
--
https://megaup.net/3Pwaa/something.rar #comment: example link
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
; UTF-8 encoding

[Common]
; download_directory
; if directory contains spaces then enclose it in quotation marks
; set empty or '.' for current directory
download_directory=/media/downloads
user_agent_for_https_connections="Mozilla/5.0 (Windows NT 6.1; rv:60.0) Gecko/20100101 Firefox/60.0"
; shutdown_system_on_finish
; uses 'sudo shutdown --no-wall -P +2' command on Linux or 'shutdown /s /t 120' command on Windows
shutdown_system_on_finish=false
; max_seconds_before_reset_inactive_connection
; integer value
max_seconds_before_reset_inactive_connection=45
; download_lists_directory
; program will read file 'download_list.txt' from download_lists_directory
download_lists_directory=
; links_file_directory
; program will read file 'links.txt' from links_file_directory
links_file_directory=

[LinkChecker]
max_parallel_link_checks=5
; max_seconds_before_check_link_again_on_error
; integer value
max_seconds_before_check_link_again_on_error=10

[Downloader]
max_parallel_file_downloads=1
max_segments_per_file=2
; min_segment_size_mb
; integer value. Do not change min_segment_size_mb when you have unfinished downloads
min_segment_size_mb=4
; segment_network_read_buffer_kb
; integer value. Download read buffer(in RAM) size per one segment in KBytes.
; Appication will try to stop reading from the network once this buffer is full, thus causing the download to throttle down as well.
; This can happen when your system is not capable to download with full speed.
segment_network_read_buffer_kb=2048
; segment_file_write_buffer_kb
; integer value. File write buffer(in RAM) size for one segment in KBytes.
segment_file_write_buffer_kb=1024
; min_seconds_before_next_download, max_seconds_before_next_download, next_download_increase_step_msecs
; next_download_increase_reset_after_seconds
; integer values. Applies to per file download (not per segment)
min_seconds_before_next_download=0
max_seconds_before_next_download=7
next_download_increase_step_msecs=250
; next_download_increase_reset_after_seconds
; integer value. Resets next file download delay to 0 seconds after X seconds
next_download_increase_reset_after_seconds=8
; progress_check_interval_seconds
; integer value. Show 'Progress:...' message every X seconds
progress_check_interval_seconds=10
; speed_check_interval_seconds
; integer value. Show actual speed and average speed every X seconds
speed_check_interval_seconds=8
; user_reconnect_interval_minutes
; integer value. Reconnects all connections every X minutes if avg speed is less than min_avg_speed_before_reconnect_mbps
user_reconnect_interval_minutes=4
; min_avg_speed_before_reconnect_mbps
; integer value
min_avg_speed_before_reconnect_mbps=13

[OutputMessages]
; log_to_file_enabled
; enable or disable applog_*.log files. Applog contains all output messsages.
log_to_file_enabled=true
log_directory=
; timestamp_format
; format details: https://doc.qt.io/archives/qt-5.13/qdatetime.html#toString
timestamp_format="yyyyMMdd.hh:mm:ss"
; log_max_rotate_kb
; integer value in KBytes
log_max_rotate_kb=4096
log_max_archive_files=3
```

Zdownloader can power off system on download finish. For that zdownloader uses  
- for Linux: `sudo shutdown --no-wall -P +2` command which should execute without any passwords,
- for Windows: `shutdown /s /t 120` command.

#### applog_\*.log files:
Zdownloader can save everything from standard output to rotated log files:  
`applog_1.log ...`

## Download:
- for Linux x86-64: [DOWNLOAD](https://github.com/sin2000/zdownloader/releases/download/1.0/zdownloader_linux-x86-64_bin.zip)
- for Linux ARM gnueabihf (eg. Raspberry Pi >= 3): [DOWNLOAD](https://github.com/sin2000/zdownloader/releases/download/1.0/zdownloader_arm-linux-gnueabihf_bin.zip)
- for Linux ARM aarch64 (eg. Odroid C2): [DOWNLOAD](https://github.com/sin2000/zdownloader/releases/download/1.0/zdownloader_aarch64-linux-gnu_bin.zip)
- for Windows 7, 8, 10 x64: [DOWNLOAD](https://github.com/sin2000/zdownloader/releases/download/1.0/win7_8_10-x64.zip)
