
#include "definiciones.h"


//leo las entradas de pulsadores con:  ~PINC&B0100111 (en mi caso los pulsadores con pullup)
// y las entradas del encoder con bitRead(PUERTO_PIN_ENCODER, bitEncoderCLK)  (tambien pullup)

//Tengo que pulir el tema de la lectura de las entradas porque no se 
//si al final estará todo en un puerto o no, por ahora usar estos pines...


#define PIN_zumbador       10

#define PIN_alarm          32   //el pulsador del encoder 
#define PIN_DT             33
#define PIN_CLK            34

#define PIN_peep           35   //
#define PIN_pico           36   // 
#define PIN_rpm            37   //


#define PUERTO_PIN_PULSADOR_ENCODER     PINC
#define PUERTO_PIN_ENCODER              PINC
#define bitEncoderPULSADOR              5
#define bitEncoderDT                    4
#define bitEncoderCLK                   3
#define bitPulsadorPEEP                 2
#define bitPulsadorPICO                 1
#define bitPulsadorRPM                  0

// 30,31,32,33,34,35,36,37    <-- Pines nomenclatura Arduino
//  7, 6, 5, 4, 3, 2, 1, 0    <-- Bits del registro del puerto C,    PORC

// NOTA: clock, mi cable rojo, data el marron

