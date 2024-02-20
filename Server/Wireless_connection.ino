void Connection() {
  const char *ssid = "Access Point";
  const char *password = "";

  WiFiServer server(80);

  Serial.println();
  Serial.println("Configuring access point...");

  
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("Soft AP creation failed.");
    while(1);
  }
  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  server.begin();
  Serial.println("Server started");

  WiFiClient client = server.available();   

  if (client) {                             
    Serial.println("New Client.");                        
    while (client.connected()) {
      if (client.available()) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println();
        client.print("Hello, World!");
        client.println();
        break;
      } else { 
            Serial.println("Sem usuario");
      }
      client.stop();
      Serial.println("Client Disconnected.");
    }
  }
}
