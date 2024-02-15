#define IN A0
#define OUT 9
#define RST 7
#define in_supply 2
#define out_supply 5

void setup(){
  Serial.begin(3600);
  pinMode(IN, INPUT);
  pinMode(in_supply, INPUT);
  pinMode(out_supply, OUTPUT);
  pinMode(OUT, OUTPUT);
  pinMode(RST, OUTPUT);
  digitalWrite(RST, 0);
  digitalWrite(OUT, 1);
  digitalWrite(OUT, 0);
  //digitalWrite(out_supply, 1);
}

/*
 * Falta corrigir para começar em 1
 * Verificar se o temp anterior é o mesmo para n sobre escrever
 * Como o processador de cima vai saber que o chuveiro está desligado?
 */


void loop(){
  bool POWER = digitalRead(in_supply); // verifica se o aparelho está ligado
  digitalWrite(out_supply, POWER);
  
  if (POWER){
    
    int temp = analogRead(IN);
    temp = map(temp, 0, 1024, 1, 9); // mapeia a temperatura de 1 a 9
    
    int count = 0; // contador para subir os numero do display
    
    do{
      digitalWrite(OUT, 1);
      digitalWrite(OUT, 0); // clock do contador
      
      count++; // sobi um numero
    }while(count < temp);

    count = 0; // reseta o contador dos numeros
    
  }else{
    digitalWrite(in_supply, 0); // desliga o display
  }
}
