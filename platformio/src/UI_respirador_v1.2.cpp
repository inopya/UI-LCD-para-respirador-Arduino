
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


bool FLAG_un_segundo = true;
bool FLAG_un_milisegundo = true;



uint32_t inicio;
uint32_t fin;
uint32_t contador1=0;
uint32_t contador2=0;



void setup() 
{
  /* Inicializar el puerto serie para DEBUG */
  Serial.begin(9600); 

  miPanelControl.setupPantalla();
  pinMode(PIN_zumbador, OUTPUT);
  
  Serial.println(F("version 1.2 05/05/2020"));

  pinMode(13, OUTPUT); 
  digitalWrite(13,1);
  delay(1000);
  digitalWrite(13,0);

}



void loop() 
{

  /*  <<<<<   RELOJES   (PARA PRUEBAS / DEBUG)    >>>>>         */

  if( relojSegundos.estado()==false ){    
    relojSegundos.begin(3000);
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
  int a = random(-2, 3);
  miPanelControl.valorActualPeep += a;

  a = random(-2, 3);
  miPanelControl.valorActualPico +=a;
  
  a = random(-2, 3);
  miPanelControl.valorActualRpm += a; 

}

void medirTiempos()
{
  uint32_t inicio = micros();
  uint32_t contador;
  //pinMode(13,INPUT);
  //pinMode(13,OUTPUT);

  for(contador=0; contador <1000;contador++){
    //digitalWrite(13,1);
    //bitWrite(PORTB,7,1);
    //digitalRead(13);
    bitRead(PORTB,7);
  }

  uint32_t fin = micros();
  Serial.print("Inopya: ");
  Serial.print(contador);
  Serial.print(",");
  Serial.println(fin-inicio);
  contador = 0;
}

