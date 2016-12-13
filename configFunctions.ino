// Функция удаления конфигурации контроллера
bool clearConfig(){
    Debugln("clearConfig");
    configToClear = 0;
    return SPIFFS.remove("/config.json");  
}

// Функция загрузки конфигурации контроллера
bool loadConfig(){
  Debugln("loadConfig()");
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Debugln("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Debugln("Config file size is too large");
    return false;
  }

  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Debugln("Failed to parse config file");
    return false;
  }

  // Конфигурация контроллера
  String enameC = json["ename"];
  String esidC = json["esid"];
  String epassC = json["epass"];
  String pubTopicC = json["pubTopic"];
  String mqttServerC = json["mqttServer"];
  String otaEnabledC = json["otaEnabled"];
  String mqttEnabledC = json["mqttEnabled"];
  
  // Конфигурация пинов
  String pin0typeC = json["p0type"];
  String pin0actionC = json["p0action"];
  String pin0scriptC = json["p0script"];
  String pin0inmodeC = json["p0inmode"];
  String pin0outdefC = json["p0outdef"];
  String pin0outmodeC = json["p0outmode"];
  String pin2typeC = json["p2type"];
  String pin2actionC = json["p2action"];
  String pin2scriptC = json["p2script"];
  String pin2inmodeC = json["p2inmode"];
  String pin2outdefC = json["p2outdef"];
  String pin2outmodeC = json["p2outmode"];
  String pin4typeC = json["p4type"];
  String pin4actionC = json["p4action"];
  String pin4scriptC = json["p4script"];
  String pin4inmodeC = json["p4inmode"];
  String pin4outdefC = json["p4outdef"];
  String pin4outmodeC = json["p4outmode"]; 
  String pin5typeC = json["p5type"];
  String pin5actionC = json["p5action"];
  String pin5scriptC = json["p5script"];
  String pin5inmodeC = json["p5inmode"];
  String pin5outdefC = json["p5outdef"];
  String pin5outmodeC = json["p5outmode"];
  String pin12typeC = json["p12type"];
  String pin12actionC = json["p12action"];
  String pin12scriptC = json["p12script"];
  String pin12inmodeC = json["p12inmode"];
  String pin12outdefC = json["p12outdef"];
  String pin12outmodeC = json["p12outmode"];
  String pin13typeC = json["p13type"];
  String pin13actionC = json["p13action"];
  String pin13scriptC = json["p13script"];
  String pin13inmodeC = json["p13inmode"];
  String pin13outdefC = json["p13outdef"];
  String pin13outmodeC = json["p13outmode"];
  String pin14typeC = json["p14type"];
  String pin14actionC = json["p14action"];
  String pin14scriptC = json["p14script"];
  String pin14inmodeC = json["p14inmode"];
  String pin14outdefC = json["p14outdef"];
  String pin14outmodeC = json["p14outmode"];
  String pin15typeC = json["p15type"];
  String pin15actionC = json["p15action"];
  String pin15scriptC = json["p15script"];
  String pin15inmodeC = json["p15inmode"];
  String pin15outdefC = json["p15outdef"];
  String pin15outmodeC = json["p15outmode"];

  Opt.ename = enameC;
  Opt.esid = esidC;
  Opt.epass = epassC;
  Opt.pubTopic = pubTopicC;
  Opt.mqttServer = mqttServerC;
  if (otaEnabledC == "true") Opt.otaEnabled = true; else Opt.otaEnabled = false;
  if (mqttEnabledC == "true") Opt.mqttEnabled = true; else Opt.mqttEnabled = false;
  Debug("ename: ");
  Debugln(Opt.ename);
  Debug("esid: ");
  Debugln(Opt.esid);
  Debug("epass: ");
  Debugln(Opt.epass);
  Debug("pubTopic: ");
  Debugln(Opt.pubTopic);
  Debug("mqttServer: ");
  Debugln(Opt.mqttServer);
  Debug("otaEnabled: ");
  Debugln(Opt.otaEnabled);
  Debug("mqttEnabled: ");
  Debugln(Opt.mqttEnabled);
  Pin[0].type = pin0typeC.toInt();
  Pin[0].action = pin0actionC;
  Pin[0].script = pin0scriptC;
  Pin[0].in_mode = pin0inmodeC.toInt();
  Pin[0].out_def = pin0outdefC.toInt();
  Pin[0].out_mode = pin0outmodeC.toInt();
  Pin[2].type = pin2typeC.toInt();
  Pin[2].action = pin2actionC;
  Pin[2].script = pin2scriptC;
  Pin[2].in_mode = pin2inmodeC.toInt();
  Pin[2].out_def = pin2outdefC.toInt();
  Pin[2].out_mode = pin2outmodeC.toInt();
  Pin[4].type = pin4typeC.toInt();
  Pin[4].action = pin4actionC;
  Pin[4].script = pin4scriptC;
  Pin[4].in_mode = pin4inmodeC.toInt();
  Pin[4].out_def = pin4outdefC.toInt();
  Pin[4].out_mode = pin4outmodeC.toInt();
  Pin[5].type = pin5typeC.toInt();
  Pin[5].action = pin5actionC;
  Pin[5].script = pin5scriptC;
  Pin[5].in_mode = pin5inmodeC.toInt();
  Pin[5].out_def = pin5outdefC.toInt();
  Pin[5].out_mode = pin5outmodeC.toInt();
  Pin[12].type = pin12typeC.toInt();
  Pin[12].action = pin12actionC;
  Pin[12].script = pin12scriptC;
  Pin[12].in_mode = pin12inmodeC.toInt();
  Pin[12].out_def = pin12outdefC.toInt();
  Pin[12].out_mode = pin12outmodeC.toInt();
  Pin[13].type = pin13typeC.toInt();
  Pin[13].action = pin13actionC;
  Pin[13].script = pin13scriptC;
  Pin[13].in_mode = pin13inmodeC.toInt();
  Pin[13].out_def = pin13outdefC.toInt();
  Pin[13].out_mode = pin13outmodeC.toInt();
  Pin[14].type = pin14typeC.toInt();
  Pin[14].action = pin14actionC;
  Pin[14].script = pin14scriptC;
  Pin[14].in_mode = pin14inmodeC.toInt();
  Pin[14].out_def = pin14outdefC.toInt();
  Pin[14].out_mode = pin14outmodeC.toInt();
  Pin[15].type = pin15typeC.toInt();
  Pin[15].action = pin15actionC;
  Pin[15].script = pin15scriptC;
  Pin[15].in_mode = pin15inmodeC.toInt();
  Pin[15].out_def = pin15outdefC.toInt();
  Pin[15].out_mode = pin15outmodeC.toInt();
  return true;
}

