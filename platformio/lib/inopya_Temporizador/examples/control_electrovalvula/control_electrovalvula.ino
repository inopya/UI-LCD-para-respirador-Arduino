/*
  #       _\|/_   A ver..., ¿que tenemos por aqui?
  #       (O-O)        
  # ---oOO-(_)-OOo---------------------------------
   
   
  ##########################################################
  # ****************************************************** #
  # *            DOMOTICA PARA AFICIONADOS               * #
  # *    Control Electrovalvula con apertura diaria      * #
  # *          Autor:  Eulogio López Cayuela             * #
  # *                                                    * #
  # *       Versión v1.0      Fecha: 07/03/2020          * #
  # ****************************************************** #
  ##########################################################
*/

#define __VERSION__ "Ejemplo ontrol Electrovalvula\n"

/*
  
 ===== NOTAS DE LA VERSION =====
 

  1.- Control de electrovalvula apra evacuacion de deposito de aguas residuales
	  El vaciado se realizará una vez cada 24 horas pero tambien disponderemos de un pulsador
	  para vaciado manual, que no afectara a los ciclos diarios.
      Los controles de tiempo no son 100% exactos y ya que no son extremadamente importantes 
      se implementan mediante timers virtuales basados en la funcion millis().
      Se utilizan emporizadores virtuales de a clase Temporizador_inopya:
      para crear un temporizador:
                                  Temporizador_inopya nombreTemporizador;
      Los temporizadores virtuales disponen de dos metodos: begin() y estado()
      
      Una vez definido un temporizador se usa de la siguiente forma:
      nombreTemporizador.begin(tiempoEnMilisegundos) --> carga tiempo en el temporizador y lo inicia.
      nombreTemporizador.estado() --> retorna un valor booleano: 
                                                                 true = temporizador aun activo; 
                                                                 false = temporizador terminado
                                                                 
    ** Compilado con Arduino _IDE 1.8.5 windows **     
     Este Sketch usa 1452 bytes, el (4%) del espacio de FLASH ROM
     Las variables Globales usan 82 bytes, el (4%)  del espacio de RAM
 
      
  CONEXIONES:

 =======================
  ARDUINO     RELE 1
 =======================
   D11  --->   DATA  
   GND  --->   GND
   5v   --->   Vcc



 =======================
  ARDUINO     RELE 2
 =======================
   D12  --->   DATA  
   GND  --->   GND
   5v   --->   Vcc


 =======================
  ARDUINO     PULSADOR
 =======================
  5v    --->   PIN1  
  D2    --->   PIN2


 =======================
     RESISTENCIA 1MH
 =======================
  D2 <--- R  --->   GND  

*/ 



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        IMPORTACION DE LIBRERIAS 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/
#include <Temporizador_inopya.h>


/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        SECCION DE DECLARACION DE CONSTANTES  Y  VARIABLES GLOBALES
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//------------------------------------------------------
// Definiciones para pines y variables
//------------------------------------------------------

#define RELE_ABRIR       11
#define RELE_CERRAR      12
#define PULSADOR       	  2  	//usando el pulsador en pin2, podremos usar interrupciones en el futuro si se desea

#define LED_ONBOARD      13

#define ENCENDER        LOW   	//ojo, mis reles se activan con 0 (LOW)
#define APAGAR         HIGH   	//ojo, el rele se desactiva con 1 (HIGH) 


//tiempos reducidos a unos pocos segundos a efectos de pruebas, usar los que estan entre () 
//para un ciclo diario con 25 minutos de tiempo de vaciado y 40s de tiempo de reaccion de la valvula
#define DURACION_DIA             300000  // (86400000) // 24 horas en milisegundos
#define DURACION_VACIADO          60000  //  (1500000) // 25 minutos en milisegundos =     25 * 60 * 1000 
#define TIEMPO_REACCION_VALVULA   10000  //    (40000) // 40s tiempo necesario para operar la valvula


boolean FLAG_estado_led  = false;



//------------------------------------------------------------------------
// Creacion de objetos del tipo Temporizador_inopya, 
// uno para cada una de las tareas
//------------------------------------------------------------------------

