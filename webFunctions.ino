// Функция вывода страницы настройки порта контроллера
void webHandleConfigPort(AsyncWebServerRequest *request){
	Debugln("webHandleConfigPort()");
	String s = "";
	int port;
	String type0opt="",type1opt="",type2opt="",type3opt="";
	String inmode0opt="",inmode1opt="",inmode2opt="";
	String outdef0opt="",outdef1opt="";
	String outmode0opt="",outmode1opt="";
	
	// если не задан GET-параметр с номером порта - редирект на главную
	if(request->hasParam("p")) port = request->getParam("p")->value().toInt(); else request->redirect("/");
	
	// открываем файл шаблона страницы
	File file = SPIFFS.open("/cfgport.html", "r");
	while (file.available()) { s += file.readStringUntil('\n'); }
	file.close();
	
	// заменяем переменные в шаблоне
	s.replace("%port%", String(port));
	s.replace("%port_stat%", String(digitalRead(port)));
	switch (Pin[port].type) {
		case PT_NC: type0opt = "selected"; break;
		case PT_IN: type1opt = "selected"; break;
		case PT_OUT: type2opt = "selected"; break;
		case PT_DSEN: type3opt = "selected"; break;
	}
	s.replace("%type0opt%", type0opt);
	s.replace("%type1opt%", type1opt);
	s.replace("%type2opt%", type2opt);
	s.replace("%type3opt%", type3opt);
	s.replace("%action%", Pin[port].action);
	s.replace("%script%", Pin[port].script);
	switch (Pin[port].in_mode) {
		case PIM_P: inmode0opt = "selected"; break;
		case PIM_R: inmode1opt = "selected"; break;
		case PIM_PR: inmode2opt = "selected"; break;
	}
	s.replace("%inmode0opt%", inmode0opt);
	s.replace("%inmode1opt%", inmode1opt);
	s.replace("%inmode2opt%", inmode2opt);
	if (Pin[port].out_mode == POM_SW) {
		switch (Pin[port].out_def) {
			case 0: outdef0opt = "selected"; break;
			case 1: outdef1opt = "selected"; break; 
		}
	}  
	s.replace("%outdef0opt%", outdef0opt);
	s.replace("%outdef1opt%", outdef1opt);
	if (Pin[port].out_mode == POM_PWM) s.replace("%out_def_pwm%", String(Pin[port].out_def)); else s.replace("%out_def_pwm%", "0");
	switch (Pin[port].out_mode) {
		case POM_SW: outmode0opt = "selected"; break;
		case POM_PWM: outmode1opt = "selected"; break; 
	}
	s.replace("%outmode0opt%", outmode0opt);
	s.replace("%outmode1opt%", outmode1opt);
	s += "\r\n\r\n";
	
	// отправляем HTML клиенту
	request->send(200, "text/html", s);
}

// Функция вывода страницы настроек контроллера
void webHandleConfig(AsyncWebServerRequest *request){
	Debugln("webHandleConfig()");
	String s = "";
	
	// открываем файл шаблона страницы
	File file = SPIFFS.open("/cfg.html", "r");
	while (file.available()) { s += file.readStringUntil('\n'); }
	file.close();
	
	// заменяем переменные в шаблоне
	s.replace("%name%", Opt.ename);
	s.replace("%ssid%", Opt.esid);
	s.replace("%pass%", Opt.epass);
	s.replace("%host%", Opt.mqttServer);
	s.replace("%pubtop%", Opt.pubTopic);
	if (Opt.otaEnabled) s.replace("%ota%", "checked"); else s.replace("%ota%", "");
	if (Opt.mqttEnabled) s.replace("%usemqtt%", "checked"); else s.replace("%usemqtt%", "");
	s += "\r\n\r\n";
	
	// отправляем HTML клиенту
	request->send(200, "text/html", s);
}

