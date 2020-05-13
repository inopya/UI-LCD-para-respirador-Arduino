
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
#include <avr/pgmspace.h> 




class PANEL_CONTROL
{


  typedef struct {
    uint8_t min;
    uint8_t consigna;
    uint8_t max;
    char etiqueta[6];
  } parametrosMaquina;


  parametrosMaquina parametrosConfiguracion[9]=
  {
    {  0,  0,   1, "SEXO ", },  // sexo 0=femenino, 1=masculino
    {  0, 65, 200, "PESO ", },  //peso en kg
    { 70, 80,  95, "PEEP ", },
    { 60, 70,  80, "PICO ", },
    { 10, 15,  25, " FR  ", },
    { 30, 40,  50, "TIDAL", },
    { 35, 45,  55, "RAMPA", },
    {  8, 10,  16, "TRIGG", },
    {  1,  0,   0, "ALARM", },   //numero de alarmas si revicion, (max y min no tiene sentido)
  };


  char etiquetaBorrar[6] = "     ";



  public:

    PANEL_CONTROL( bool pullup);

    void update( void );


    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    //  ALARMAS                  //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

    uint8_t listaDeAlarmas [30];    // ={1,2,4,2,0,5,6}; // cargado con  alarmas para DEBUG
    uint8_t contadorALARMAS =  0;   // incrementa el indice de listaDeAlarmas para poner
                                    // en dicha posicion el numero correspondiete a la alarma 
                                    // segun "lista_nombres_alarmas"    

    typedef struct {
      char textoAlarma [21];
    } mensaje_alarma_Type;

    void mostrarMensajeAlarma (uint8_t);

    char lista_nombres_alarmas[7][11] =
    { 
      "PEEP Alto ",
      "PEEP Bajo ",
      "PICO Alto ", 
      "PICO Bajo ",
      "FREQ Alta ",
      "FREQ Baja ",
      "BATERIA er",
    };


    bool FLAG_alarmaPEEP = false;
    bool FLAG_alarmaPICO = false;
    bool FLAG_alarmaFR = false;


    //  <<<<<<<<<<<<<   TEXTOS PANTALLAS ESTADOS   >>>>>>>>>>>>>>>>>

    //PANTALLA ESPERA PREVIA A SETUP RESPIRADOR
    char msg_pantallaWaitToSetupRespirador_L2[21]=  "  PULSE UNA TECLA";   //20
    char msg_pantallaWaitToSetupRespirador_L3[21]=  "  PARA CONFIGURAR";   //20

   //PANTALLA SETUP RESPIRADOR
    char msg_pantallaSetupRespirador_L0[21]=  " CONFIG. PARAMETROS ";   //20

    //PANTALLA ESPERA PREVIA A RUN
    char msg_pantallaWaitToRun_L0[21]= "Enter  >>  Run Mode ";   //20
    char msg_pantallaWaitToRun_L1[21]= "Esc < Back to Config";
    char msg_pantallaWaitToRun_L3[21]= "< ESC        ENTER >";   //20

    //MENSAJES GENERALES
    char msg_hayAlarmas[21]= "  >>  ALARMAS  <<   ";   //20
    char msg_lineaVacia[21]= "                    ";   //20

  


    uint8_t indiceMenuSecundario = 8;     //lleva la cuenta de la opciona a mostrar.por defecto (8)ALARM
    #define MINIMA_OPCION_SECUNDARIA  5   //donde empieza el menu secundario
    #define MAXIMA_OPCION_SECUNDARIA  8   //dodne acaba el menu secundario




    char msg_valorItoA[6]="00000";      //este debe ser mutable, 
                                        //sobre el se escriben todos los resultados
                                        //tras ser convertidos a cadena de caracteres
  

    uint8_t volumenTidalactual;

    uint8_t valorTRIGGERactual;

    uint8_t valorRAMPAactual;

    uint8_t valorActualPeep=0;
    uint8_t valorAnteriorPeep=0;   //por si queremos ahorrar refrescos de lcd


    uint8_t valorActualPico=0;
    uint8_t valorAnteriorPico=0;

    uint8_t valorAnteriorRpm=0;
    uint8_t valorActualRpm=0;


    uint16_t timeOutMenu = 0;  //para ser usado en distintas menus como timeout timer


    
    uint32_t _contadorTicksReloj = 0;      // lleva la cuenta de las veves que somos llamados (reloj interno)
                                           // de la clase
    bool FLAG_MODO_DEBUG=false;            // permite los 'Serial.print' repartidos por el codigo para prueba

    void ModoStopMaquina( void );
    void setupLcd( void );                  // inicializa el lcd y prepara su buffer
    void ModoWaitToConfig( void );          // pantalla invitando a configurar
    void ModoConfigRespirador( void );      // cambia parametro sexo, peso...
    void ModoWaitToRun( void );             // previa a RUN que permite volver a CONFIG
    void ModoRun( void );                   //modo normal de funcionamiento, pantalla con datos
    void ModoEdicionConsignas( void );
    void ModoAceptarConsignas( void );
    void ModoPausa( void );
    void ModoRevisarAlarmas( void );
    void ModoError( void );


    void pantallaPrint(uint8_t x, uint8_t y,  const char* ); // engloba posicionar el cursor e imprimir
    void lcdPrint( const char *info);  //añade el buffer sin informacion de locate
    void ultraitoa( int valor ,  char *salida );
    void checkEncoderLimits( void );
  
    void mostrarValorEncoder( void );
    void mostrarValoresSecundarios( void );

