
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
  
  Serial.println(F("version 1.1"));

  pinMode(13, OUTPUT); 
  digitalWrite(13,1);
  delay(1000);
  digitalWrite(13,0);

}



void loop() 
{

  /*       <<<<<     RELOJES   PARA PRUEBAS/DEBUG    >>>>>                      */

  if( relojSegundos.estado()==false ){    
    relojSegundos.begin(3000);
    simularValoresDeSensores();
  }


  if( relojMillis.estado()==false ){ 
    FLAG_un_milisegundo = true;
    relojMillis.begin(1);                 // simular una llamada cada 1 milisegundo
    miPanelControl.update(); 
  }


  /*       <<<<<     TAREAS ASOCIADAS A LOS FLAGs DE LOS RELOJES  (PRUEBAS/DEBUG)  >>>>>                      */
  if( FLAG_un_milisegundo ){
    FLAG_un_milisegundo = false;
    //uint32_t inicio = micros();
    //miPanelControl.update(); 
    //uint32_t fin = micros();
    //uint32_t tiempo_empleado = (fin-inicio);
    //Serial.println(tiempo_empleado);

    //if(tiempo_empleado>120){ digitalWrite(13,HIGH);Serial.println(tiempo_empleado); }
    //else{ digitalWrite(13,LOW); }

     contador1++;
    // inicio =micros();
    // miPanelControl.update();
    // fin=micros();

    // if(fin-inicio>=120){
    //   Serial.print("Inopya: ");
    //   Serial.print(contador1);
    //   Serial.print(",");
    //   Serial.println(fin-inicio);
    //   contador1 = 0;
    //   delay(1500);
    // }
  }



  if (contador1 == 10000){
    //medirTiempos();
    contador1=0;
  }


  /* benchMark */
  //uint32_t inicio = micros();
  //miPanelControl.update(); 
  //for(uint32_t i=0;i<10000;i++){
  //  digitalWrite(10,1);
    //bitWrite(PORTB,7,0);
    //setBit(PORTB,7);
    //clearBit(PORTB,7);
    //setBit(PORTB,4);  //altavoz
  //}
  
  // uint32_t fin = micros();
  // uint32_t tiempo_empleado = (fin-inicio);
  // Serial.println(tiempo_empleado);
  // clearBit(PORTB,4);
  // while(true);


  // delay(1);
  // if(tiempo_empleado>100){
  //   digitalWrite(13,HIGH);
  // }
  // else{
  //   digitalWrite(13,LOW);
  // }

  //delay(100);
}




/* mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
              BLOQUE DE FUNCIONES AUXILIARES / DEMOS
mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm */


void simularValoresDeSensores()
  //generar valores aleatorios para mostrar en la simulacion/DEBUG
{

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

