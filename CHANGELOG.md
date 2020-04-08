# Changelog
### Changes in v1.9 (2020-04-08):
- added: bypass gdrive download limit by using google drive api (see [readme](https://github.com/sin2000/zdownloader/blob/master/README.md) and [service account howto](https://github.com/sin2000/zdownloader/blob/master/HOWTO_gdrive_service_account.md))
- fixed: the program will crash when user stops it(Ctrl-C/SIGINT) immediately after starting it(after 'Loading state')

### Changes in v1.8 (2020-03-23):
- changed: zippyshare link extractor algorithm
- changed: skipping links in link checker when link is invalid(for example: is already on download list)
- added: micro scheduler - see zdownloader.ini file
- fixed: in very rarely cases download file can be corrupted when server returns other content than file bytes

### Changes in v1.7 (2020-02-17):
- changed: zippyshare link extractor algorithm

### Changes in v1.6 (2020-02-07):
- added: network proxy support (socks5, http)

### Changes in v1.5 (2020-02-07):
- fixed: sometimes crash after download (regression from version v1.2 - use pointer after free). This is second one since v1.2(sic!) but should be last one

### Changes in v1.4 (2020-02-06):
- fixed: sometimes crash after download (regression from version v1.2 - use pointer after free)
- change: do not shedule system shutdown in certain situation when user/system stops application

### Changes in v1.3 (2020-02-04):
- changed: zippyshare link extractor algorithm

### Changes in v1.2 (2020-02-02):
- added: unpack rar, 7z and zip files after download with multi-password support
- added finished_downloads_\*.txt files that contains only finished or skipped downloads
- download_list.txt now have only pending downloads items and group_id in last column
- added ETA(estimated time of download end) time to every progress message
- skipping dead links and download files that does not exists on remote anymore or files that already exists locally
- better output messages

### Changes in v1.1 (2019-12-26):
- fixed null pointer access when remote file does not exists
- fixed 100% CPU usage after more than 100 links checks (workaround for cleanConnections every 2mins by QNetworkAccessManager which causes CPU spikes)
- added ability to connect more than 6 times to one host (workaround for QNetworkAccessManager 6 connections limit)
- added segmented downloading
- added more settings to zdownloader.ini file
- now you can put line comments in links.txt file - just write hash(#) character before comment
- better output messages
- added ability to download small files by google drive
- now google drive links like https://drive.google.com/open?id=7B3Uon3LO_we3zF8NVaIEGNJeNAR are recognized as file downloads
- updated precompiled Qt library to 5.14.0
