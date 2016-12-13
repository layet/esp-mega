// Функция инициализации WiFi
void initWiFi(){
	Debugln("initWiFi()");
	Debug("CONNECTING TO '");
	Debug(Opt.esid);
	Debugln("'");
	// скажем ESP не писать во флеш настройки WiFi при кажом подключении
	WiFi.persistent(false);
	// проверить зачем это нужно!
	WiFi.softAPdisconnect(true);
	delay(100);
	WiFi.mode(WIFI_OFF);
	// включаем режим станции
	WiFi.mode(WIFI_STA);
	// пробуем подключиться к точке доступа из настроек
	WiFi.begin(Opt.esid.c_str(), Opt.epass.c_str());
	if ( testWifi() == 1 ) { 
		Debugln("IP: "+WiFi.localIP().toString());
		// успешно - запускаем веб сервер
		launchWeb();
		return;
	}
	// не вышло - запускаем свою точку доступа
	setupAP();   
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
