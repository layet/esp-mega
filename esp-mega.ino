#define DEBUG
// включаем или отключаем режим отладки

#ifdef DEBUG
  #define Debug(x)    Serial.print(x)
  #define Debugln(x)  Serial.println(x)
  #define Debugf(...) Serial.printf(__VA_ARGS__)
  #define Debugflush  Serial.flush
#else
  #define Debug(x)    {}
  #define Debugln(x)  {}
  #define Debugf(...) {}
  #define Debugflush  {}
#endif

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#include <PubSubClient.h>
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "FS.h"
#include "stdio.h"

String ssid = "IoTController"; // SSID в режиме точки доступа

// Типы портов ввода-вывода
#define PT_NC   0 // не подключен
#define PT_IN   1 // вход
#define PT_OUT  2 // выход
#define PT_DSEN 3 // цифровой датчик
// Режимы входа
#define PIM_P  0 // замыкание
#define PIM_R  1 // размыкание
#define PIM_PR 2 // и замыкание, и размыкание
// Режимы выхода
#define POM_SW  0 // выключатель
#define POM_PWM  1 // ШИМ

#define OTAUPDATECLASS "WIFIMEGA" // OTA: Имя класса устройства
#define OTAUPDATEVERSION "0.1.1" // OTA: Версия прошивки устройства
#define OTAUPDATESERVER "ota.i-alice.ru" // OTA: Сервер обновления
#define OTAUPDATEPATH "/update/index.php" // OTA: Путь обновления на сервере

// Структура настроек контроллера
typedef struct
{
  String ename = "";
  String esid = "";
  String epass = "";
  String pubTopic = "";
  String mqttServer = "";
  bool otaEnabled = 0;
  bool mqttEnabled = 0;
} options;

// Структура настроек порта контроллера
typedef struct
{
  byte type = PT_NC;
  String action = "";
  String script = "";
  byte in_mode = PIM_P;
  byte in_value = 0;
  byte out_def = 0;
  byte out_mode = POM_SW;
  byte out_value = 0;
} pins;

//##### Объекты #####
AsyncWebServer server(80);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
HTTPClient http;
Ticker gpio_timer;
Ticker work_timer;

//##### Флаги ##### нужны, чтобы не выполнять длительные операции вне цикла loop
int rstNeed=0;   // флаг рестарта
int toPub=0; // флаг публикации MQTT
int toPubHttp=0; // флаг публикации HTTP
int configToClear=0; // флаг стирания конфигурации

//##### Глобальные переменные ##### 
unsigned long countReset = 0; // Счетчик длительности нажатия кнопки сброса
char buf[40]; // буфер для данных MQTT
unsigned long workCounter = 0;
String topic_to_send = "";
String msg_to_send = "";
String http_to_send = "";

// Переменные для чтения настроек из SPIFFS
options Opt;
// Переменные для чтения настроек портов ввода-вывода
pins Pin[16];
// Массив используемых пинов
byte used_pins[] = {0,2,4,5,12,13,14,15};

