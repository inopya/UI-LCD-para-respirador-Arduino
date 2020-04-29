
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

/* mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
  --------------------------------------------------------------------------------
       *******    IMPORTACION DE LIBRERIAS Y DEFINICION DE CLASES    *******
  --------------------------------------------------------------------------------
  mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/


#include "Arduino.h"

#include "definiciones.h"

#include  <LiquidCrystal.h>
#include  "TimerOne.h"
#include <avr/pgmspace.h> 


class PANEL_CONTROL
{

  public:
    //LiquidCrystal lcd;

    // PANEL_CONTROL( uint8_t pin1, uint8_t pin2, uint8_t pin3, 
    //                uint8_t pin4, uint8_t pin5, uint8_t pin6, bool pullup);
    PANEL_CONTROL( bool pullup);

    void update( void );
    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    //  publico      PANTALLA    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

    uint8_t listaDeAlarmas [250];   //



    char msg_peep[6] ="PEEP ";  //estos deberian estar en PROGMEM, pero me dan problemas...
    char msg_pico[6] ="PICO ";  //estos deberian estar en PROGMEM, pero me dan problemas...
    char msg_rpm[6]  ="RPM  ";  //estos deberian estar en PROGMEM, pero me dan problemas...
    char msg_alarm[6]="ALARM";  //estos deberian estar en PROGMEM, pero me dan problemas...
    char erase_msg[6]="     ";  //estos deberian estar en PROGMEM, pero me dan problemas...

    char msg_valorItoA[6]="00000";   //este debe ser mutable
    
    uint8_t consignaPEEP = 80;    // 
    uint8_t consignaPICO = 70;    //  } estos valores deberan venir de alguna parte
    uint8_t consignaRPM =  30;    //

    uint8_t consignaPEEPminima = 60;    // 
    uint8_t consignaPICOminima = 50;    //  } estos valores deberan venir de alguna parte
    uint8_t  consignaRPMminima =  20;    //

    uint8_t consignaPEEPmaxima = 100;    // 
    uint8_t consignaPICOmaxima = 95;    //  } estos valores deberan venir de alguna parte
    uint8_t  consignaRPMmaxima =  35;    //

    uint8_t contadorALARMAS =  10;         // cargado con un numero de alarmas para DEBUG
    uint32_t _contadorTicksReloj = 0;      // lleva la cuenta de las veves que somos llamados (reloj interno)
                                           // de la clase
    bool FLAG_MODO_DEBUG=false;            // permite los 'Serial.print' repartidos por el codigo para prueba
    bool FLAG_MOSTAR_ALARMAS = false;      // control del boton del encoder en modo mostrar alarmas


    void setupPantalla( void );                       // inicializa y muestra mensaje de bienvenida
    void pantallaPrint(uint8_t x, uint8_t y,  const char* ); // engloba posicionar el cursor e imprimir
    //void pantallaPrint2(uint8_t x, uint8_t y, const char*[]);  //  Sin uso, 
    void ultraitoa( int valor ,  char *salida );
    void mostrarLineaConsignas( void );
    void mostrarListadoAlarmas( void );
    void mostrarValorEncoder( void );
    void actualizarContadorAlarmas( void );
    void mostrarLineaMenus( void );

    void mostrarLineaCabecera( void );
    void mostrarLineaAlarma( void );

    void mostarEdicionConsignas( void );   //sin uso, por si separo una aprte del codigo??
    //int Menu( char *menuitems[] , int items );
    void mostrarMensajeAlarma (const char * const * message);

    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    //     publico   ENCODER    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

    int8_t leerEncoder( void );           // retorna el estado del pulsador  -1,0,1
                                          // giro Izquierda, Reposo,  giro Derecha


    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    //   publico   PULSADORES    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

    void updatePulsadores( void );
    uint8_t get( void );
    uint8_t estadoPulsadores = 0;   // aqui se almacena el estado para que lo vea el programa
    uint8_t tipoPulsacion = 0;      // indica si es pulsacion CORTA o LARGA
    


  private:
    bool _FLAG_PULLUP;  
    //void accionTeclado( uint8_t estadoPulsadores, uint8_t tipoPulsacion );


    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    //   privado   ENCODER   //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

    uint8_t _sentidoGiroEncoder = 0;       // almacena el sentido de giro
    uint8_t _valorEncoder = 0;             //el valor de consigna que se esta modificando
    uint8_t _valorEncoderMax = 0; 
    uint8_t _valorEncoderMin = 0; 
    void modificarConsigna();

    int16_t _valorAnteriorEncoder = -1;     // para controlar si cambia el valor y asi ahorrar refrescos de lcd
                                            // int8_t si vamos a tener menos de 127 alarmas. En pruebas y dado que las alarmas se generan
                                            // aleatoriamente, mejor asi para evitpasarnos y tener problemas de refresco
    void asignarCosignaYterminarEdicion( void );
    
    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    //   privado  PULSADORES  //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

    uint8_t _estadoActualPulsadores;      // indica la lectura real del pulsador
    uint8_t _estadoAnteriorPulsadores;    // informa de la lectura anterior del pulsador
    uint16_t _contadorInternoPulsador;    // cuenta 'ticks' para distinguir REBOTES, pulsaciones CORTAS y LARGAS
    bool FLAG_beep = false;               // controla el beep que indica pulsacion
    bool FLAG_parpadeo_edicion = false;   // controla el parpadeo al entrar en modo edicion
    bool FLAG_MODO_EDICION = false;       // SIN USO ( indica que estamos en modo edicion )
    uint8_t leerPulsadores( void );       // retorna el tipo de pulsacion  0,1,2 y lo asigna a 'tipoPulsacion'  
    void beep( uint16_t );                // genera el beep del pulsador al terminar pulsacion
    void reset( void );                   // restablece el pulsador tras una pulsacion 
    uint8_t _pulsacionAnterior=0;         // ultima pulsacion util y que se esta procesando

    uint16_t _duracion_parpadeo=500;      // ticks, (o sea ms) que dura 'medio' parpadeo 
                                          // es deir tiempo que se muestra y tiempo que se oculta

    uint8_t contador_parpadeos;           // lleva la cuenta de los parpadeos del menu en modo edicion
                                          // es accesible accesible para el encoder
                                          // de forma que lo va 'reseteando' para que 
                                          //el aprpadeo se mantenga mientras estemos manipulando

    void parpadeoMenuAlPulsar();          // Estamos editando alguna consigna o revisando alarmas

    const uint8_t numeroMaximoParpadeos = 20;  // 'tiempo' que se mantiene ativo el modo edicion
                                               // aunque no se esté manipulando el encoder o
                                               // los pulsadores. Pasado ese tiempo si no se 
                                               // acepta la consigna, se cancela  el proceso 
                                               // y se dejan los valores originales.
                                               // Recuerda que el tiempo será este numero  por 
                                               // lo que dura un parpadeo: '_duracion_parpadeo'
  
  uint8_t readPin( void );
  
};





//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//              ENUMERADORES    sin uso por ahora
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm


enum enumEstadosDispay
{
    STARTUP,
    RUN,
    EDIT,
    IDLE
};

enum pinType
{
   pullup,
   pulldown
};


enum alarmCode
{
   PEEP_BAJO, PEEP_ALTO, PICO_BAJO, PICO_ALTO, RPM_BAJO, RPM_ALTO

};
