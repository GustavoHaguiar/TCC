/*
 * Código utilizado para todos os testes com esp32, já foi compilado e não apresentou bugs
 * 
 * Para a versão final, algumas características devem ser alteradas:
 *  - Valores dos pinos mostrados no #define devem corresponder ao hardware;
 *  - Alguns trechos sinalizados durante o código devem ser retirados;
 *  - Função updateData() não foi testada
 *  
 *  Última revisão feita em 14-07-24 por Lucas Souza
 */

// Libraries
#include <WiFiManager.h>
#include <WebServer.h>
#include <HTTPClient.h>

// Instância WebServer
WebServer server(80);
// Instância WiFiManager
WiFiManager wifiManager;

//define pins
#define pinRST 25
#define BUZ 26 // Buzzer
#define DSP_D 12 // Dados display
#define DSP_CLK 14 // Clock display
#define USR 34 // Potenciômetro
#define V_REF 33 // Sensor de tensão
#define ITR 32 // Interrupção Attiny85

// define variables
unsigned long startTime = 0;      // initial time
unsigned long interval = 0;       // time for shower
unsigned long intervalAlert = 0;  // time for alart
int ctrInput, // Valor lido no potenciômetro 
    valRST, // value of pin RESET
    ctrVref, // Tensão mínima para considerar o chuveiro como ligado
    dtaBathTime = 30; // Tempo de banho, por enquanto mantido estático, será atualizado via app

int dtaVolt,dtaPwr; // Configuráveis via app

float ctrMidPower; // Potência média em que o chuveiro operou em determinado banho 

unsigned long ctrStart; // Tempo em milissegundos do início do banho

bool ctrOn, // Indica se o chuveiro já se encontra ligado
        dtaActive, // Indica se o buzzer será utilizado, atualizado via app
        timerSet = false, // time defined by user
        dtaAutoOff; // Indica se o final do banho será forçado, via app
// bool alert2 = false;              // variable alternative to alert
// bool alertTemp = false;           // other variable alternative to alert
bool active = true;               // variable as verify if has energy on shower (on/off)
String idShower2;                 // variable as armazened datas: id and temporizer of shower
// int times2;                       // value alternative of time of shower
// int power = 7;                    // variable of power of the bath
double _E;