/* 
 * Los temporizadores virtuales disponen de dos metodos: begin() y estado()
 * 
 *  nombreTemporizador.begin(tiempoEnMilisegundos) --> carga tiempo en el temporizador y lo inicia.
 *  nombreTemporizador.estado() --> retorna un valor booleano: 
 *                                                              true = temporizador aun activo; 
 *                                                              false = temporizador terminado
 */
 
Temporizador_inopya reloj24h;                 	//controla los ciclos diarios
Temporizador_inopya tiempoOperacionValvula;   	//controla el tiempo necesario para abrir o cerrar la valvula
Temporizador_inopya tiempoVaciado;			 	//tiempo de desagüe del deposito

Temporizador_inopya parpadeoLED;		    	//por el capricho de tener un led parpadeando


//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//***************************************************************************************************
//         FUNCION DE CONFIGURACION
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 

void setup() 
{
  pinMode(RELE_ABRIR, OUTPUT);
  pinMode(RELE_CERRAR, OUTPUT);
  pinMode(PULSADOR, INPUT);
  pinMode(LED_ONBOARD, OUTPUT);
  
  digitalWrite(RELE_ABRIR, LOW);
  digitalWrite(RELE_CERRAR, LOW);  
}



//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//***************************************************************************************************
//  BUCLE PRINCIPAL DEL PROGRAMA   (SISTEMA VEGETATIVO)
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 


void loop() 
{
  /* Control de apertura por temporizador */
  if(reloj24h.estado()==false){
    reloj24h.begin(DURACION_DIA);
	
    digitalWrite(LED_ONBOARD, HIGH);		//mantener encendido el led D13 de la placa
    vaciarDeposito();
  }   
  
  /* Control de apertura manual */
  if (digitalRead(PULSADOR) == true){
   
    digitalWrite(LED_ONBOARD, HIGH);		//mantener encendido el led D13 de la placa
    vaciarDeposito(); 
  }

  /* Mantener un parpadeo del led onboard mientras el sistema esta inactivo */
  if(parpadeoLED.estado()==false){
    FLAG_estado_led = !FLAG_estado_led;
    parpadeoLED.begin(500);
    digitalWrite(LED_ONBOARD, FLAG_estado_led);
  }
}



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
   ###################################################################################################### 
                                  BLOQUE DE FUNCIONES AUXILIARES
   ###################################################################################################### 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//  VACIADO DEL DEPOSITO
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

void vaciarDeposito()
{  
  /* Secuencia de apertura */
  digitalWrite(RELE_CERRAR, APAGAR);    
  digitalWrite(RELE_ABRIR, ENCENDER); 
  
  /* Tiempo necesario para abrir la valvula (ajustar segun sea necesario) */
  tiempoOperacionValvula.begin(TIEMPO_REACCION_VALVULA); //tiempo en milisegundos
  while(tiempoOperacionValvula.estado()==true){
    //nada, o cualquier tarea queramos ejecutar durante este 'tiempo muerto'
  }
  
  /* Quitamos la alimentacion de ambos polos de la valvula */                            
  digitalWrite(RELE_CERRAR, APAGAR);
  digitalWrite(RELE_ABRIR, APAGAR);
  
  /* Tiempo que mantenemos abierta la valvula */ 
  tiempoVaciado.begin(DURACION_VACIADO);  //tiempo en milisegundos
  while(tiempoVaciado.estado()==true){
    //nada, o cualquier tarea queramos ejecutar durante este 'tiempo muerto'
  }
   
  /* Secuencia de cierre */
  digitalWrite(RELE_CERRAR, ENCENDER); 
  digitalWrite(RELE_ABRIR, APAGAR);       
  
  /* Tiempo necesario para cerrar la valvula (ajustar segun sea necesario) */
  tiempoOperacionValvula.begin(TIEMPO_REACCION_VALVULA); //tiempo en milisegundos
  while(tiempoOperacionValvula.estado()==true){
    //nada, o cualquier tarea queramos ejecutar durante este 'tiempo muerto'
  }
                              
  /* Quitamos la alimentacion de ambos polos de la valvula */                             
  digitalWrite(RELE_CERRAR, APAGAR);
  digitalWrite(RELE_ABRIR, APAGAR); 
}



//*******************************************************
//                    FIN DE PROGRAMA
//*******************************************************
