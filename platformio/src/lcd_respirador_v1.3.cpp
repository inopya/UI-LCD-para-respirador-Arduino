
 /*
  #       _\|/_   A ver..., ¿que tenemos por aqui?
  #       (O-O)        
  # ---oOO-(_)-OOo---------------------------------
   
   
  ##########################################################
  # ****************************************************** #
  # *            LCD PARA REESPIRATOR 23??               * #
  # *          Autor:  Eulogio López Cayuela             * #
  # *            https://github.com/inopya/              * #
  # *       Versión v1.0      Fecha: 17/04/2020          * #
  # ****************************************************** #
  ##########################################################
*/


#include "pinout.h"
#include "definiciones.h"
#include "utiles.h"

#include "Temporizador_inopya.h" 


PANEL_CONTROL miPanelControl(true);    //true --> pullup


Temporizador_inopya relojMillis;
Temporizador_inopya relojSegundos;

void simularValoresDeSensores( void );
void medirTiempos( void );
void medirTiempos_v2( void );
void Fastitoa(unsigned int InputInteger, char* OutputString);


bool FLAG_un_segundo = true;
bool FLAG_un_milisegundo = true;





void setup() 
{
  /* Inicializar el puerto serie para DEBUG */
  Serial.begin(9600); 
  pinMode(PIN_zumbador, OUTPUT);

  Serial.println(F("version 1.3 07/05/2020"));

  pinMode(13, OUTPUT); 
  digitalWrite(13,1);
  delay(1000);
  digitalWrite(13,0);
  
  //miPanelControl.FLAG_MODO_DEBUG=true;  //activar mensajes por serial para DEBUG
}



void loop() 
{

  /*  <<<<<   RELOJES   (PARA PRUEBAS / DEBUG)    >>>>>         */

  if( relojSegundos.estado()==false ){    
    relojSegundos.begin(1000);
    simularValoresDeSensores();
  }


  if( relojMillis.estado()==false ){ 
    FLAG_un_milisegundo = true;
    relojMillis.begin(1);                 // simular una llamada cada 1 milisegundo
    miPanelControl.update(); 
  }

}




/* mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
              BLOQUE DE FUNCIONES AUXILIARES / DEMOS
mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm */


void simularValoresDeSensores()
{
  //generar valores aleatorios para mostrar en la simulacion/DEBUG
  randomSeed(analogRead(A15));
  
  
  static uint8_t peep = miPanelControl.parametrosConfiguracion[ID_MENU_PEEP].consigna;
  static uint8_t pico = miPanelControl.parametrosConfiguracion[ID_MENU_PICO].consigna;
  static uint8_t rpm = miPanelControl.parametrosConfiguracion[ID_MENU_FR].consigna;

  int a = random(-2, 3);
  peep+=a;
  miPanelControl.valorActualPeep = peep;

  a = random(-2, 3);
  pico+=a;
  miPanelControl.valorActualPico = pico;
  
  a = random(-2, 3);
  rpm+=a;
  miPanelControl.valorActualRpm = rpm; 

}

