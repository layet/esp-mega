// Функция инициализации WiFi
void initWiFi(){  
	Debugln("[initWiFi()] Initializing WiFi");
  // Если не задан SSID в настройках - запускаем точку доступа
  if (Opt.esid == "") {
    Debugln("[initWiFi()]  No SSID in settings. Starting AP.");
    setupAP();  
  }
	Debug("[initWiFi()]  CONNECTING TO '");
	Debug(Opt.esid);
	Debugln("'");
	// скажем ESP не писать во флеш настройки WiFi при кажом подключении
	WiFi.persistent(false);
	WiFi.disconnect(true);
  // добавляем обработчик событий WiFi
  WiFi.onEvent(WiFiEvent);
	delay(100);
	// включаем режим станции
	WiFi.mode(WIFI_STA);
	// пробуем подключиться к точке доступа из настроек
	WiFi.begin(Opt.esid.c_str(), Opt.epass.c_str());
	if ( testWifi() == 1 ) { 
		Debugln("[initWiFi()]  IP: "+WiFi.localIP().toString());
		// успешно - запускаем веб сервер
		launchWeb();
		return;
	}
}

// Функция проверки доступности точки доступа
int testWifi(void) {
	int c = 0;  
	Debugln("testWifi()");
	// 30 секунд на подключение
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

void WiFiEvent(WiFiEvent_t event) {
      switch(event) {
        case WIFI_EVENT_STAMODE_GOT_IP:
            Debugln("[WiFiEvent()] WiFi connected");
            Debug("[WiFiEvent()] IP address: ");
            Debugln(WiFi.localIP());
            break;
        case WIFI_EVENT_STAMODE_DISCONNECTED:
            Debugln("[WiFiEvent()] WiFi lost connection");
            break;
    }
}
