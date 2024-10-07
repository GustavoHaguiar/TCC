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
#define BUZ 26 // Buzzer
#define DSP_D 12 // Dados display
#define DSP_CLK 14 // Clock display
#define USR 34 // Potenciômetro
#define V_REF 33 // Sensor de tensão
#define ITR 32 // Interrupção Attiny85

int ctrInput, // Valor lido no potenciômetro 
    ctrVref, // Tensão mínima para considerar o chuveiro como ligado
    dtaBathTime = 30; // Tempo de banho, por enquanto mantido estático, será atualizado via app

int dtaVolt,dtaPwr; // Configuráveis via app

float ctrMidPower; // Potência média em que o chuveiro operou em determinado banho 

unsigned long ctrStart; // Tempo em milissegundos do início do banho

bool ctrOn, // Indica se o chuveiro já se encontra ligado
        dtaActive, // Indica se o buzzer será utilizado, atualizado via app
        dtaAutoOff; // Indica se o final do banho será forçado, via app

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

void setup() {

  // Definições de entrada e saída de cada pino
  pinMode(BUZ, OUTPUT);
  pinMode(DSP_CLK, OUTPUT);
  pinMode(DSP_D, OUTPUT);
  pinMode(USR, INPUT);
  pinMode(V_REF, INPUT);
  pinMode(ITR,OUTPUT);

  // Reseta o display, necessário para sincronização do 74HC164
  show(10);

  // Inicializa comunicação serial, para fins de debug
  Serial.begin(115200);

  // Próximas linhas são instruções de teste e devem ser retiradas na versão final
  dtaActive = true;
  dtaAutoOff = true;
  ctrVref = 744;
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
    if(_now - ctrStart >= (dtaBathTime - 15)* 1000 && dtaActive) {

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
    
    if(!dtaAutoOff && ctrOn) updateData(); // Atualiza consumo se usuário desligar o chuveiro dentro do prazo
    
    ctrOn = false; // Define ctrOn como falso
    show(10); // Desliga display
    dtaActive = true; // Volta a permitir acionamento do buzzer
    ctrMidPower = 0; // Volta a potência média para zero

  }
  digitalWrite(ITR, LOW); // Como padrão, mantém sinal de interrupção em zero
  delay(100); // Aguarda 100ms e repete o processo, poderá ser retirado na versão final
}
