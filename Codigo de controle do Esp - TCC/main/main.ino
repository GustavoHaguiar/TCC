/*
 * Ligar energia........................................................................//certo//
 * Crar conexão ap......................................................................//certo//
 * Exibir pagina web....................................................................//certo//
 * Receber dados de rede wifi...........................................................//cert//
 * encerrar conexão ap..................................................................//certo//
 * conectar-se a rede do usuario........................................................//certo//
 * caso de errado, voltar ao "criar conexão ap".........................................//certo//
 * caso de certo: Dividir em 2 ramos principais;........................................//em produção................
 * 1 ramo: controle temperatura;
 *  ler entrada analogica
 *  fazer logica para exibir valor no display
 *  imprimir os bits do display
 * 2 ramo: controle por wifi;
 *  chuveiro está ligado? 2 opções
 *  1 opção: não está;
 *    aguardar comandos do aplicativo
 *    volta para "caso de certo"
 *  2 opção: está ligado;
 *    manter as configurações ja definidas
 *    ao desligar o cuveiro mandar relatorio ( JSON ) para o backend 
*/


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
  // wifiManager.resetSettings(); // Descomente para limpar as configurações anteriores

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
  HTTPClient http;
  http.begin("http://192.168.15.80:3001/esp32/register"); // Substitua "backend-ip" pelo IP do backend
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
  http.end();

  // Configura o servidor web para lidar com requisições GET e POST
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Servidor ESP32 em funcionamento");
    Serial.println("Recebeu uma solicitação HTTP GET");
  });

  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
    Serial.println("Recebeu uma solicitação HTTP POST");

    if (request->hasParam("idShower", true)) {
      String valorRecebido = request->getParam("idShower", true)->value();
      Serial.println("Id do chuveiro: " + valorRecebido);
      
      String valorRecebido2 = request->getParam("alert", true)->value();
      Serial.println("alerta: " + valorRecebido2);

      String valorRecebido3 = request->getParam("temp", true)->value();
      Serial.println("temporizador: " + valorRecebido3);

      String valorRecebido4 = request->getParam("time", true)->value();
      Serial.println("tempo: " + valorRecebido4);
      request->send(200, "text/plain", "Dados recebidos com sucesso");
    } else {
      Serial.println("Valor 'idShower' não presente na solicitação POST");
      request->send(400, "text/plain", "Parâmetro 'idShower' não encontrado");
    }

    // Adicionando cabeçalhos CORS
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Recebido");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.begin();
}

void loop() {
  // Código principal do seu projeto
}
