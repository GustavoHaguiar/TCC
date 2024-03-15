#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPIFFS.h>

const char *ssid = "Lucas_AP";
const char *password = "lcs.2024";

AsyncWebServer server(80);

void setup() {
    Serial.begin(115200);  
  
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando ao WiFi...");
    }
    Serial.println("Conectado ao WiFi");
    Serial.println(WiFi.localIP());

    if(!SPIFFS.begin(true)){
        Serial.println("Ocorreu um erro ao montar SPIFFS");
        return;
    }
  
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", "text/html");
        Serial.println("Recebeu uma solicitação http");
    });

    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request){
        Serial.println("Recebeu uma solicitação POST");

        if (request->hasParam("value", true)) {
            String valorRecebido = request->getParam("value", true)->value();
            Serial.println("Valor recebido: " + valorRecebido);
            request->send(200);
        }
        else {
            Serial.println("Valor 'value' não presente na solicitação POST");
            request->send(400);
        }
    });

    server.begin();
}

void loop() { 
}