// Функция обработки формы настроек контроллера
void webHandleConfigSave(AsyncWebServerRequest *request){
	Debugln("webHandleConfigSave()");
	String s;
	s = "<p>Settings saved to config file and reset to boot into new settings</p>\r\n\r\n";
	request->send(200, "text/html", s); 
	
	// очистка конфигурации
	clearConfig();

	// Имя контроллера
	String qname; 
	qname = request->getParam("name")->value();
	qname.replace("%2F","/");
	Debugln(qname);
	Debugln("");
	Opt.ename = (char*) qname.c_str();
	
	// SSID WiFi-сети
	String qsid; 
	qsid = request->getParam("ssid")->value();
	qsid.replace("%2F","/");
	Debugln(qsid);
	Debugln("");
	Opt.esid = (char*) qsid.c_str();

	// Пароль WiFi-сети
	String qpass;
	qpass = request->getParam("pass")->value();
	qpass.replace("%2F","/");
	Debugln(qpass);
	Debugln("");
	Opt.epass = (char*) qpass.c_str();

	// MQTT топик 
	String qpubTop;
	qpubTop = request->getParam("pubtop")->value();
	qpubTop.replace("%2F","/");
	Debugln(qpubTop);
	Debugln("");
	Opt.pubTopic = (char*) qpubTop.c_str();

	// адрес MQTT-сервера
	String qmqttip;
	qmqttip = request->getParam("host")->value();
	Debugln(qmqttip);
	Debugln("");
	Opt.mqttServer = (char*) qmqttip.c_str();

	// использовать OTA обновление?
	String qotaUpdate;
	if(request->hasParam("ota")) qotaUpdate = request->getParam("ota")->value(); else qotaUpdate = "off";
	Debugln(qotaUpdate);
	Debugln("");
	if (String(qotaUpdate) == String("on")) Opt.otaEnabled = 1; else Opt.otaEnabled = 0;

	// использовать MQTT?
	String qmqttEnabled;
	if(request->hasParam("usemqtt")) qmqttEnabled = request->getParam("usemqtt")->value(); else qmqttEnabled = "off";
	Debugln(qmqttEnabled);
	Debugln("");
	if (String(qmqttEnabled) == String("on")) Opt.mqttEnabled = 1; else Opt.mqttEnabled = 0;
	
	Debug("Settings written ");
	saveConfig()? Debugln("sucessfully.") : Debugln("not succesfully!");
	Debugln("Restarting!"); 
	delay(1000);
	
	// перезагрузка контроллера
	ESP.reset();
}

// Функция обработки формы настроеки порта контроллера
void webHandleConfigSavePort(AsyncWebServerRequest *request){
	Debugln("webHandleConfigSavePort()");

	// Порт
	int qport; 
	qport = request->getParam("port")->value().toInt();
	Debugln(qport);
	Debugln("");

	// Тип порта
	int qtype; 
	qtype = request->getParam("type")->value().toInt();
	Debugln(qtype);
	Debugln("");
	Pin[qport].type = qtype;

	// Действие при срабатывании порта
	String qaction; 
	qaction = request->getParam("action")->value();
	qaction.replace("%3A",":");
	qaction.replace("%3B",";");
	Debugln(qaction);
	Debugln("");
	Pin[qport].action = qaction;

	// Скрипт на веб-сервере который вызывается, если порт сработал
	String qscript; 
	qscript = request->getParam("script")->value();
	/*qscript.replace("%3A",":");
	qscript.replace("%2F","/");
	qscript.replace("%3F","?");
	qscript.replace("%3D","=");
	qscript.replace("%26","&");*/
	Debugln(qscript);
	Debugln("");
	Pin[qport].script = qscript;

	// Режим входа порта
	int qinmode; 
	qinmode = request->getParam("in_mode")->value().toInt();
	Debugln(qinmode);
	Debugln("");
	Pin[qport].in_mode = qinmode;

	// Режим выхода порта
	int qoutmode; 
	qoutmode = request->getParam("out_mode")->value().toInt();
	Debugln(qoutmode);
	Debugln("");
	Pin[qport].out_mode = qoutmode;

	// Состояние выхода порта при включении
	int qoutdef; 
	qoutdef = request->getParam("out_def")->value().toInt();
	Debugln(qoutdef);
	Debugln("");

	// Состояние PWM-выхода порта при включении
	int qoutdefpwm; 
	qoutdefpwm = request->getParam("out_def_pwm")->value().toInt();
	Debugln(qoutdefpwm);
	Debugln("");

	if (qoutmode == POM_SW) Pin[qport].out_def = qoutdef; else Pin[qport].out_def = qoutdefpwm;
	
	// Сохраняем настройки порта
	Debug("Port settings written ");
	saveConfig()? Debugln("sucessfully.") : Debugln("not succesfully!");
	// Применяем настройки без перезагрузки
	module_init();
	// Редирект на главную
	request->redirect("/");
}

