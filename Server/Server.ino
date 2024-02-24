#include <WiFi.h>
#include <WiFiClient.h>

TaskHandle_t Task1;

const int led = 2;

void setup(){
  Serial.begin(115200); 

  pinMode(led, OUTPUT);

  xTaskCreatePinnedToCore(
    Task1code,   /* Task function. */
    "Task1",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task1,      /* Task handle to keep track of created task */
    0            /* pin task to core 0 */  
  );                          

void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
}

void loop(){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  digitalWrite(led, HIGH);
  delay(1000);
  digitalWrite(led, LOW);
  delay(1000);
}
