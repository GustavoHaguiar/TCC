/*
 * Ligar energia
 * Crar conexão ap
 * Exibir pagina web
 * Receber dados de rede wifi
 * encerrar conexão ap
 * conectar-se a rede do usuario
 * caso de errado, voltar ao "criar conexão ap"
 * caso de certo: Dividir em 2 ramos principais;
 * 1 ramo: controle temperatura;
 *  ler entrada analogica
 *  fazer logica para exibir valor no display
 *  imprimir os bits do display
 * 2 ramo: controle por wifi;
 *  chuveiro está ligado? 2 opções
 *  1 opção: não está;
 *    aguardar comandos do aplicativo
 *    volta para "caso de certo"
 *  2 opção: está ligado;
 *    manter as configurações ja definidas
 *    ao desligar o cuveiro mandar relatorio ( JSON ) para o backend 
*/

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <FS.h>

AsyncWebServer server(80);

struct resAp{
  String ssid;
  String pass;
};



void setup() {
  

}

void loop() {
  

}
