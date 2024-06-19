#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPIFFS.h>

const char *ssid = "moto g(7) play 2026";
const char *password = "gustavoppc";
//IPAddress ip(192, 168, 1, 100); // Endereço IP fixo que você deseja atribuir ao ESP32
//IPAddress gateway(192, 168, 1, 1);
//IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

void setup() {
    Serial.begin(115200);  
  
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando ao WiFi...");
    }
    Serial.println("Conectado ao WiFi");

//    WiFi.config(ip, gateway, subnet );
    Serial.println(WiFi.localIP());

    if(!SPIFFS.begin(true)){
        Serial.println("Ocorreu um erro ao montar SPIFFS");
        return;
    }
  
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", "text/html");
        Serial.println("Recebeu uma solicitação http");
    });

    server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
      Serial.println("Recebeu uma solicitação POST");

      if (request->hasParam("idShower", true)) {
        String valorRecebido = request->getParam("idShower", true)->value();
        Serial.println("Id do chuveiro: " + valorRecebido);
        
        String valorRecebido2 = request->getParam("alert", true)->value();
        Serial.println("alerta: " + valorRecebido2);

        String valorRecebido3 = request->getParam("temp", true)->value();
        Serial.println("temporizador: " + valorRecebido3);

        String valorRecebido4 = request->getParam("time", true)->value();
        Serial.println("tempo: " + valorRecebido4);
        request->send(200);
      }
      else {
        Serial.println("Valor 'value' não presente na solicitação POST");
        request->send(400);
      }
//
//    // Adicionando cabeçalhos CORS
      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", "Recebido");
      response->addHeader("Access-Control-Allow-Origin", "*");
    });

    server.begin();
}

void loop() { 
}
