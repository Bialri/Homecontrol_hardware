#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

struct Cfg {
  char ssid[16];
  char pass[16];
  byte mode;
};

struct Usr {
  char token[60];
  byte mode;
};

Usr usr;

Cfg cfg;
const char* ssid = "NodeMCU";
const char* password = "12345678";
int isAP = 0;
byte counter = 0;
String host = "https://homecontrol.kardasov.ru/buttons/GetWithToken?token=";

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);


WiFiClientSecure client;
ESP8266WebServer server(80);

void setup() {
  pinMode(0,INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(LED_BUILTIN,0);

  EEPROM.begin(4096);
  isAP = EEPROM.read(256);

  Serial.begin(115200);
  Serial.println(isAP);
  
  if (isAP != 1 & isAP != 3){
    Serial.println("Building AP server");
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);
    server.on("/", HandleIndex);
    server.on("/setWifiConf",HandleWifiConf);

    if (!MDNS.begin("esp8266")) {             // Start the mDNS responder for esp8266.local
      Serial.println("Error setting up MDNS responder!");
    }
    Serial.println("mDNS responder started");
  

    server.begin();
  }

  if (isAP == 1){
    EEPROM.get(0, cfg);

    Serial.print("Connecting to ");
    Serial.print(cfg.ssid);
    WiFi.begin(cfg.ssid, cfg.pass);
    while (WiFi.status() != WL_CONNECTED) {
      if (counter > 60){
        ResetEEPROM();
        break;}
      delay(500);
      Serial.print(".");
      counter++;
    }
    // Print local IP address and start web server
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HandleVue);
    server.on("/setToken", HandleSetToken);

    if (!MDNS.begin("esp8266")) {             // Start the mDNS responder for esp8266.local
      Serial.println("Error setting up MDNS responder!");
    }
    Serial.println("mDNS responder started");

    server.begin();
  }

  if (isAP == 3){
    EEPROM.get(0, cfg);
    EEPROM.get(200, usr);
    Serial.print("Connecting to ");
    Serial.print(cfg.ssid);
    WiFi.begin(cfg.ssid, cfg.pass);
    while (WiFi.status() != WL_CONNECTED) {
      if (counter > 60){
        ResetEEPROM();
        break;}
      delay(500);
      Serial.print(".");
      counter++;
    }
    host += String(usr.token);
  }
}







void loop() {
  bool isRST = false;
  if (isAP == 0 || isAP == 1){
  server.handleClient();
  }
  if (isAP == 3){

    String response = httpGETRequest(host);
    Serial.println(response);
    StaticJsonDocument<256> doc;

    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
    }

    bool status = doc["data"]["status"];
    digitalWrite(4,status);
  }
  
  isRST = !digitalRead(0);
  if (isRST){
    delay(4000);
    isRST = isRST = !digitalRead(0);
    if (isRST){ResetEEPROM();}
  }



}







void HandleIndex(){
  server.send(200, "text/html", GetIndexContent()); 
}

void HandleWifiConf(){
  if( ! server.hasArg("ssid") || ! server.hasArg("password") 
      || server.arg("ssid") == NULL || server.arg("password") == NULL) {
      server.send(400, "text/plain", "400: Invalid Request");
      }

  else{
    EEPROM.get(0, cfg);
    server.arg("ssid").toCharArray(cfg.ssid, 16);
    server.arg("password").toCharArray(cfg.pass, 16);
    EEPROM.put(0, cfg);
    EEPROM.write(256,1);
    EEPROM.commit();
    
    Serial.println(cfg.ssid);
    Serial.println(cfg.pass);
    server.send(200, "text/plain", "ssid и пароль установлен, перезагрузите устройство");
  }
}
void HandleVue(){
  server.send(200, "text/html", GetIndexVue());
}

void HandleSetToken(){
  if( ! server.hasArg("token") || server.arg("token") == NULL) {
      Serial.println(server.arg("plain"));
      server.send(400, "text/plain", "400: Invalid Request");
      }

  else{
    server.arg("token").toCharArray(usr.token, 40);
    EEPROM.put(200, usr);
    EEPROM.write(256, 3);
    EEPROM.commit();
    
    Serial.println(usr.token);
    server.send(200, "text/plain", "email и пароль установлен, перезагрузите устройство");
  }
}

String GetIndexContent(){
  String index_content = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><style>";
  index_content += "html{text-align:center;}form{margin-top:200px;}.mb-3{margin-top:5px;}</style>";
  index_content += "</head><body><h1>Введите данные Wifi сети</h1><form method='post' action='/setWifiConf' >";
  index_content += "<div class='mb-3'><input type='text' name='ssid' placeholder='Название сети'/></div><div class='mb-3'><input type='password' name='password' placeholder='Пароль'/></div><div class='mb-3'><button class='btn btn-primary d-block w-100' type='submit'>Установить</button></div>";
  index_content += "</form></section></body></html>";
  
  return index_content;
}

String GetIndexVue(){
  String index_content = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'>";
  index_content += "<link rel='icon' href='/favicon.ico'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Vite App</title>";
  index_content += "<script type='module' crossorigin src='https://homecontrol.kardasov.ru/comp/index.js'></script><link rel='stylesheet' href='https://homecontrol.kardasov.ru/comp/index.css'>";
  index_content += "</head><body><div id='app'></div></body></html>";
  
  return index_content;
}

String httpGETRequest(String serverName) {
  Serial.println(serverName);
  WiFiClientSecure client;
  HTTPClient http;
  client.setInsecure();
  client.connect(serverName, 433);
  http.begin(client, serverName);
  http.setReuse(false);

  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void ResetEEPROM(){
  for (int i = 0; i < 257; i++ ){
    EEPROM.write(i,0);
  }
  EEPROM.commit();
  for (int i = 0; i < 4; i++ ){
      digitalWrite(LED_BUILTIN, LOW);
      delay(150);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(150);
  }
}
