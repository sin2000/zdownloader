## How to enable google drive api and create google service account

1. Log in to google account.
2. Go to https://console.developers.google.com  
2.1 Click "Select a project" and click "NEW PROJECT".  
![screenshot](https://github.com/sin2000/zdownloader/blob/master/images/google01.png)
2.2 Enter project name 'zdproj' and click "Create".  
![screenshot](https://github.com/sin2000/zdownloader/blob/master/images/google02.png)

3. Go to https://console.developers.google.com/apis/library?project=zdproj&supportedpurview=project  
3.1 Search for google drive and click "Google Drive API" from search results.  
![screenshot](https://github.com/sin2000/zdownloader/blob/master/images/google03.png)
3.2 Click "ENABLE".  
![screenshot](https://github.com/sin2000/zdownloader/blob/master/images/google04.png)

4. Go to https://console.developers.google.com/apis/api/drive.googleapis.com/overview?project=zdproj&supportedpurview=project  
4.1 Click "CREATE CREDENTIALS" button.  
![screenshot](https://github.com/sin2000/zdownloader/blob/master/images/google05.png)
4.2 On form select "Google Drive API", "Other non-UI (e.g. cron job, daemon)", "Application data", "No, I'm not using them". See image below.  
![screenshot](https://github.com/sin2000/zdownloader/blob/master/images/google06.png)
4.3 Click "What credentials do I need?" button.

5. On next page set "Service account name" to "zd" and choose "Editor" in "Role"(Project->Editor).
Make sure that "JSON" is selected as "Key type".  
![screenshot](https://github.com/sin2000/zdownloader/blob/master/images/google07.png)

6. Click "Continue" button. JSON file will be downloaded with service account credentials.
Save it near to zdownloader location and update google_service_account_json_file= in zdownloader.ini file.  
![screenshot](https://github.com/sin2000/zdownloader/blob/master/images/google08.png)