// Функция вывода главной страницы контроллера
void webHandleMain(AsyncWebServerRequest *request){
	Debugln("webHandleMain()");
	String s = "", worktime;
	uint8_t mac[6];
	WiFi.macAddress(mac);
	int wDays, wHours, wMinutes, wSeconds;
	int freeheap;
	unsigned long _wc;
	
	// вычисляем время работы
	_wc = workCounter;
	wDays = _wc / 86400;
	_wc -= 86400 * wDays;
	wHours = _wc / 3600;
	_wc -= 3600 * wHours;
	wMinutes = _wc / 60;
	wSeconds = _wc - 60 * wMinutes;
	
	// вычисляем свободное ОЗУ
	freeheap = float(ESP.getFreeHeap()) / 81920 * 100;
	worktime = String(wDays) + " д " + String(wHours) + ":" + String(wMinutes) + ":" + String(wSeconds);
	
	// открываем файл шаблона страницы
	File file = SPIFFS.open("/index.html", "r");
	while (file.available()) { s += file.readStringUntil('\n'); }
	file.close();
	
	// заменяем переменные в шаблоне
	s.replace("%name%", Opt.ename);
	s.replace("%mac%", macToStr(mac));
	if (WiFi.status() == WL_CONNECTED) {s.replace("%ip%", WiFi.localIP().toString());} else {s.replace("%ip%", WiFi.softAPIP().toString());}
	s.replace("%freemem%", String(ESP.getFreeHeap()));
	s.replace("%freememperc%", String(freeheap));
	s.replace("%worktime%", worktime);
	switch (Pin[0].type) {
		case PT_NC: s.replace("%pin0type%", "Не Подключен"); break;
		case PT_IN: s.replace("%pin0type%", "Вход"); break;
		case PT_OUT: s.replace("%pin0type%", "Выход"); break;
		case PT_DSEN: s.replace("%pin0type%", "Цифровой Датчик"); break;
	}
	switch (Pin[2].type) {
		case PT_NC: s.replace("%pin2type%", "Не Подключен"); break;
		case PT_IN: s.replace("%pin2type%", "Вход"); break;
		case PT_OUT: s.replace("%pin2type%", "Выход"); break;
		case PT_DSEN: s.replace("%pin2type%", "Цифровой Датчик"); break;
	}
	switch (Pin[4].type) {
		case PT_NC: s.replace("%pin4type%", "Не Подключен"); break;
		case PT_IN: s.replace("%pin4type%", "Вход"); break;
		case PT_OUT: s.replace("%pin4type%", "Выход"); break;
		case PT_DSEN: s.replace("%pin4type%", "Цифровой Датчик"); break;
	}
	switch (Pin[5].type) {
		case PT_NC: s.replace("%pin5type%", "Не Подключен"); break;
		case PT_IN: s.replace("%pin5type%", "Вход"); break;
		case PT_OUT: s.replace("%pin5type%", "Выход"); break;
		case PT_DSEN: s.replace("%pin5type%", "Цифровой Датчик"); break;
	}
	switch (Pin[12].type) {
		case PT_NC: s.replace("%pin12type%", "Не Подключен"); break;
		case PT_IN: s.replace("%pin12type%", "Вход"); break;
		case PT_OUT: s.replace("%pin12type%", "Выход"); break;
		case PT_DSEN: s.replace("%pin12type%", "Цифровой Датчик"); break;
	}
	switch (Pin[13].type) {
		case PT_NC: s.replace("%pin13type%", "Не Подключен"); break;
		case PT_IN: s.replace("%pin13type%", "Вход"); break;
		case PT_OUT: s.replace("%pin13type%", "Выход"); break;
		case PT_DSEN: s.replace("%pin13type%", "Цифровой Датчик"); break;
	}
	switch (Pin[14].type) {
		case PT_NC: s.replace("%pin14type%", "Не Подключен"); break;
		case PT_IN: s.replace("%pin14type%", "Вход"); break;
		case PT_OUT: s.replace("%pin14type%", "Выход"); break;
		case PT_DSEN: s.replace("%pin14type%", "Цифровой Датчик"); break;
	}
	switch (Pin[15].type) {
		case PT_NC: s.replace("%pin15type%", "Не Подключен"); break;
		case PT_IN: s.replace("%pin15type%", "Вход"); break;
		case PT_OUT: s.replace("%pin15type%", "Выход"); break;
		case PT_DSEN: s.replace("%pin15type%", "Цифровой Датчик"); break;
	}
	s += "\r\n\r\n";
	
	// отправляем HTML клиенту
	request->send(200, "text/html", s);
}

