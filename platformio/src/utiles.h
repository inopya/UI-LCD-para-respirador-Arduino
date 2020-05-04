
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
#include <avr/pgmspace.h> 


class PANEL_CONTROL
{

  public:

    PANEL_CONTROL( bool pullup);

    void update( void );
    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    //  publico      PANTALLA    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

    uint8_t listaDeAlarmas [30];


    typedef struct {
      char textoAlarma [21];
    } mensaje_alarma_Type;


  // //static const mensaje_alarma_Type mensajes_alarmas [10] PROGMEM = 
  // static const char mensajes_alarmas [10][20] PROGMEM = 
  // { 
  //   {"PEEP ALTO"}, 
  //   {"PEEP BAJO"}, 
  //   {"PICO ALTO"}, 
  //   {"PICO BAJO"}, 
  //   {"RPM ALTO"}, 
  //   {"RPM BAJO"}, 
  //   {"CONEXION PERDIDA"}, 
  //   {"DATOS NO VALIDOS"}, 
  //   {"BATERIA BAJA"}, 
  //   {"FALLO ALIMENTACION"}, 
  // };

    const char alarma0000 [] PROGMEM = "PEEP ALTO"; 
    const char alarma0001 [] PROGMEM = "PEEP BAJO"; 
    const char alarma0002 [] PROGMEM = "PICO ALTO"; 
    const char alarma0003 [] PROGMEM = "PICO BAJO"; 
    const char alarma0004 [] PROGMEM = "FRALTA"; 
    const char alarma0005 [] PROGMEM = "FR BAJA"; 
    const char alarma0006 [] PROGMEM = "CONEXION PERDIDA"; 
    const char alarma0007 [] PROGMEM = "DATOS NO VALIDOS"; 
    const char alarma0008 [] PROGMEM = "BATERIA BAJA"; 
    const char alarma0009 [] PROGMEM = "FALLO ALIMENTACION"; 

    // const char * const mensajes_alarmas[10] PROGMEM = 
    //     { alarma0000, alarma0001, alarma0002, alarma0003, alarma0004, 
    //  	   alarma0005, alarma0006, alarma0007, alarma0008, alarma0009, };

    PGM_P const mensajes_alarmas[10] PROGMEM = 
    //const char * const mensajes_alarmas[10] PROGMEM =
        { alarma0000, alarma0001, alarma0002, alarma0003, alarma0004, 
        alarma0005, alarma0006, alarma0007, alarma0008, alarma0009, };

    void mostrarMensajeAlarma (uint8_t);

    uint8_t bufferFreeSpacePublic;

    // char msg_peep[6] ="PEEP ";  //estos deberian estar en PROGMEM, pero me dan problemas...
    // char msg_pico[6] ="PICO ";  //estos deberian estar en PROGMEM, pero me dan problemas...
    // char msg_rpm[6]  =" FR  ";  //estos deberian estar en PROGMEM, pero me dan problemas...
    // char msg_alarm[6]="ALARM";  //estos deberian estar en PROGMEM, pero me dan problemas...
    // char erase_msg[6]="     ";  //estos deberian estar en PROGMEM, pero me dan problemas...

    char lista_menus[8][6] =
    {
      "     ", "PEEP ", "PICO ", " FR  ", 
      "ALARM", "TIDAL", "TRIGG", "RAMPA",
    };
    uint8_t indiceMenuSecundario = 4;  //por defecto ALARM
    //void editarOpcionesSecundarias( void );

    bool FLAG_alarmaPEEP = false;
    bool FLAG_alarmaPICO = false;
    bool FLAG_alarmaFR = false;


    char msg_valorItoA[6]="00000";   //este debe ser mutable, sobre el se escriben todos los resultados
  
    uint8_t consignaTIDALminimo = 30;
    uint8_t consignaTIDALmaximo = 50;    //asignados arbitrariamente para pruebas
    uint8_t consignaTIDAL = 40;
    uint8_t volumenTidalactual;

    uint8_t consignaTRIGGERminimo = 20;
    uint8_t consignaTRIGGERmaximo = 40;    //asignados arbitrariamente para pruebas
    uint8_t consignaTRIGGER = 30;
    uint8_t valorTRIGGERactual;

