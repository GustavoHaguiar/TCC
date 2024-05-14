String WifiAp(){
  Serial.begin(115200);
  const char *ssid = "ShowerNetwork";
  const char *password = "";
  WiFiServer server(80);

  if (!WiFi.softAP(ssid, password)) {
    log_e("Soft AP creation failed.");
    while(1);
  }

  IPAddress IP = "192.168.43.87";
  WiFi.config(IP);

  Serial.begin(WiFi.softAPIP());

  if(!SPIFFS.begin(true)){
        Serial.println("Ocorreu um erro ao montar SPIFFS");
        return;
    }
  
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", "text/html");
        Serial.println("Recebeu uma solicitação http");
    });

    server.on("/WiFi", HTTP_POST, [](AsyncWebServerRequest *request){
      Serial.println("Recebeu uma solicitação POST");

      if (request->hasParam("ssid", true) && request->hasParam("senha", true)) {
        String valorRecebido1 = request->getParam("ssid", true)->value();
        Serial.println("Valor recebido1: " + valorRecebido1);
        String valorRecebido2 = request->getParam("senha", true)->value();
        Serial.println("Valor recebido2: " + valorRecebido2);  
        
        
        request->send(200);
      }
      else {
        Serial.println("Valores não presentes na solicitação POST");
        request->send(400);
      }
//
//    // Adicionando cabeçalhos CORS
      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", "Recebido");
      response->addHeader("Access-Control-Allow-Origin", "*");
    });

    server.begin();
}
