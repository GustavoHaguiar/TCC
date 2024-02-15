#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
 
const char* ssid = "Access Point";
const char* password =  "";
 
WebServer server(80);
 
void setup(){
  Serial.begin(115200);
 
  if(!SPIFFS.begin()){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
  }
 
  WiFi.begin(ssid, password);
 
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.println("Ponto de acesso criado com sucesso!");
  Serial.print("Endereço IP do ESP32: ");
  Serial.println(WiFi.softAPIP());
 
  server.on("/index", HTTP_GET, [](){
    //server.send(200, "/index.html", "text/html");
    server.send(200, "text/html", "/index.html");
  });
 
  server.begin();
}
 
void loop(){}
