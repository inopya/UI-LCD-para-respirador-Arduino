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


#define LIMITE_ENCODER        100  //sin  uso por ahora

#define PULSACION_CORTA        50   // 50 ms necesarios para reconocer una pulsacion CORTA
                                    // evitando posibles rebotes.
#define PULSACION_LARGA       450   // 450 ms para reconocer una pulsacion LARGA
                                    // Los pulsadores devuelve 0,1 o 2 segun esten en reposo
                                    // se producza una pulsacion corta o una larga respectivamente
                                    // aunque por ahora no estoy haciedo uso de esta distincion
                                    // ya que las pulsaciones tipo 2 despreciamos en:
                                    // PANEL_CONTROL::parpadeoMenuAlPulsar()

#define DURACION_PULSACION_CORTA     80  //sin  uso por ahora
#define DURACION_PULSACION_LARGA    280  //sin  uso por ahora


#define ALARMA_PEEP_ALTO        1
#define ALARMA_PEEP_BAJO        2
#define ALARMA_PICO_ALTO        3
#define ALARMA_PICO_BAJO        4
#define ALARMA_RPM_ALTO         5
#define ALARMA_RPM_BAJO         6


#define getBit(data,y)      (data>>y) & 1           // Obtener el valor  del bit (data.y)

// #define setBit(data,y)       data |= (1 << y)          // Poner a 1 el bit (data.y) 
// #define clearBit(data,y)     data &= ~(1 << y)         // Poner a 0 el bit (data.y)
// #define togleBit(data,y)     data ^=getBit(y)          // Invertir el valor del bit (data.y)
// #define togleByte(data)      data =~data               // Invertir el valor del byte (data)

//asignacion numerica a los modos  (CAMBIADO POR UN 'emun')
/*
#define MODE_STOP               0  // parada de la maquina por fin de uso
#define MODE_SETUP_LCD          1  // setup inicial que incluye solo al LCD
#define MODE_WAIT_TO_CONFIG     2  // setup inicial que incluye solo al LCD
#define MODE_CONFIG_RESPIRATOR  3  // parametros iniciales del respirador, sexo, altura, limites pico...
#define MODE_WAIT_TO_RUN        4  // pantalla de espera entre configurar y modo run
#define MODE_RUN                5  // modo normal de funcionamiento
#define MODE_EDICION_CONSIGNAS  6  // modo normal de funcionamiento
#define MODE_VER_OTROS          7  // modo normal de funcionamiento
#define MODE_REVISION_ALARMAS   8  // modo normal de funcionamiento

#define MODE_PAUSE              99  // ¿ será necesria para algun tipo  de mantenimiento ?
#define MODE_ERROR              100 // ¿ algun tipo de parada crítica ?

*/