void setup() {
	#ifdef DEBUG
		Serial.begin(9600, SERIAL_8N1);
	#endif

	// Запускаем таймеры
	gpio_timer.attach(0.05, gpio_handle);
	work_timer.attach(1, work_handle);
	// wifi_reconnect_timer.attach(60, wifi_reconnect_handle);
	if (!SPIFFS.begin()) {
		Debugln("[setup()] FAILED TO MOUNT FILE SYSTEM!");
	}
	// загружаем настройки
	if (!loadConfig()) { Debugln("[setup()] loadConfig() FAILED!"); }
	// инициализация WiFi
	initWiFi();
	// если соединение прошло успешно - проверяем флаг OTA обновления
	if (Opt.otaEnabled) {
		Debugln("[setup()] OTA UPDATE ENABLED. CHECKING FOR UPDATES.");
		Debugln(OTAUPDATESERVER);
		Debugln(OTAUPDATEPATH);
		Debugln(OTAUPDATECLASS+String("-")+OTAUPDATEVERSION);
		t_httpUpdate_return ret = ESPhttpUpdate.update(OTAUPDATESERVER, 80, OTAUPDATEPATH, OTAUPDATECLASS+String("-")+OTAUPDATEVERSION);
		switch(ret) {
			case HTTP_UPDATE_FAILED:
				Debugf("[setup()] Update failed. Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
				Debugln("");
				break;
			case HTTP_UPDATE_NO_UPDATES:
				Debugln("[setup()] Update no Update.");
				break;
			case HTTP_UPDATE_OK:
				Debugln("[setup()] Update ok.");
				break;
		}
	}
	module_init(); 
}

// Первоначальная настройка модуля
void module_init()
{
	if (Opt.mqttEnabled) { 
		IPAddress ip;
		ip.fromString((char*) Opt.mqttServer.c_str());
		mqttClient.setServer(ip, 1883);
		mqttClient.setCallback(mqtt_arrived);
		connectMQTT();
	}
	for (int i=0;i<sizeof(used_pins);i++)
	{
		switch (Pin[used_pins[i]].type) {
			case PT_IN:
				pinMode(used_pins[i], INPUT_PULLUP);
				Pin[used_pins[i]].in_value = digitalRead(used_pins[i]);
				break;
			case PT_OUT:
				pinMode(used_pins[i], OUTPUT);
				if (Pin[used_pins[i]].out_mode == POM_PWM) analogWrite(used_pins[i], Pin[used_pins[i]].out_def);
					else digitalWrite(used_pins[i], Pin[used_pins[i]].out_def);
				break;      
		}
	}
}

// Проверка состояния пинов по таймеру
void gpio_handle()
{
	int pin, pin_dir;
	for (int i=0;i<sizeof(used_pins);i++)
	{
		if (Pin[used_pins[i]].type == PT_IN) {
			pin = digitalRead(used_pins[i]);
			pin_dir = Pin[used_pins[i]].in_value - pin;
			if ((Pin[used_pins[i]].in_mode == PIM_P) && (pin_dir == 1)) runAction(used_pins[i]);
			if ((Pin[used_pins[i]].in_mode == PIM_R) && (pin_dir == -1)) runAction(used_pins[i]);
			if ((Pin[used_pins[i]].in_mode == PIM_PR) && (pin_dir != 0)) runAction(used_pins[i]);
			Pin[used_pins[i]].in_value = pin;
		}
	} 
}

// Функция ежесекундного таймера
void work_handle()
{
	workCounter++;
}

// Кандидат на выбывание
/*void wifi_reconnect_handle()
{
	if (WiFi.status() != WL_CONNECTED){
		// Если соединение с сетью отсутствует
		delay(1000);
		initWiFi(); // Пробуем переподключиться
	}
}*/

// Обработчик необработанных запросов сервера
void onRequest(AsyncWebServerRequest *request){
	// Выдадим ошибку HTTP 404
	request->send(404);
}

// Функция обработки действия назначенного на порт
void runAction(byte _pin) {
	byte start = 0;
	String action = Pin[_pin].action;
	// Разбираем настроенное дествие для порта
	while (start < action.length()){
		int end = action.indexOf(';', start);
		if (end < 0) end = action.length();
		int equal = action.indexOf(':', start);
		if (equal < 0 || equal > end) equal = end;
		String act_pin = action.substring(start, equal);
		String act_val = equal + 1 < end ? action.substring(equal + 1, end) : String();
		Debugf("[runAction()] act_pin: [%s], act_val: [%s]\n", act_pin.c_str(), act_val.c_str());
		// Если порт действия настроен как выход - выполняем действие
		if (Pin[act_pin.toInt()].type == PT_OUT) {
			if (act_val == "2") digitalWrite(act_pin.toInt(), !digitalRead(act_pin.toInt()));
				else digitalWrite(act_pin.toInt(), act_val.toInt());
		}
		start = end + 1;
	}
	// Если включена отправка сообщений MQTT-брокеру
	if (Opt.mqttEnabled) {
		// выставляем флаг публикации MQTT
		topic_to_send = "/gpio"+String(_pin);
		msg_to_send = String(Pin[_pin].in_value);
		toPub=1;
	}
	// Если настроен адрес отправки сообщения по HTTP
	if (Pin[_pin].script != "") {
		// выставляем флаг отправки HTTP
		http_to_send = Pin[_pin].script;
		http_to_send.replace("%val%", String(Pin[_pin].in_value));
		toPubHttp=1;
	}
}

// Функция запуска веб-сервера
void launchWeb() {    
	// Настраиваем роутер веб-сервера
    Debugln("[launchWeb()] Launching web server");
	server.on("/", HTTP_GET, webHandleMain);
	server.serveStatic("/matmin.css", SPIFFS, "/matmin.css");
	server.on("/cfg", HTTP_GET, webHandleConfig);
	server.on("/cfgport", HTTP_GET, webHandleConfigPort);
	server.on("/cfgsave", HTTP_GET, webHandleConfigSave);
	server.on("/cfgsaveport", HTTP_GET, webHandleConfigSavePort);
	server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request){
		ESP.reset();
	});
	server.onNotFound(onRequest);
	// Запускаем вебсервер
	server.begin();       
}

// Преобразование MAC-адреса в строку
String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

// Функция запуска точки доступа
void setupAP(void) {
	Debugln("[setupAP()] Launching Access Point");
	WiFi.disconnect();
	delay(100);
	WiFi.mode(WIFI_OFF);
	WiFi.mode(WIFI_AP);
	
	// Запускаем точку доступа
	WiFi.softAP((char*) ssid.c_str());
	WiFi.begin((char*) ssid.c_str());
	
	// Запускаем веб-сервер
	launchWeb();
}

// Главный цикл
void loop() {
	// Если установлен флаг сброса настроек
	if(configToClear==1){
		// сбрасываем настройки и перезагружаемся
		clearConfig();
		delay(1000);
		ESP.reset();
	}
	// Если включена отправка сообщений MQTT-брокеру
	if (Opt.mqttEnabled) {
		// обрабатываем соединение с брокером
		mqtt_handler();
	}
	// Если установлен флаг отправки HTTP-сообщения
	if (toPubHttp==1){
		// отправляем
		Debug("[loop()] Publish state via HTTP ");
		http.begin(http_to_send);
		int httpCode = http.GET();
		if(httpCode > 0) Debugln("success!"); else Debugln("failed!");
		toPubHttp=0;
		http.end();
	}
}
