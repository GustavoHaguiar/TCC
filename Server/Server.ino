#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "moto g(7) play 2026";
const char *password = "gustavoppc";

#define pinLed 2

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(pinLed, OUTPUT);
  
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");
  Serial.println(WiFi.localIP());

  
  server.on("/true", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(pinLed, HIGH);
    
    Serial.println("Recebeu uma solicitação para ligar");
    request->send(200, "text/plain", "HIGH");
  });

  server.on("/false", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(pinLed, LOW);
    
    Serial.println("Recebeu uma solicitação para desligar");
    request->send(200, "text/plain", "LOW");
  });
  
  server.begin();
}

void loop() {
  
}
