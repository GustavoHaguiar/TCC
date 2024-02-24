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

#define MR 2
#define CLK 4
#define Digit 9
#define PotPin A0

void setup() {
  Serial.begin(115200);
  pinMode(CLK, OUTPUT);
  pinMode(MR, OUTPUT);
  pinMode(Digit, OUTPUT);  
}

void loop() {
  int pot = analogRead(PotPin);
  digitalWrite(MR, 1);
  
  pot = map(pot, 0, 4095, 0, 9);

//  Serial.print("POT: ");
//  Serial.println(pot);
//  Serial.println("");
  
  for (int col=0; col < 8; col++){
    digitalWrite(CLK, 0);
    digitalWrite(Digit, Led[pot][col]);
    //delay(200);
    digitalWrite(CLK, 1);
  }

  digitalWrite(MR, 0);
//  Serial.println();
}
