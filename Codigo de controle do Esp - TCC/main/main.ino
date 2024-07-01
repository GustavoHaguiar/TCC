#include <WiFiManager.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>   

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Instancia WiFiManager
  WiFiManager wifiManager;

  // Reseta configurações WiFi para testes
  wifiManager.resetSettings(); // Descomente para limpar as configurações anteriores

  // Inicia um AP e habilita o portal de configuração
  if (!wifiManager.autoConnect("ESP32ConfigAP", "")) {
    Serial.println("Falha ao conectar e timeout atingido");
    // Reinicia o ESP
    ESP.restart();
  }

  // Se chegou aqui, já está conectado à rede WiFi
  Serial.println("Conectado à rede WiFi!");
  String localIp = WiFi.localIP().toString();
  Serial.println(localIp);

  // Registra o IP no backend
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://192.168.1.114:3001/esp32/register"); // Substitua "backend-ip" pelo IP do backend
    http.addHeader("Content-Type", "application/json");
    String payload = "{\"ip\":\"" + localIp + "\"}";
    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.println("Erro ao registrar IP");
    }
    http.end();  // Certifique-se de liberar os recursos
  } else {
    Serial.println("WiFi não está conectado");
  }

  // Configura o servidor web para lidar com requisições GET e POST
  AsyncWebServer server(80);
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Servidor ESP32 em funcionamento");
    Serial.println("Recebeu uma solicitação HTTP GET");
  });

  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
    Serial.println("Recebeu uma solicitação HTTP POST");

    if (request->hasParam("idShower", true)) {
      String valorRecebido = request->getParam("idShower", true)->value();
      Serial.println("Id do chuveiro: " + valorRecebido);
      
      if (request->hasParam("alert", true)) {
        String valorRecebido2 = request->getParam("alert", true)->value();
        Serial.println("alerta: " + valorRecebido2);
      }
      
      if (request->hasParam("temp", true)) {
        String valorRecebido3 = request->getParam("temp", true)->value();
        Serial.println("temporizador: " + valorRecebido3);
      }
      
      if (request->hasParam("time", true)) {
        String valorRecebido4 = request->getParam("time", true)->value();
        Serial.println("tempo: " + valorRecebido4);
      }

      request->send(200, "text/plain", "Dados recebidos com sucesso");
    } else {
      Serial.println("Valor 'idShower' não presente na solicitação POST");
      request->send(400, "text/plain", "Parâmetro 'idShower' não encontrado");
    }

    // Adicionando cabeçalhos CORS
    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "Recebido");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.begin();
}

void loop() {
  // Código principal do seu projeto
}
