#define analog_in A0
#define power 9
const int vetor_saida[4] = {3,4,5,6};
#define clock_seg1 7
#define clock_seg2 8

bool vector_out[8];

void Dabble(bool A, bool B, bool C, bool D, bool vetor[4]);
void complete_Dabble(bool A, bool B, bool C, bool D, bool E, bool vetor_out[8]);
String binario(int valor, int numDigitos);


void setup() {
  Serial.begin(9600);
  pinMode(power, INPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(clock_seg1, OUTPUT);
  pinMode(clock_seg2, OUTPUT);
}

void loop() {
  bool state_power = digitalRead(power);
  if (state_power) {  // verifica se o aparelho está ligado
    
  
    // get do valor lógico do potenciometro e transformando-o nas 25 temperaturas pré definidas
    int get_in = analogRead(analog_in); // get do valor do pot
    int val_pot = map(get_in, 0, 1024, 0 , 26); // map para as temp pré definidas (25)
    String get_bin = binario(val_pot, 5);
    
    bool array_final_teste[5];
    for(int i = 0; i < sizeof(array_final_teste); i++){    // Guarda o valor de uma String numa array booleana
      array_final_teste[i] = (get_bin[i] == '1');  
    }

    // chama a função complete_dabble e salva o valor em vector_out
    complete_Dabble(array_final_teste[0], array_final_teste[1], array_final_teste[2], array_final_teste[3], array_final_teste[4], vector_out);

    bool vetor_out1[4];
    bool vetor_out2[4];

    for(int vetor1=0;vetor1<4; vetor1++){
      vetor_out1[vetor1] = vector_out[vetor1];
      vetor_out2[vetor1] = vector_out[vetor1+4];
    }


    digitalWrite(clock_seg1, 0);
    digitalWrite(clock_seg2, 1);

  for(int saida=0; saida<sizeof(vetor_saida); saida++)
    digitalWrite(vetor_saida[saida], vetor_out2[saida]);
  
    delay(20);
    digitalWrite(clock_seg1, 1);
    digitalWrite(clock_seg2, 0);
  
    for(int saida=0; saida<sizeof(vetor_saida); saida++)
    digitalWrite(vetor_saida[saida], vetor_out1[saida]);
    
    
    delay(20);
  }
  else{
    Serial.println("Power off");
    delay(200);
    for(int saida=0;saida<4;saida++){
    digitalWrite(vetor_saida[saida],0);
    digitalWrite(clock_seg1, 0);
    digitalWrite(clock_seg2, 0);
    }
  }
}


void Dabble(bool A, bool B, bool C, bool D, bool vetor[4]) {               // Faz o Dabble (soma 3 ao valor após o digitos 5)
  vetor[0] = B * (C + D) + A;                       // B(C + D) + A
  vetor[1] = B * (!(C + D)) + (A * D);             // B!(C + D) + (AD)
  vetor[2] = C * (!B + D) + (A * (!D));            // C(!B + D) + (A!D)
  vetor[3] = D * (!(A + B)) + (!D) * ((B * C) + A); // D!(A + B)  + !D((BC) + A)
}


void complete_Dabble(bool A, bool B, bool C, bool D, bool E, bool vetor_out[8]){   // realiza o dabble até o valor de 5 bits de entrada
  bool array1[4];
  bool array2[4];
  Dabble(0, A, B, C, array1);
  Dabble(array1[1], array1[2], array1[3], D, array2);
  vetor_out[0] = 0;
  vetor_out[1] = 0;
  vetor_out[2] = array1[0];
  vetor_out[3] = array2[0];
  vetor_out[4] = array2[1];
  vetor_out[5] = array2[2];
  vetor_out[6] = array2[3];
  vetor_out[7] = E;
}

String binario(int valor, int numDigitos) { // transforma de decimal para binário mantendo os bits do tamanho de numDigitos
  String resultado = "";

  // Loop para calcular os bits
  while (valor > 0 || numDigitos > 0) {
    // Adiciona o bit menos significativo à esquerda da string resultado
    resultado = (valor % 2 == 0 ? "0" : "1") + resultado;

    // Desloca para a direita para verificar o próximo bit
    valor = valor / 2;

    // Reduz o número de dígitos que ainda precisam ser preenchidos
    numDigitos--;
  }

  // Preenche com zeros à esquerda, se necessário
  while (numDigitos > 0) {
    resultado = "0" + resultado;
    numDigitos--;
  }

  return resultado;
}