    uint8_t consignaRAMPAminimo = 40;
    uint8_t consignaRAMPAmaximo = 60;      //asignados arbitrariamente para pruebas
    uint8_t consignaRAMPA = 50;
    uint8_t valorRAMPAactual;

    uint8_t valorActualPeep=0;
    uint8_t valorAnteriorPeep=0;
    uint8_t consignaPEEP = 80;
    uint8_t consignaPEEPminima = 60;
    uint8_t consignaPEEPmaxima = 100;


    uint8_t valorActualPico=0;
    uint8_t valorAnteriorPico=0;
    uint8_t consignaPICO = 70;
    uint8_t consignaPICOminima = 50;
    uint8_t consignaPICOmaxima = 95;

    uint8_t valorAnteriorRpm=0;
    uint8_t valorActualRpm=0;
    uint8_t consignaRPM =  30;    //
    uint8_t  consignaRPMminima =  20;
    uint8_t  consignaRPMmaxima =  35; 
  
    uint16_t timeOutMenuSecundario = 0;  //para volver ala opcion predeterminada:ALARM

    uint8_t contadorALARMAS =  10;         // cargado con un numero de alarmas para DEBUG
    uint32_t _contadorTicksReloj = 0;      // lleva la cuenta de las veves que somos llamados (reloj interno)
                                           // de la clase
    bool FLAG_MODO_DEBUG=false;            // permite los 'Serial.print' repartidos por el codigo para prueba

    void setupPantalla( void );                       // inicializa y muestra mensaje de bienvenida
    void pantallaPrint(uint8_t x, uint8_t y,  const char* ); // engloba posicionar el cursor e imprimir
    void ultraitoa( int valor ,  char *salida );

    void mostrarListadoAlarmas( void );
    void mostrarValorEncoder( void );
    void actualizarContadorAlarmas( void );
    void mostrarValoresSecundarios( void );
    void recorrerAlarmas( void );

    void mostrarLineaCabecera( void );
    void mostrarLineaAlarma( void );
    void mostrarLineaConsignas( void );
    void mostrarLineaValoresActuales( void );
    void mostrarLineaMenus( void );
    void mostrarOpcionMenuSecundario( void );

    void mostarEdicionConsignas( void );   //sin uso, por si separo una parte del codigo??
    void mostrarMensajeAlarma (const char * const * message);

    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    //     publico   ENCODER    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

    int8_t leerEncoder( void );           // retorna el estado del pulsador  -1,0,1
                                          // giro Izquierda, Reposo,  giro Derecha


    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    //   publico   PULSADORES    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

    void updatePulsadores( void );
    uint8_t estadoPulsadores = 0;   // aqui se almacena el estado para que lo vea el programa
    uint8_t tipoPulsacion = 0;      // indica si es pulsacion CORTA o LARGA
    uint8_t hayPrimeraPulsacionValida = 0;  //util en el control de edicion


  private:
    bool _FLAG_PULLUP;  


    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    //   privado   ENCODER   //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

    int8_t _sentidoGiroEncoder = 0;       // almacena el sentido de giro
    uint8_t _valorEncoder = 0;             // el valor de consigna que se esta modificando
    uint8_t _valorEncoderMax = 0; 
    uint8_t _valorEncoderMin = 0; 
    void modificarConsigna();

    int16_t _valorAnteriorEncoder = -1;     // para controlar si cambia el valor y asi ahorrar refrescos de lcd
                                            // int8_t si vamos a tener menos de 127 alarmas. En pruebas y dado que las alarmas se generan
                                            // aleatoriamente, mejor asi para evitar pasarnos y tener problemas de refresco
    void asignarCosignaYterminarEdicion( void );
    
    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    //   privado  PULSADORES  //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

    uint8_t _estadoActualPulsadores;      // indica la lectura real del pulsador
    uint8_t _estadoAnteriorPulsadores;    // informa de la lectura anterior del pulsador
    uint16_t _contadorInternoPulsador;    // cuenta 'ticks' para distinguir REBOTES, pulsaciones CORTAS y LARGAS

    bool FLAG_beep = false;               // controla el beep que indica pulsacion
    bool FLAG_MODO_EDICION = false;       // controla la entrada en modo edicion, con todo lo que conlleva
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

    const uint8_t numeroMaximoParpadeos = 15;  // 'tiempo' que se mantiene ativo el modo edicion
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
