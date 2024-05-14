int Led[10][7] = {
  {0,1,1,1,1,1,1}, // 0
  {0,0,0,0,1,1,0}, // 1
  {1,0,1,1,0,1,1}, // 2
  {1,0,0,1,1,1,1}, // 3
  {1,1,0,0,1,1,0}, // 4
  {1,1,0,1,1,0,1}, // 5
  {1,1,1,1,1,0,1}, // 6
  {0,1,0,0,1,1,1}, // 7
  {1,1,1,1,1,1,1}, // 8
  {1,1,0,1,1,1,1}  // 9
};

int pins[7] = {
  12,
  13,
  14,
  15,
  18,
  19,
  22
};

#define MRst 3
#define CLK 4
#define Digit 9
//#define PotPin A0
#define PotPin 34

#define ZERO_CROSS 2
#define POT A0
#define SIGNAL 4

//TaskHandle_t Potencia;

volatile int counter = 0;
volatile bool active = false;
int percentage = 0;
const double PERIOD = 1000/120;

//void ShowerProcess( void * parameter);
//void zeroDetection();

void setup() {
  Serial.begin(115200);
  pinMode(CLK, OUTPUT);
  pinMode(MRst, OUTPUT);
  pinMode(Digit, OUTPUT);

  for (int sla=0; sla<8; sla++){
    pinMode(pins[sla], OUTPUT);
  }

  pinMode(ZERO_CROSS, INPUT);
  pinMode(POT, INPUT);
  pinMode(SIGNAL, OUTPUT);

  

//  xTaskCreatePinnedToCore(
//      ShowerProcess, /* Function to implement the task */
//      "Potencia", /* Name of the task */
//      10000,  /* Stack size in words */
//      NULL,  /* Task input parameter */
//      0,  /* Priority of the task */
//      &Potencia,  /* Task handle. */
//      0); /* Core where the task should run */
}

void loop() {

  //Serial.print("setup() running on core ");
  //Serial.println(xPortGetCoreID());

  int pot = analogRead(PotPin);
  digitalWrite(MRst, 1);
  pot = map(pot, 0, 4095, 0, 9);

  Serial.print("POT: ");
  Serial.println(pot);
  Serial.println("");


  for (int i = 0; i < 8; i++){
    digitalWrite(pins[i], Led[pot][i]);  
  }
  

  
  
  
//  
//  for (int col=0; col < 8; col++){
//    digitalWrite(CLK, 0);
//    digitalWrite(Digit, Led[pot][col]);
//    //delay(200);
//    digitalWrite(CLK, 1);
//  }
//
//  digitalWrite(MRst, 0);
////  Serial.println();
//}

/*************** Functions *****************/
//void zeroDetection(){
//  delayMicroseconds(PERIOD * percentage * 10);
//  digitalWrite(SIGNAL, HIGH);
//}
//
//void ShowerProcess( void * parameter){
//
//  attachInterrupt(digitalPinToInterrupt(ZERO_CROSS), zeroDetection, RISING);
//
//  percentage = map(analogRead(POT), 0, 4095, 0, 100);
//
//  Serial.print("setup() running on core ");
//  Serial.println(xPortGetCoreID());
//
//  delayMicroseconds(700);
//  digitalWrite(SIGNAL, LOW);
//
//  Serial.print(percentage);
//}