// Функция сохранение конфигурации контроллера
bool saveConfig() {
  Debugln("saveConfig()");
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  
  // Конфигурация контроллера
  json["ename"] = Opt.ename;
  json["esid"] = Opt.esid;
  json["epass"] = Opt.epass;
  json["pubTopic"] = Opt.pubTopic;
  json["mqttServer"] = Opt.mqttServer;
  json["otaEnabled"] = Opt.otaEnabled;
  json["mqttEnabled"] = Opt.mqttEnabled;
  
  // Конфигурация пинов
  json["p0type"] = Pin[0].type;
  json["p0action"] = Pin[0].action;
  json["p0script"] = Pin[0].script;
  json["p0inmode"] = Pin[0].in_mode;
  json["p0outdef"] = Pin[0].out_def;
  json["p0outmode"] = Pin[0].out_mode;
  json["p2type"] = Pin[2].type;
  json["p2action"] = Pin[2].action;
  json["p2script"] = Pin[2].script;
  json["p2inmode"] = Pin[2].in_mode;
  json["p2outdef"] = Pin[2].out_def;
  json["p2outmode"] = Pin[2].out_mode;
  json["p4type"] = Pin[4].type;
  json["p4action"] = Pin[4].action;
  json["p4script"] = Pin[4].script;
  json["p4inmode"] = Pin[4].in_mode;
  json["p4outdef"] = Pin[4].out_def;
  json["p4outmode"] = Pin[4].out_mode;
  json["p5type"] = Pin[5].type;
  json["p5action"] = Pin[5].action;
  json["p5script"] = Pin[5].script;
  json["p5inmode"] = Pin[5].in_mode;
  json["p5outdef"] = Pin[5].out_def;
  json["p5outmode"] = Pin[5].out_mode;
  json["p12type"] = Pin[12].type;
  json["p12action"] = Pin[12].action;
  json["p12script"] = Pin[12].script;
  json["p12inmode"] = Pin[12].in_mode;
  json["p12outdef"] = Pin[12].out_def;
  json["p12outmode"] = Pin[12].out_mode;
  json["p13type"] = Pin[13].type;
  json["p13action"] = Pin[13].action;
  json["p13script"] = Pin[13].script;
  json["p13inmode"] = Pin[13].in_mode;
  json["p13outdef"] = Pin[13].out_def;
  json["p13outmode"] = Pin[13].out_mode;
  json["p14type"] = Pin[14].type;
  json["p14action"] = Pin[14].action;
  json["p14script"] = Pin[14].script;
  json["p14inmode"] = Pin[14].in_mode;
  json["p14outdef"] = Pin[14].out_def;
  json["p14outmode"] = Pin[14].out_mode;
  json["p15type"] = Pin[15].type;
  json["p15action"] = Pin[15].action;
  json["p15script"] = Pin[15].script;
  json["p15inmode"] = Pin[15].in_mode;
  json["p15outdef"] = Pin[15].out_def;
  json["p15outmode"] = Pin[15].out_mode;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Debugln("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  configFile.close();
  return true;
}
