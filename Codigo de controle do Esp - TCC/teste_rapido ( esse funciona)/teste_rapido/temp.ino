//#include <WiFiManager.h>
//#include <WebServer.h>
//#include <HTTPClient.h>
//
//// Instância WebServer
//WebServer server(80);
//// Instância WiFiManager
//WiFiManager wifiManager;
//
////#define pinSensor 3
////#define pinRST 4
////#define pinInterrupt 5
////#define MRst 3
////#define CLK 4
////#define Digit 9
////#define PotPin 34
//
//unsigned long startTime = 0;
//unsigned long interval = 0;
//bool timerSet = false;
////int valRST;
////int valSensor;
//
////int Led[10][7] = {
////  {0,1,1,1,1,1,1}, // 0
////  {0,0,0,0,1,1,0}, // 1
////  {1,0,1,1,0,1,1}, // 2
////  {1,0,0,1,1,1,1}, // 3
////  {1,1,0,0,1,1,0}, // 4
////  {1,1,0,1,1,0,1}, // 5
////  {1,1,1,1,1,0,1}, // 6
////  {0,1,0,0,1,1,1}, // 7
////  {1,1,1,1,1,1,1}, // 8
////  {1,1,0,1,1,1,1}  // 9
////};
//
//void handleRoot() {
//  server.send(200, "text/plain", "Servidor ESP32 em funcionamento");
//  Serial.println("Recebeu uma solicitação HTTP GET");
//}
//
//void handlePost() {
//  if (server.hasArg("idShower")) {
//    String idShower = server.arg("idShower");
//    Serial.println("Id do chuveiro: " + idShower);
//    
//    if (server.hasArg("alert")) {
//      String alert = server.arg("alert");
//      Serial.println("Alerta: " + alert);
//    }
//    
//    if (server.hasArg("temp")) {
//      String temp = server.arg("temp");
//      Serial.println("Temporizador: " + temp);
//    }
//    
//    if (server.hasArg("time")) {
//      String times = server.arg("time");
//      Serial.println("Tempo: " + times);
//
//      // Converte o tempo de minutos para milissegundos
//      interval = times.toInt() * 60 * 1000;
//      startTime = millis();
//      timerSet = true;
//    }
//
//    server.send(200, "text/plain", "Dados recebidos com sucesso");
//  } else {
//    server.send(400, "text/plain", "Parâmetro 'idShower' não encontrado");
//  }
//}
//
////void sendLogs(String idShower, String times, String power){
////  // Envia o log para o backend
////  if (WiFi.status() == WL_CONNECTED) {
////    HTTPClient http;
////    http.begin("http://192.168.15.80:3001/logs");
////    http.addHeader("Content-Type", "application/json");
////    String payload = "{\"idShower\":\"" + idShower + "\", \"time\":\"" + times + "\", \"power\":\"" + power + "\"}";
////    int httpResponseCode = http.POST(payload);
////    if (httpResponseCode > 0) {
////      String response = http.getString();
////      Serial.println(httpResponseCode);
////      Serial.println(response);
////    } else {
////      Serial.println("Erro ao enviar os logs");
////    }
////    http.end();  // Certifique-se de liberar os recursos
////  } else {
////    Serial.println("WiFi não está conectado");
////  }
////}
//
//void setup() {
//  Serial.begin(115200);
////  pinMode(pinSensor, INPUT);
////  pinMode(pinRST, INPUT);
////  pinMode(pinInterrupt, OUTPUT);
////  pinMode(CLK, OUTPUT);
////  pinMode(MRst, OUTPUT);
////  pinMode(Digit, OUTPUT);
//
//  // Reseta configurações WiFi para testes
//  // wifiManager.resetSettings(); // Descomente para limpar as configurações anteriores
//
//  // Inicia um AP e habilita o portal de configuração
//  if (!wifiManager.autoConnect("ESP32ConfigAP", "")) {
//    Serial.println("Falha ao conectar e timeout atingido");
//    // Reinicia o ESP
//    ESP.restart();
//  }
//
//  // Se chegou aqui, já está conectado à rede WiFi
//  Serial.println("Conectado à rede WiFi!");
//  String localIp = WiFi.localIP().toString();
//  Serial.println(localIp);
//
//  // Envia o IP para o backend
//  if (WiFi.status() == WL_CONNECTED) {
//    HTTPClient http;
//    http.begin("http://192.168.15.80:3001/esp32/register");
//    http.addHeader("Content-Type", "application/json");
//    String payload = "{\"ip\":\"" + localIp + "\"}";
//    int httpResponseCode = http.POST(payload);
//    if (httpResponseCode > 0) {
//      String response = http.getString();
//      Serial.println(httpResponseCode);
//      Serial.println(response);
//    } else {
//      Serial.println("Erro ao registrar IP");
//    }
//    http.end();  // Certifique-se de liberar os recursos
//  } else {
//    Serial.println("WiFi não está conectado");
//  }
// 
//
//  // Configura o servidor web para lidar com requisições GET e POST
//  server.on("/", HTTP_GET, handleRoot);
//  server.on("/update", HTTP_POST, handlePost);
//
//  server.begin();
//}
//
//void loop() {
//  server.handleClient();
//
////  valRST = digitalRead(pinRST);
////  valSensor = 3;// analogRead(pinSensor);
//
//  // reinicia o WiFi do ESP32
////  if(!valRST){
////    Serial.print("valRST: ");
////    Serial.println(valRST);
////    wifiManager.resetSettings();
////    ESP.restart();
////  }
//
//  //verifica se o valor do sensor de corrente é maior que 2A
////  if( valSensor > 2){
////
////    // inicio imprime valor no 7SEG///////////////
////    int pot = analogRead(PotPin);               //
////    digitalWrite(MRst, 1);                      //
////    pot = map(pot, 0, 4095, 0, 9);              //
////                                                //
////    Serial.print("POT: ");                      //
////    Serial.println(pot);                        //
////    Serial.println("");                         //
////                                                //
////                                                //
////    for (int i = 0; i < 8; i++){                //
////      // digitalWrite(pins[i], Led[pot][i]);    //
////    }                                           //
////    // fim imprime valor no 7SEG//////////////////
//  
//    // Verifica se o temporizador foi configurado e se o tempo passou
////    if (timerSet && (millis() - startTime >= interval)) {
////      Serial.println("Tempo passou. Enviando sinal...");
////      Serial.println(idShower);
////      Serial.println(alert);
////      Serial.println(temp);
////      Serial.println(times);
//////      sendLogs("id","time","power");
////      
////      // Reseta o temporizador
////      timerSet = false;
////    //}
////  }
//}
