#define DSP_D 12 // Dados display
#define DSP_CLK 14 // Clock display

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


void setup() {
  Serial.begin(115200);
  pinMode(34, INPUT);
  pinMode(DSP_CLK, OUTPUT);
  pinMode(DSP_D, OUTPUT);

}

void loop() {
  int number = analogRead(34);
  number = map(number, 0, 4095, 0, 9);
  Serial.println(number);
  for(int i : ctrDisplay[number]) { 
    
    digitalWrite(DSP_D, i); // Atualiza o bit de dados
    digitalWrite(DSP_CLK, HIGH); // Pulso positivo de clock
    digitalWrite(DSP_CLK, LOW); // Pulso negativo de clock 
  }

}
