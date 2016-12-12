void initWiFi(){
  Debugln("initWiFi()");
  Debug("CONNECTING TO '");
  Debug(Opt.esid);
  Debugln("'");
  // скажем ESP не писать во флеш настройки WiFi при кажом подключении
  WiFi.persistent(false);
  WiFi.softAPdisconnect(true);
  delay(100);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(Opt.esid.c_str(), Opt.epass.c_str());
  if ( testWifi() == 1 ) { 
    Debugln("IP: "+WiFi.localIP().toString());
    launchWeb();
    return;
  }
  setupAP();   
}

int testWifi(void) {
  int c = 0;  
  Debugln("testWifi()");
  while ( c < 60 ) {
    if (WiFi.status() == WL_CONNECTED) { 
      Debugln("");
      return(1); 
    } 
    delay(500);
    Debug(".");    
    c++;
  }
  Debugln("");
  return(0);
} 

