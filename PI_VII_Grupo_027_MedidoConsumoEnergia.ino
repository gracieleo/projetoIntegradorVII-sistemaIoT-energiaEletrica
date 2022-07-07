#include <Ethernet.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <SPI.h>
#include "EmonLib.h"

#define VOLT_CAL 1120.99

int pino_sct = A1;
int pino_ZMPT = A2;

EnergyMonitor emon1;

int leitura;

float leituraconvertida;
float leituratensao;
float leiturapotencia;

char sentenca[128];
char CORRENTE[10];
char TENSAO[10];
char POTENCIA[10];

byte mac_addr[]     = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress server_addr(185, 42, 117, 115);      // O IP DO SERVIDOR DA CLEVER CLOUD
char user[]         = "ue2qhguh1a2y";          // Usuario MySQL
char password[]     = "9L3PKc1QON0elc";        // Senha MySQL  
char ssid[]         = "Rede_WIFI";             //  Nome de rede Wifi   
char pass[]         = "Senha_WIFI";            //  Senha Wi-Fi 

char INSERT_DATA[]  = "INSERT INTO ENERGIA (EQUIPAMENTO, CORRENTE, TENSAO, POTENCIA) VALUES ('%s', %s, %s, %s)";

char BANCODEDADOS[] = "USE blbg7qzdjryay";

EthernetClient client;
MySQL_Connection conn((Client *)&client);
void setup()
{
   emon1.voltage(2, VOLT_CAL, 1.7);
   emon1.current(pino_sct, 1.840);
   Serial.println(INSERT_DATA);
   Serial.begin(115200);
   while (!Serial); 
   Ethernet.begin(mac_addr);
   Serial.println("Conectando...");
   if (conn.connect(server_addr, 3306, user, password)) 
   {
      delay(1000);
      
      MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
      cur_mem->execute(BANCODEDADOS);
      delete cur_mem;
   }
   else
   {
      Serial.println("A conexão falhou");
      conn.close();
   }
}
void loop()
{
   emon1.calcVI(100,2000);  
   float supplyVoltage = emon1.Vrms;
   //Calcula a corrente  
   double Irms = emon1.calcIrms(1480);
   Serial.println("Executando sentença");
   leitura = analogRead(pino_sct);
   leituraconvertida = (Irms);
   leituratensao = supplyVoltage;
   leiturapotencia = (leituratensao)*(leituraconvertida);
   
   dtostrf(leituraconvertida, 4, 1, CORRENTE);
   dtostrf(leituratensao, 4, 1, TENSAO);
   dtostrf(leiturapotencia, 4, 1, POTENCIA);
   
   sprintf(sentenca, INSERT_DATA, "DESKTOP", CORRENTE, TENSAO, POTENCIA);
   
   MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
   cur_mem->execute(sentenca);
   delete cur_mem;
   delay(20000);
}