// Caracteres que podem ser utilizados no display 7 segmentos
int ctrDisplay[11][7] = {
  //g,f,e,d,c,b,a
  {0,1,1,1,1,1,1}, // 0
  {0,0,0,0,1,1,0}, // 1
  {1,0,1,1,0,1,1}, // 2
  {1,0,0,1,1,1,1}, // 3
  {1,1,0,0,1,1,0}, // 4
  {1,1,0,1,1,0,1}, // 5
  {1,1,1,1,1,0,1}, // 6
  {0,1,0,0,1,1,1}, // 7
  {1,1,1,1,1,1,1}, // 8
  {1,1,0,1,1,1,1}, // 9
  {0,0,0,0,0,0,0}  // reset
};


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
      dtaActive = alert;
      // alertTemp = alert;
    }
    
    if (server.hasArg("temp")) {                // verify as the argument has the parameter "temp"
      String temp = server.arg("temp");
      dtaAutoOff = temp;
    }
    
    if (server.hasArg("time")) {                // verify as the argument has the parameter "time"
      String times = server.arg("time");
      dtaBathTime = (times.toInt())*60;

      
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


void setup() {

  // Definições de entrada e saída de cada pino
  pinMode(BUZ, OUTPUT);
  pinMode(DSP_CLK, OUTPUT);
  pinMode(DSP_D, OUTPUT);
  pinMode(USR, INPUT);
  pinMode(V_REF, INPUT);
  pinMode(ITR,OUTPUT);
  pinMode(pinRST, INPUT); // pin to RESET

  
  // Reset the WiFi configurations
  // wifiManager.resetSettings();           // clear the older configurations

  // Reseta o display, necessário para sincronização do 74HC164
  show(10);

  // Inicializa comunicação serial, para fins de debug
  Serial.begin(115200);

  // Próximas linhas são instruções de teste e devem ser retiradas na versão final
  dtaActive = true;
  dtaAutoOff = true;
  ctrVref = 744;

  
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

/*
 *  Verifica pino de entrada do sinal V_REF (ver esquemático), retorna se o valor de tensão 
 *  é maior que o valor mínimo estipulado por ctrVref.
 */
bool checkBath(){
 
   int peak = 0, // Tensão de pico lida pelo sensor de tensão
        past = millis(), // Tempo decorrido para amostragem
        voltage; // Tensão lida no sensor, atualizará variável peak

  // Define 33ms de tempo de amostragem 
   while(millis() - past < 33){
      voltage = analogRead(V_REF); // Lê valor do sensor
      if(voltage > peak) peak = voltage; // Atualiza peak se tensão lida for maior que o pico atual
   }
   
   return peak > ctrVref; // Se tensão de pico for maior que o parâmetro mínimo, retorna true
}

/*
 * Exibe caracteres no display.
 * 
 * Parâmetro number indica a posição no array ctrDisplay em que o caracter se encontra 
 */
void show(int number) {

  for(int i : ctrDisplay[number]) { 
    
    digitalWrite(DSP_D, i); // Atualiza o bit de dados
    digitalWrite(DSP_CLK, HIGH); // Pulso positivo de clock
    digitalWrite(DSP_CLK, LOW); // Pulso negativo de clock
    
  }
}

/*
 * Ativa buzzer se as condições predeterminadas forem atingidas
 */
void buzzer(){
  Serial.println("buzzer");
    unsigned long _now = millis(); // Momento atual em milissegundos

    // Verifica se o tempo de banho foi excedido e se o buzzer está ativo 
    if(_now - ctrStart >= (dtaBathTime - 60)* 1000 && dtaActive) {

      while(millis() - _now < 700) digitalWrite(BUZ, HIGH);  // Liga buzzer por 0,7 segundos

      digitalWrite(BUZ, LOW); // Desliga buzzer
    
      dtaActive = false;  // Desativa sinal do buzzer para evitar loops infinitos
    }
    
}

/*
 * Define as etapas para encerramento do banho, caso o tempo seja maior que o permitido
 */
void endBath() {
  Serial.println("Ending..");
  
  if(millis() - ctrStart < dtaBathTime * 1000) return; // Sai da função se o tempo estiver no limite
      Serial.println("OK");
      
  do {
    digitalWrite(ITR, HIGH); // Ativa interrupção do Attiny85
    Serial.println("Waiting..");
    delay(1000); // Aguarda um segundo
    digitalWrite(ITR, LOW); // Desativa interrupção do Attiny85
  }
  while(checkBath()); // Repete enquanto o chuveiro não for desligado

  updateData(); // Atualiza consumo

  Serial.println("DONE");
}

/*
 * Atualiza dados de consumo de energia do chuveiro a cada banho tomado
 * 
 * Posteriormente, esses dados são enviados para o app
 */
void updateData() {
  
  int pastTime = millis() - ctrStart;

  // Próximas linhas representam a Equação 4 da monografia
  double x = sqrt(0.5 - 20 * ctrMidPower / 360 + sin(40 * ctrMidPower * PI / 180) / (4 * PI));
  double _E = dtaPwr / dtaVolt * dtaVolt * sqrt(2) * x * 5 * pastTime / 18;

}

/*
 * Loop infinito de execução
 */
void loop() {
  
    server.handleClient();
    
  // Verifica se há um banho ativo
  if(checkBath()) { 

    // Caso ctrOn esteja em zero..
    if(!ctrOn) {
      ctrStart = millis(); // Atualiza valor de ctrStart
      ctrOn = true; // Define ctrOn como verdadeiro
    }
    
    ctrInput = map(analogRead(USR), 0, 4095, 9, 0); // Lê configuração de temperatura escolhida
    ctrMidPower = (ctrMidPower + ctrInput) / 2.0; // Armazena o valor médio da potência consumida
    
    show(ctrInput); // Atualiza display com dados do potenciômetro
    buzzer(); // Liga buzzer, se necessário
    if(dtaAutoOff) endBath(); // Desativa chuveiro se as condições para isto forem atendidas
  }
    
  else {

    valRST = digitalRead(pinRST);                   // read the state of reset pin
  
    // Reinicia o WiFi do ESP32
    if (valRST) {                                  // control the button reset
      Serial.print("valRST: ");
      Serial.println(valRST);
      wifiManager.resetSettings();
      ESP.restart();
    }

    // Verify if the temporizer was been configured and if the time past
//   if (alertTemp && (millis() - startTime >= intervalAlert)) {
//     Serial.println("1 min para o fim do banho");
//     if (alertTemp & (millis() - startTime >= (intervalAlert + 500))) {        // verify if the time of alert was pasted
//       alertTemp = false;
//     }
//   }

   if (timerSet && (millis() - startTime >= interval)) {                       // verify if the bath is ended
      Serial.println("Tempo passou. Enviando sinal...");
      Serial.println(idShower2);
      Serial.println(dtaActive);
      Serial.println(dtaAutoOff);
      Serial.println(dtaBathTime);
      sendLogs(idShower2, dtaBathTime, _E); // send the logs of bath
        
      // Reset the temporizer
      startTime = millis();
      // alertTemp = alert2;
      // timerSet = false;
    }
    
    if(!dtaAutoOff && ctrOn) updateData(); // Atualiza consumo se usuário desligar o chuveiro dentro do prazo
    
    ctrOn = false; // Define ctrOn como falso
    show(10); // Desliga display
    dtaActive = true; // Volta a permitir acionamento do buzzer
    ctrMidPower = 0; // Volta a potência média para zero

  }
  digitalWrite(ITR, LOW); // Como padrão, mantém sinal de interrupção em zero
  delay(100); // Aguarda 100ms e repete o processo, poderá ser retirado na versão final
}
