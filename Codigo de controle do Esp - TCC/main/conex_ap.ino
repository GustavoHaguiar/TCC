resAp conex_ap(String ssid){
  if(!WiFi.softAP(ssid, "")){
    log_e("Soft AP creation failed.");
    while(1);
  }

  Serial.begin(WiFi.softAPIP());

  if(!SPIFFS.begin(true)){
    Serial.println("Ocorreu um erro ao montar SPIFFS");
    struct resAp res1;
      res1.ssid = "";
      res1.pass = "";

      return res1;
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
    Serial.println("Recebeu uma solicitação http");
  });

  server.on("/WiFi", HTTP_POST, [](AsyncWebServerRequest *request){
    Serial.println("Recebeu uma solicitação POST");

    if (request->hasParam("ssid", true) && request->hasParam("senha", true)) {
      String UserSSID = request->getParam("ssid", true)->value();
      Serial.println("ssid: " + UserSSID);
      String UserPASSWORD = request->getParam("senha", true)->value();
      Serial.println("senha: " + UserPASSWORD);  
        
      request->send(200);

      // Adicionando cabeçalhos CORS
      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", "Recebido");
        response->addHeader("Access-Control-Allow-Origin", "*");
  
      server.begin();
      
      struct resAp res1;
      res1.ssid = UserSSID;
      res1.pass = UserPASSWORD;

      return res1;
    }
    else {
      Serial.println("Valores não presentes na solicitação POST");
      request->send(400);

      // Adicionando cabeçalhos CORS
      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", "Recebido");
        response->addHeader("Access-Control-Allow-Origin", "*");
  
      server.begin();
      struct resAp res1;
      res1.ssid = "";
      res1.pass = "";

      return res1;
    } 
  });
}
