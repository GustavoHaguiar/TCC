#define IN A0
#define OUT 9
#define RST 7
#define in_supply 2

void setup(){
  Serial.begin(3600);
  pinMode(IN, INPUT);
  pinMode(in_supply, INPUT);
  pinMode(OUT, OUTPUT);
  pinMode(RST, INPUT);
  digitalWrite(RST, 1);
  digitalWrite(OUT, 1);
  digitalWrite(OUT, 0);
}

void loop(){
  bool POWER = digitalRead(in_supply);
  if (POWER){
    int temp = analogRead(IN);
    temp = map(temp, 0, 1024, 1, 9);
    int count = 0;
    do{
      digitalWrite(OUT, 1);
      digitalWrite(OUT, 0);
      count++;
    }while(count < temp);

    count = 0;
  }else{
    digitalWrite(RST, 0);
    digitalWrite(RST, 1);
  }
}
