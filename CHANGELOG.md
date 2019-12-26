# Changelog
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
