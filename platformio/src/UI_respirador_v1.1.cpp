

#include "pinout.h"
#include "definiciones.h"
#include "utiles.h"

#include "Temporizador_inopya.h" 


PANEL_CONTROL miPanelControl(PIN_peep, PIN_pico, PIN_rpm, PIN_CLK, PIN_DT, PIN_alarm, true);      //pullup


Temporizador_inopya relojMillis;
Temporizador_inopya relojSegundos;

void simularValoresDeSensores( void );

bool FLAG_un_segundo = true;
bool FLAG_un_milisegundo = true;



void setup() 
{
  /* Inicializar el puerto serie para DEBUG */
  Serial.begin(9600); 

  miPanelControl.setupPantalla();
  pinMode(PIN_zumbador, OUTPUT);
  
  Serial.println(F("version 1.1"));

  pinMode(13, OUTPUT); 

}



void loop() 
{

  /*       <<<<<     RELOJES   PARA PRUEBAS/DEBUG    >>>>>                      */

  if( relojSegundos.estado()==false ){    
    FLAG_un_segundo = true;               // simular una llamada cada 1 segundo
    relojSegundos.begin(1000);
  }

  if( relojMillis.estado()==false ){ 
    FLAG_un_milisegundo = true;
    relojMillis.begin(1);                 // simular una llamada cada 1 milisegundo
  }



 if( FLAG_un_milisegundo ){
     FLAG_un_milisegundo = false;
    miPanelControl.update(); 
 }

  if( FLAG_un_segundo ){
    FLAG_un_segundo = false;
    simularValoresDeSensores();
  }
}




/* mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
              BLOQUE DE FUNCIONES AUXILIARES / DEMOS
mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm */


void simularValoresDeSensores()
  //generar valores aleatorios para mostrar en la simulacion/DEBUG
{

  static uint8_t valorPeep = 0;
  static uint8_t valorPico = 0;
  static uint8_t valorRpm = 0;

  static uint8_t valorActualPeep = miPanelControl.consignaPEEP;
  static uint8_t valorActualPico = miPanelControl.consignaPICO;
  static uint8_t valorActualRpm = miPanelControl.consignaRPM;

  //randomSeed(analogRead(A15));
  int a = random(-2, 3);

  valorActualPeep += a;

  a = random(-2, 3);
  valorActualPico +=a;
  
  a = random(-2, 3);
  valorActualRpm += a; 

  if (valorActualPeep^valorPeep){
    miPanelControl.ultraitoa(valorActualPeep, miPanelControl.msg_valorItoA);
    miPanelControl.pantallaPrint(0,2, miPanelControl.msg_valorItoA);
  }
  if (valorActualPico^valorPico){
    miPanelControl.ultraitoa(valorActualPico, miPanelControl.msg_valorItoA);
    miPanelControl.pantallaPrint(5,2, miPanelControl.msg_valorItoA);
  }
  if(valorActualRpm^valorRpm ){
    miPanelControl.ultraitoa(valorActualRpm, miPanelControl.msg_valorItoA);
    miPanelControl.pantallaPrint(10,2, miPanelControl.msg_valorItoA);
  }

  valorPeep = valorActualPeep;
  valorPico = valorActualPico;
  valorRpm = valorActualRpm;

  if( (valorActualPeep<miPanelControl.consignaPEEP-20) || 
                              (valorActualPeep>miPanelControl.consignaPEEP+20) ){
    miPanelControl.contadorALARMAS ++;
    valorActualPeep = miPanelControl.consignaPEEP;
  }
  if( (valorActualPico<miPanelControl.consignaPICO-20) || 
                              (valorActualPico>miPanelControl.consignaPICO+20) ){
    miPanelControl.contadorALARMAS ++;
    valorActualPico = miPanelControl.consignaPICO;
  }
  if( (valorActualRpm<miPanelControl.consignaRPM-20) || 
                              (valorActualRpm>miPanelControl.consignaRPM+20) ){
    miPanelControl.contadorALARMAS ++;
    valorActualRpm = miPanelControl.consignaRPM;
  }
}
