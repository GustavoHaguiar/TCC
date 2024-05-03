#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPIFFS.h>

String valorRecebido1;
String valorRecebido2;

IPAddress ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
const char *ssid = "moto g(7) play 2026";
const char *password = "gustavoppc";

AsyncWebServer server(80);


String WifiAp(){
  Serial.begin(115200);
  const char *ssid = "ShowerNetwork";
  const char *password = "";

  if (!WiFi.softAP(ssid, password)) {
    log_e("Soft AP creation failed.");
    while(1);
  }
  WiFi.config(ip=ip, gateway=gateway, subnet=subnet );

  Serial.println(WiFi.softAPIP());

  if(!SPIFFS.begin(true)){
    Serial.println("Ocorreu um erro ao montar SPIFFS");
    return "";
  }
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
    Serial.println("Recebeu uma solicitação http");
  });

  server.on("/WiFi", HTTP_POST, [](AsyncWebServerRequest *request){
    Serial.println("Recebeu uma solicitação POST");

    if (request->hasParam("ssid", true) && request->hasParam("senha", true)) {
      valorRecebido1 = request->getParam("ssid", true)->value();
      Serial.println("Valor recebido1: " + valorRecebido1);
      valorRecebido2 = request->getParam("senha", true)->value();
      Serial.println("Valor recebido2: " + valorRecebido2);  
        
        
      request->send(200);
    }
    else {
      Serial.println("Valores não presentes na solicitação POST");
      request->send(400);
    }

    // Adicionando cabeçalhos CORS
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", "Recebido");
    response->addHeader("Access-Control-Allow-Origin", "*");
  });

  server.begin();

  String result = "{" + valorRecebido1 + "}" + "{" + valorRecebido2 + "}";

  return result;
}


void setup() {
    Serial.begin(115200);  

    WifiAp();
  
//    WiFi.begin(ssid, password);
//    while (WiFi.status() != WL_CONNECTED) {
//        delay(1000);
//    }
//    Serial.println("Conectado ao WiFi");
//    Serial.println(WiFi.localIP());
//
//    if(!SPIFFS.begin(true)){
//        Serial.println("Ocorreu um erro ao montar SPIFFS");
//        return;
//    }
//  
//    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
//        request->send(SPIFFS, "/index.html", "text/html");
//        Serial.println("Recebeu uma solicitação http");
//    });
//
//    server.on("/WiFi", HTTP_POST, [](AsyncWebServerRequest *request){
//      Serial.println("Recebeu uma solicitação POST");
//
//      if (request->hasParam("ssid", true) && request->hasParam("senha", true)) {
//        String valorRecebido1 = request->getParam("ssid", true)->value();
//        Serial.println("Valor recebido1: " + valorRecebido1);
//        String valorRecebido2 = request->getParam("senha", true)->value();
//        Serial.println("Valor recebido2: " + valorRecebido2);  
//        
//        
//        request->send(200);
//      }
//      else {
//        Serial.println("Valores não presentes na solicitação POST");
//        request->send(400);
//      }
////
////    // Adicionando cabeçalhos CORS
//      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", "Recebido");
//      response->addHeader("Access-Control-Allow-Origin", "*");
//    });
//
//    server.begin();
}

void loop() { 
}
