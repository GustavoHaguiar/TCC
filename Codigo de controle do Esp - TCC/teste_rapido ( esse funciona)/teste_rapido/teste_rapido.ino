#include <WiFiManager.h>
#include <WebServer.h>
#include <HTTPClient.h>

// Instância WebServer
WebServer server(80);
// Instância WiFiManager
WiFiManager wifiManager;

#define pinRST 4

unsigned long startTime = 0;
unsigned long interval = 0;
unsigned long intervalAlert = 0;
bool timerSet = false;
bool alert2 = false;
int valRST;
String idShower2, temp2, times2;

void handleRoot() {
  server.send(200, "text/plain", "Servidor ESP32 em funcionamento");
  Serial.println("Recebeu uma solicitação HTTP GET");
}

void handlePost() {
  if (server.hasArg("idShower")) {
    String idShower = server.arg("idShower");
    idShower2 = idShower;
    // Serial.println("Id do chuveiro: " + idShower);
    
    if (server.hasArg("alert")) {
      String alert = server.arg("alert");
      alert2 = alert;
      // Serial.println("Alerta: " + alert);
    }
    
    if (server.hasArg("temp")) {
      String temp = server.arg("temp");
      temp2 = temp;
      // Serial.println("Temporizador: " + temp);
    }
    
    if (server.hasArg("time")) {
      String times = server.arg("time");
      times2 = times;
      // Serial.println("Tempo: " + times);

//       Converte o tempo de minutos para milissegundos
      interval = times.toInt() * 60 * 1000;
      intervalAlert = (times.toInt()-1) * 60 * 1000;
      Serial.print("Tempo para emitir alerta: ");
      Serial.println(intervalAlert);
      Serial.print("Tempo para fim do banho: ");
      Serial.println(interval);
      startTime = millis();
      timerSet = true;
    }

    server.send(200, "text/plain", "Dados recebidos com sucesso");
  } else {
    server.send(400, "text/plain", "Parâmetro 'idShower' não encontrado");
  }
}

void postLog(String idShower, int timeBath, int power, String date){
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://192.168.15.80:3001/postLog");
    http.addHeader("Content-Type", "application/json");
    String payload = "{\"idShower\":\"" + idShower + "\", \"time\":\"" + timeBath + "\", \"power\":\"" + power + "\", \"date\":\"" + date + "\"}";
    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.println("Erro ao registrar log de uso");
    }
    http.end();
  } else {
    Serial.println("WiFi não está conectado");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(pinRST, INPUT);

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

  // Envia o IP para o backend
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://192.168.15.80:3001/esp32/register");
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

  valRST = digitalRead(pinRST);
  //valSensor = 3;// analogRead(pinSensor);

//   reinicia o WiFi do ESP32
  if(!valRST){
    Serial.print("valRST: ");
    Serial.println(valRST);
    wifiManager.resetSettings();
    ESP.restart();
  }

//       Verifica se o temporizador foi configurado e se o tempo passou
    if(alert2 && (millis() - startTime >= intervalAlert)){
        Serial.println("1 min para o fim do banho");
        if(millis() - startTime >= (intervalAlert + 500)){
          alert2 = false;
        }
      }
    if (timerSet && (millis() - startTime >= interval)) {
      Serial.println("Tempo passou. Enviando sinal...");
      Serial.println(idShower2);
      Serial.println(alert2);
      Serial.println(temp2);
      Serial.println(times2);
      postLog(
        idShower2, 
        int(temp2), 
        int(power2), 
        "date"
        );
      
      // Reseta o temporizador
      startTime = millis();
      // timerSet = false;
   }
}
