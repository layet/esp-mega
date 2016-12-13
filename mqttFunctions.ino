// Функция соединения с MQTT-брокером
boolean connectMQTT(){
	if (mqttClient.connected()){
		// если уже подключены - выходим
		return true;
	}

	Debug("Connecting to MQTT server ");
	Debug(Opt.mqttServer);
	Debug(" as ");
	Debugln(Opt.ename);
  
	// Пытаемся соединиться с именем клиента из настроек (должно быть уникально)
	if (mqttClient.connect((char*)Opt.ename.c_str())) {
		Debugln("Connected to MQTT broker");
		// Подписываемся на входящие сообщения
		if(mqttClient.subscribe((char*)String(Opt.pubTopic+"/#").c_str())){
			Debugln("Subsribed to topic.");
			// Отправляем информацию о контроллере
			pubMQTT("/class", OTAUPDATECLASS);
			pubMQTT("/version", OTAUPDATEVERSION);
			pubMQTT("/rssi", (char*) String(WiFi.RSSI()).c_str());
		} else {
			// Подписка не удалась
			Debugln("NOT subsribed to topic!");      
		}
		return true;
	} else {
		// Соединение не установлено
		Debugln("MQTT connect failed! ");
		return false;
	}
}

// Функция отключения от MQTT-брокера
void disconnectMQTT(){
	mqttClient.disconnect();
}

// Функция обработки соединения с MQTT-брокером
void mqtt_handler(){
	// Если есть сообщение для публикации
	if (toPub==1){
		// отправляем его.
		Debugln("DEBUG: Publishing state via MQTT");
		if(pubMQTT((char*) topic_to_send.c_str(), (char*) msg_to_send.c_str())){
			// Если успешно - сбрасываем флаг публикации
			toPub=0; 
		}
	}
	// Цикл обработки соединения с брокером
	mqttClient.loop();
	delay(100); // пусть дела делаются в фоне :)
}

// Обработка входящего сообщения
void mqtt_arrived(char* subTopic, byte* payload, unsigned int length) {
	int i = 0;
	String topic;
	topic = String(subTopic);
	
	Debug("MQTT message arrived:  topic: " + topic);
	// делаем нуль-терминированную строку
	for(i=0; i<length; i++) {    
		buf[i] = payload[i];
	}
	buf[i] = '\0';
	String msgString = String(buf);
	Debugln(" message: " + msgString);
	// убираем лишнее из названия входящего топика
	topic.replace(Opt.pubTopic+"/", "");
	// если сообщение отправлено в топик /gpioN
	if (topic.indexOf("gpio")!=-1) {
		// определяем номер пина контроллера
		topic.replace("gpio","");
		int _pin = topic.toInt();
		// если пин настроен на выход как переключатель - включаем/выключаем его
		if ((Pin[_pin].type == PT_OUT) && (Pin[_pin].out_mode == POM_SW)) digitalWrite(_pin, msgString.toInt());
		// если пин настроен на выход как ШИМ - пишем в него значение ШИМ
		if ((Pin[_pin].type == PT_OUT) && (Pin[_pin].out_mode == POM_PWM)) analogWrite(_pin, msgString.toInt());
	} 
}

// Отправка исходящего сообщения
boolean pubMQTT(char* pubTopic, char* pubValue){
	// Проверяем подключен ли контроллер к брокеру
	if (!connectMQTT()){
		delay(100);
		// Пробуем еще раз
		if (!connectMQTT){                            
			Debugln("Could not connect MQTT.");
			Debugln("Publish state NOK");
			return false;
		}
	}
	// Если подключен
	if (mqttClient.connected()){      
		Debug("Publish topic: ");
		Debugln(pubTopic);
		Debug("Publish value: ");
		Debugln(pubValue);
		// отправляем сообщение
		if (mqttClient.publish((char*) String(Opt.pubTopic+pubTopic).c_str(), pubValue)) {
			Debugln("Publish state OK");        
			return true;
		} else {
			Debugln("Publish state NOK");        
			return false;
		}
	} else {
		Debugln("Publish state NOK");
		Debugln("No MQTT connection.");        
	}    
}