    //void mostrarLineaCabecera( void );
    void mostrarLineaInopya( void );
    void mostrarLineaAlarma( uint8_t intervalo=1 );
    void mostrarLineaConsignas( void );
    void mostrarLineaValoresActuales( void );
    void mostrarLineaMenus( void );
    void mostrarOpcionMenuSecundario( void );

    void mostrarMensajeAlarma (const char * const * message);
    void setModo ( uint8_t modo );
  
    uint8_t modoMaquinaActual = 1;      //setupLCD
    uint8_t modoMaquinaAnterior = 0;    //parada
    //uint8_t modoMaquinaPrevioUsado = 0; //parada
  

    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    //   publico   PULSADORES    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

    void updatePulsadores( uint8_t intervalo=1 );
    
    uint8_t tipoPulsacion = 0;              // indica si es pulsacion CORTA o LARGA
    uint8_t estadoPulsadoresActual = 0;     // aqui se almacena el estado para que lo vea el programa
    uint8_t hayPrimeraPulsacionValida = 0;  //util en el control de edicion


  private:
    bool _FLAG_PULLUP;  


    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    //             ENCODER   //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

    int8_t leerEncoder( void );           // retorna el estado del pulsador  -1,0,1
                                          // giro Izquierda, Reposo,  giro Derecha

    int8_t _sentidoGiroEncoder = 0;           // almacena el sentido de giro
    uint8_t _valorEncoder = 0;                // el valor que se muestra mientras se esta editando
    uint8_t _valorEncoderMin = 0;             // limite inferior del dato editado 
    uint8_t _valorEncoderMax = 0;             // limite superior del dato editado 
    
  
    int16_t _valorAnteriorEncoder = -1;     // para controlar si cambia el valor que esta controlando
                                            //y asi ahorrar refrescos de lcd
    
    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    //   privado  PULSADORES  //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

    uint8_t _lecturaActualPulsadores;      // indica la lectura real del pulsador
    uint8_t _lecturaAnteriorPulsadores;    // informa de la lectura anterior del pulsador
    uint16_t _contadorInternoPulsador;    // cuenta 'ticks' para distinguir REBOTES, pulsaciones CORTAS y LARGAS

    bool FLAG_beep = false;               // controla el beep que indica pulsacion
        uint8_t leerPulsadoresRaw( uint8_t intervalo );       // retorna el tipo de pulsacion  0,1,2 y lo asigna a 'tipoPulsacion'  
    void beep( uint16_t );                // genera el beep del pulsador al terminar pulsacion
    void resetPulsador( void );                   // restablece el pulsador tras una pulsacion 
    uint8_t estadoPulsadoresAnterior=0;         // ultima pulsacion util y que se esta procesando

  
    uint8_t readPin( void );
  
};





//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//              ENUMERADORES    en uso
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

//TIMEOUTS  
enum enumTimeouts
{  
  TIMEOUT_MENU_SECUNDARIO = 8000,
  TIMEOUT_MENU_EDICION_CONSIGNA = 6000,
};

//FRECUENCIAS DE PARPADEO  
enum enumFrecuenciasParpadeo
{  
  FRECUENCIA_PARPADEO_MENU = 500,
  FRECUENCIA_PARPADEO_LINEA_ALARMA = 750,
  FRECUENCIA_REFRESCO_VALORES = 5000,
  FRECUENCIA_REFRESCO_NUEVAS_ALARMAS = 1000,
};


//              ENUMERADORES    sin uso por ahora
enum enumEstadosDisplay   
{
  MODE_STOP               =   0,    // parada de la maquina por fin de uso
  MODE_SETUP_LCD          =   1,    // setup inicial que incluye solo al LCD
  MODE_WAIT_TO_CONFIG     =   2,    // setup inicial que incluye solo al LCD
  MODE_CONFIG_RESPIRATOR  =   3,    // parametros iniciales del respirador, sexo, altura, limites pico...
  MODE_WAIT_TO_RUN        =   4,    // pantalla de espera entre configurar y modo run
  MODE_RUN                =   5,    // modo normal de funcionamiento
  MODE_EDICION_CONSIGNAS  =   6,    // modo edicion consignas
  MODE_ACEPTAR_CONSIGNAS  =   7,    // ¿hace falta?  //por ahora lo resulevo en edicion
  MODE_REVISION_ALARMAS   =   8,    // revision de alarmas actuales 
  MODE_PAUSE              =  99,    // ¿ será necesria para algun tipo  de mantenimiento ?
  MODE_ERROR              = 100,    // ¿ algun tipo de parada crítica ?
};

enum enumPulsadorID
{
  ID_PULSADOR_FR       = 1,   //es el valor decimal que tienen en el puerto en el que estan conectados
  ID_PULSADOR_PICO     = 2,  
  ID_PULSADOR_PEEP     = 4,
  ID_PULSADOR_ENCODER  = 32,
};

enum enumMenuID
{
  ID_MENU_PEEP    = 2,   // indice de las etiquetas de los menus en 
  ID_MENU_PICO    = 3,   // la lista 'parametrosConfiguracion'
  ID_MENU_FR      = 4,  
  ID_MENU_TIDAL   = 5,   
  ID_MENU_RAMPA   = 6,  
  ID_MENU_TRIGG   = 7,
  ID_MENU_ALARMA  = 8,
};

enum enumPinType
{
   pullup   = 0,
   pulldown = 1,
};


enum enumAlarmCode
{
   PEEP_BAJO, PEEP_ALTO, PICO_BAJO, PICO_ALTO, RPM_BAJO, RPM_ALTO

};
