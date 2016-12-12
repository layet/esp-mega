boolean connectMQTT(){
  if (mqttClient.connected()){
    return true;
  }
  
  Debug("Connecting to MQTT server ");
  Debug(Opt.mqttServer);
  Debug(" as ");
  Debugln(Opt.ename);
  if (mqttClient.connect((char*)Opt.ename.c_str())) {
    Debugln("Connected to MQTT broker");
    if(mqttClient.subscribe((char*)String(Opt.pubTopic+"/#").c_str())){
      Debugln("Subsribed to topic.");
      pubMQTT("/class", OTAUPDATECLASS);
      pubMQTT("/version", OTAUPDATEVERSION);
      pubMQTT("/rssi", (char*) String(WiFi.RSSI()).c_str());
    } else {
      Debugln("NOT subsribed to topic!");      
    }
    return true;
  } else {
    Debugln("MQTT connect failed! ");
    return false;
  }
}

void disconnectMQTT(){
  mqttClient.disconnect();
}

void mqtt_handler(){
  if (toPub==1){
    Debugln("DEBUG: Publishing state via MQTT");
    if(pubMQTT((char*) topic_to_send.c_str(), (char*) msg_to_send.c_str())){
     toPub=0; 
    }
  }
  mqttClient.loop();
  delay(100); //let things happen in background
}

void mqtt_arrived(char* subTopic, byte* payload, unsigned int length) { // handle messages arrived 
  int i = 0;
  String topic;
  topic = String(subTopic);
  Debug("MQTT message arrived:  topic: " + topic);
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {    
    buf[i] = payload[i];
  }
  buf[i] = '\0';
  String msgString = String(buf);
  Debugln(" message: " + msgString);
  topic.replace(Opt.pubTopic+"/", "");
  if (topic.indexOf("gpio")!=-1) {
    topic.replace("gpio","");
    int _pin = topic.toInt();
    if ((Pin[_pin].type == PT_OUT) && (Pin[_pin].out_mode == POM_SW)) digitalWrite(_pin, msgString.toInt());
    if ((Pin[_pin].type == PT_OUT) && (Pin[_pin].out_mode == POM_PWM)) analogWrite(_pin, msgString.toInt());
  } 
}

boolean pubMQTT(char* pubTopic, char* pubValue){ //Publish the pubValue   
  if (!connectMQTT()){
      delay(100);
      if (!connectMQTT){                            
        Debugln("Could not connect MQTT.");
        Debugln("Publish state NOK");
        return false;
      }
    }
    if (mqttClient.connected()){      
        Debug("Publish topic: ");
        Debugln(pubTopic);
        Debug("Publish value: ");
        Debugln(pubValue);
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
