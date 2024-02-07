#include <String.h>

#define analog_in A0
#define power 2
bool vector_out[8];

void Dabble(bool A, bool B, bool C, bool D, bool vetor[4]);
void complete_Dabble(bool A, bool B, bool C, bool D, bool E, bool vetor_out[8]);
String binario(int valor, int numDigitos);


void setup() {
  Serial.begin(9600);
  pinMode(power, INPUT);
}

void loop() {
  bool state_power = digitalRead(power);
  if (1) {  // verifica se o aparelho está ligado
    
  
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
    
    Serial.print(vector_out[0]);
    Serial.print(vector_out[1]);
    Serial.print(vector_out[2]);
    Serial.print(vector_out[3]);
    Serial.print(vector_out[4]);
    Serial.print(vector_out[5]);
    Serial.print(vector_out[6]);
    Serial.println(vector_out[7]); 
    
  }
  else{
    Serial.println("Power off");
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
