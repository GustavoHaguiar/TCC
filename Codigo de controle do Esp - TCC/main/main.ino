// Libraries
#include <WiFiManager.h>
#include <WebServer.h>
#include <HTTPClient.h>

// Instância WebServer
WebServer server(80);
// Instância WiFiManager
WiFiManager wifiManager;

//define pins
#define pinRST 4

// define variables
unsigned long startTime = 0;      // initial time
unsigned long interval = 0;       // time for shower
unsigned long intervalAlert = 0;  // time for alart
bool timerSet = false;            // time defined by user
bool alert2 = false;              // variable alternative to alert
bool alertTemp = false;           // other variable alternative to alert
bool active = true;               // variable as verify if has energy on shower (on/off)
int valRST;                       // value of pin RESET
String idShower2, temp2;          // variable as armazened datas: id and temporizer of shower
int times2;                       // value alternative of time of shower
int power = 7;                    // variable of power of the bath

// function to create a main screen on route GET of the aplication
void handleRoot() {
  server.send(200, "text/plain", "Servidor ESP32 em funcionamento");
  Serial.println("Recebeu uma solicitação HTTP GET");
}

// function to create a post route of the aplication
void handlePost() {
  if (server.hasArg("idShower")) {              // verify as the argument has the parameter "idShower"
    String idShower = server.arg("idShower"); 
    idShower2 = idShower;
    
    if (server.hasArg("alert")) {               // verify as the argument has the parameter "alert"
      String alert = server.arg("alert");
      alert2 = alert;
      alertTemp = alert;
    }
    
    if (server.hasArg("temp")) {                // verify as the argument has the parameter "temp"
      String temp = server.arg("temp");
      temp2 = temp;
    }
    
    if (server.hasArg("time")) {                // verify as the argument has the parameter "time"
      String times = server.arg("time");
      times2 = times.toInt();

      
      interval = times.toInt() * 60 * 1000;             // Get the time in milisseconds to shower
      intervalAlert = (times.toInt() - 1) * 60 * 1000;  // Get the time in milisseconds to alert
      Serial.print("Tempo para emitir alerta: ");
      Serial.println(intervalAlert);
      Serial.print("Tempo para fim do banho: ");
      Serial.println(interval);
      startTime = millis();                             // Start the time to counter
      timerSet = true;
    }

    server.send(200, "text/plain", "Dados recebidos com sucesso");          // send 200 code if everything ok
  } else {
    server.send(400, "text/plain", "Parâmetro 'idShower' não encontrado");  // send 400 code if everything isn't ok
  }
}

// function as send Ip to backend app
void sendIPAddress() {
  if (WiFi.status() == WL_CONNECTED) {                        // verify if WiFi was conected
    HTTPClient http;                                          // Start a intance of HTTPClient class
    String localIp = WiFi.localIP().toString();               // Get the shower ip and save in a string
    String url = "http://192.168.43.144:3001/esp32/sendIp";   // save the url of backend app
    String payload = "{\"ip\":\"" + localIp + "\"}";          // save the payload of post

    http.begin(url);                                          // start the conection with backend
    http.addHeader("Content-Type", "application/json");       // add cors
    http.setTimeout(10000);                                   // set 10 seconds to end of post verify

    Serial.print("Enviando IP para: ");
    Serial.println(url);
    Serial.print("Payload: ");
    Serial.println(payload);

    int httpResponseCode = http.POST(payload);                // send post

    if (httpResponseCode > 0) {                               // verify if has response code of the post
      String response = http.getString();                     // get the response code and save in a string
      Serial.print("Código de resposta: ");
      Serial.println(httpResponseCode);
      Serial.print("Resposta: ");
      Serial.println(response);
    } else {
      Serial.print("Erro ao registrar IP: ");
      Serial.println(httpResponseCode);
      Serial.print("Erro HTTP: ");
      Serial.println(http.errorToString(httpResponseCode));
    }
    http.end();                                             // end http conection
  } else {
    Serial.println("WiFi não está conectado");
  }
}

// function to send logs of bath
void sendLogs(String idShowerLog, int timeLog, int powerLog) {
  if (WiFi.status() == WL_CONNECTED) {                            // verify if WiFi was conected
    HTTPClient http;                                              // Start a intance of HTTPClient class
    String url = "http://192.168.43.144:3001/postLog";            // save the url to post log (save the route)
    String payload = "{\"idShower\":\"" + idShowerLog + "\", \"time\":\"" + timeLog + "\", \"power\":\"" + powerLog + "\"}";

    http.begin(url);                                              // start the conection with backend
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(10000);                                       // set 10 seconds to end of post verify

    Serial.print("Enviando Log para: ");
    Serial.println(url);
    Serial.print("Payload: ");
    Serial.println(payload);

    int httpResponseCode = http.POST(payload);                    // send post

    if (httpResponseCode > 0) {                                   // verify if has response code of the post
      String response = http.getString();                         // get the response code and save in a string
      Serial.print("Código de resposta: ");
      Serial.println(httpResponseCode);
      Serial.print("Resposta: ");
      Serial.println(response);
    } else {
      Serial.print("Erro ao enviar Log: ");
      Serial.println(httpResponseCode);
      Serial.print("Erro HTTP: ");
      Serial.println(http.errorToString(httpResponseCode));
    }
    http.end();                                                   // end http conection
  } else {
    Serial.println("WiFi não está conectado");
  }
}

// Start the initial configuration
void setup() {
  Serial.begin(115200);                   // speed of readding of processor
  pinMode(pinRST, INPUT);                 // pin to RESET

  // Reset the WiFi configurations
  // wifiManager.resetSettings();           // clear the older configurations

  // Start the WiFi AP and start a configuration portal
  if (!wifiManager.autoConnect("ESP32ConfigAP", "")) {
    Serial.println("Falha ao conectar e timeout atingido");
    // Reinicia o ESP
    ESP.restart();
  }

  // WiFi conected
  Serial.println("Conectado à rede WiFi!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Send ip to backend
  sendIPAddress();

  // Configure the web server
  server.on("/", HTTP_GET, handleRoot);             // root server (get)
  server.on("/update", HTTP_POST, handlePost);      // server to recive the configuration of bath

  server.begin();                                   // start server
}

void loop() {
  if (active){                                      // verify if shower was on
    server.handleClient();
  
    valRST = digitalRead(pinRST);                   // read the state of reset pin
  
    // Reinicia o WiFi do ESP32
    if (!valRST) {                                  // control the button reset
      Serial.print("valRST: ");
      Serial.println(valRST);
      wifiManager.resetSettings();
      ESP.restart();
    }
  
    // Verify if the temporizer was been configured and if the time past
    if (alertTemp && (millis() - startTime >= intervalAlert)) {
      Serial.println("1 min para o fim do banho");
      if (alertTemp & (millis() - startTime >= (intervalAlert + 500))) {        // verify if the time of alert was pasted
        alertTemp = false;
      }
    }
    
    if (timerSet && (millis() - startTime >= interval)) {                       // verify if the bath is ended
      Serial.println("Tempo passou. Enviando sinal...");
      Serial.println(idShower2);
      Serial.println(alert2);
      Serial.println(temp2);
      Serial.println(times2);
      sendLogs(idShower2, times2, power);                                      // send the logs of bath
        
      // Reset the temporizer
      startTime = millis();
      alertTemp = alert2;
      // timerSet = false;
    }
  }
}
