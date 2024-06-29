extern const char* wifi_ssid; //mine was Moxee Hotspot83_2.4G
extern const char* wifi_password;
extern const char* storage_password; //to ensure someone doesn't store bogus data on your server. should match value in config.php
//data posted to remote server so we can keep a historical record
//url will be in the form: http://your-server.com:80/weather/data.php?data=
extern const char* url_get;
extern const char* host_get;
