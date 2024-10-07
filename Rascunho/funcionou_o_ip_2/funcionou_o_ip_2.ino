#include <WiFiManager.h>
#include <WebServer.h>
#include <HTTPClient.h>

WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "Servidor ESP32 em funcionamento");
  Serial.println("Recebeu uma solicitação HTTP GET");
}

void handlePost() {
  if (server.hasArg("idShower")) {
    String idShower = server.arg("idShower");
    Serial.println("Id do chuveiro: " + idShower);
    
    if (server.hasArg("alert")) {
      String alert = server.arg("alert");
      Serial.println("Alerta: " + alert);
    }
    
    if (server.hasArg("temp")) {
      String temp = server.arg("temp");
      Serial.println("Temporizador: " + temp);
    }
    
    if (server.hasArg("time")) {
      String time = server.arg("time");
      Serial.println("Tempo: " + time);
    }

    server.send(200, "text/plain", "Dados recebidos com sucesso");
  } else {
    server.send(400, "text/plain", "Parâmetro 'idShower' não encontrado");
  }
}

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

  // Envia o IP para o backend
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://192.168.15.80:3001/esp32/sendIp"); // Substitua "backend-ip" pelo IP do backend
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
  server.on("/", HTTP_GET, handleRoot);
  server.on("/update", HTTP_POST, handlePost);

  server.begin();
}

void loop() {
  server.handleClient();
}
