
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


#include "definiciones.h"



/* 
    no cambiar estos pines los uso con acceso 
    a los bit del puerto al que pertenecen para un acceso mas rapido
    tanto en el encoder  y pulsadores como en el LCD

    leo las entradas de pulsadores con:  ~PINC&B0100111 (en mi caso los pulsadores con pullup)
    y las entradas del encoder con bitRead(PUERTO_PIN_ENCODER, bitEncoderCLK)  (tambien pullup)

    Recuerda, si has de cambiar alguno de estos pines deberas hacer tambien en las partes
    en als que se leen o escriben 


    <<<< Lugares afectados por el uso de pines >>>>
    -----------------------------------------------
** para los pines de los pulsadores incluido el del encoder:
    1) PANEL_CONTROL::PANEL_CONTROL
    2) uint8_t PANEL_CONTROL::leerPulsadores() 

** para las lineas data y clock del encoder:
   >> int8_t PANEL_CONTROL::leerEncoder()   

** para los pines del lcd paralelo en modo solo 4bits (ojo);
   >>LCD_CON_BUFFER::lcdRawWrite( char cmd, uint8_t cmdtype )      

*/



#define PIN_zumbador       10

// pines del lcd 

#define PIN_D4             22   // PA0  bitWrite(PORTA,0,...)
#define PIN_D5             23   // PA1  bitWrite(PORTA,1,...)
#define PIN_D6             24   // PA2  bitWrite(PORTA,2,...)
#define PIN_D7             25   // PA3  bitWrite(PORTA,3,...)

#define PIN_E               8   // PH5  bitWrite(PORTH,5,...)
#define PIN_RS              9   // PH6  bitWrite(PORTH,6,...)

// pines del enoder y los pulsadores

#define PIN_alarm          32   // el pulsador del encoder 
#define PIN_DT             33   // linea data del encoder
#define PIN_CLK            34   // linea clock del encoder

#define PIN_peep           35   // pulsador  peep
#define PIN_pico           36   // pulsador  pico
#define PIN_rpm            37   // pulsador  rpm

//---------------------------------------------------------------------
//Todo esto hay que mejorarlo para hacerlo mas sencillo
#define PUERTO_LCD_DATA                 PORTA
#define PUERTO_LCD_CONTROL              PORTH
#define BIT_RS                          6   //El bit de PUERTO_LCD_CONTROL correspondiente al pin RS
#define BIT_E                           5   //El bit de PUERTO_LCD_CONTROL correspondiente al pin E

#define PUERTO_PIN_PULSADOR_ENCODER     PINC
#define PUERTO_PIN_ENCODER              PINC
#define bitEncoderPULSADOR              5
#define bitEncoderDT                    4
#define bitEncoderCLK                   3
#define bitPulsadorPEEP                 2
#define bitPulsadorPICO                 1
#define bitPulsadorRPM                  0


// 30,31,32,33,34,35,36,37    <-- Pines nomenclatura Arduino
//  7, 6, 5, 4, 3, 2, 1, 0    <-- Bits del registro del puerto C,    PORTC

// 29,28,27,26,25,24,23,22    <-- Pines nomenclatura Arduino
//  7, 6, 5, 4, 3, 2, 1, 0    <-- Bits del registro del puerto A,    PORTA


// NOTA(para mi): clock, mi cable rojo, data el marron


