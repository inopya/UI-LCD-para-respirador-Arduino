
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


#include "enumeradores.h"

// enum enumMenuID
// {
//   ID_MENU_SEXO    = 0,   // indices de las etiquetas de los menus en 
//   ID_MENU_PESO    = 1,   // la lista 'parametrosConfiguracion
//   ID_MENU_PEEP    = 2,   
//   ID_MENU_PICO    = 3,  
//   ID_MENU_FR      = 4,  
//   ID_MENU_TIDAL   = 5,   
//   ID_MENU_RAMPA   = 6,  
//   ID_MENU_TRIGG   = 7,
//   ID_MENU_ALARMA  = 8,
//   ID_MENU_RECRUIT = 9,
// };




class PANEL_CONTROL
{

  public:

  typedef struct {
    uint8_t min;
    uint8_t consigna;
    uint8_t max;
    char etiqueta[6];
  } parametrosMaquina;


  parametrosMaquina parametrosConfiguracion[10]=
  {
    {  0,  0,   1, "SEXO ", },  // sexo 0=femenino, 1=masculino
    { 20, 65, 200, "PESO ", },  //peso en kg
    { 70, 80,  95, "PEEP ", },
    { 60, 70,  80, "PICO ", },
    { 10, 15,  25, " FR  ", },
    { 30, 40,  50, "TIDAL", },
    { 35, 45,  55, "RAMPA", },
    {  8, 10,  16, "TRIGG", },
    {  0,  0,   1, "RECRU", },   //modo recruit off por defecto
    {  1,  0,   0, "ALARM", },   //numero de alarmas si revicion, (max y min no tiene sentido)

  };


  char etiquetaBorrar[6] = "     ";



  //public:

    PANEL_CONTROL( bool pullup);

    void update( void );

    bool FLAG_MODO_DEBUG=false;            // permite los 'Serial.print' repartidos por el codigo para prueba


    //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    //  ALARMAS                  //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

    uint8_t listaDeAlarmas [30];    // ={1,2,4,2,0,5,6}; // cargado con  alarmas para DEBUG
    uint8_t contadorALARMAS =  2;   // incrementa el indice de listaDeAlarmas para poner
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
   
    char msg_lineaRespirator2020[21] =      "  RESPIRATOR  2020  ";
    char msg_dePersonasParaPersonas_L1[21]= "de Personas         ";
    char msg_dePersonasParaPersonas_L2[21]= "      para  Personas";  
    char msg_lineaInopya[21] =              "RespiMenu 1.3 inopya";

    //PANTALLA ESPERA PREVIA A SETUP RESPIRADOR

   //PANTALLA SETUP RESPIRADOR
    char msg_pantallaSetupRespirador_L0[21]=  " CONFIG. PARAMETROS ";   //20

    //PANTALLA ESPERA PREVIA A RUN
    char msg_pantallaWaitUSer_L0[21]= "RESPIRADOR EN REPOSO";   //20
    char msg_pantallaWaitUSer_L3[21]= "< CONFIG       RUN >";   //20

    //MENSAJES GENERALES
    char msg_hayAlarmas[21]= "  >>  ALARMAS  <<   ";   //20
    char msg_lineaVacia[21]= "                    ";   //20

    uint8_t indiceMenuSecundario =    ID_MENU_ALARMA;      //lleva la cuenta de la opciona a mostrar.Por defecto ALARM
    #define MINIMA_OPCION_SECUNDARIA  ID_MENU_TIDAL     //donde empieza el menu secundario
    #define MAXIMA_OPCION_SECUNDARIA  ID_MENU_ALARMA   //dodne acaba el menu secundario
    #define MINIMA_OPCION_PRIMARIA    ID_MENU_SEXO     //donde empieza el menu secundario
    #define MAXIMA_OPCION_PRIMARIA    ID_MENU_RECRUIT   //dodne acaba el menu secundario


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


    void ModoStopMaquina( void );
    void setupLcd( void );                  // inicializa el lcd y prepara su buffer
    void ModoWaitUser( void );              // pantalla invitando a configurar o iniciar
    void ModoConfigRespirador( void );      // cambia parametros: sexo, peso...
    void ModoRun( void );                   //modo normal de funcionamiento, pantalla con datos
    void ModoEdicionConsignas( void );      //acceso a ciertas consignas desde el modo run  
    void ModoPausa( void );                 //.........  pendiente
    void ModoRevisarAlarmas( void );        //  .... en proceso
    void ModoError( void );                 // ........pendiente
    void ModoRecruit( void );               //no tengo clara su funcion. Por ahora es una cuenta atras


    void pantallaPrint(uint8_t x, uint8_t y,  const char* ); // engloba posicionar el cursor e imprimir
    void lcdPrint( const char *info);  //añade el buffer sin informacion de locate
    void ultraitoa( int valor ,  char *salida );
    void Fastitoa( unsigned int InputInteger, char* OutputString );
    void checkEncoderLimits( void );
    void lcdPausa( uint16_t pausa );
    void MostarReloj( uint8_t linea=1 );
    bool MostarCuentaAtras(uint8_t linea);
    uint8_t segundos_RecruitTime = 40;      //tiempo especificado por diseño 
    void mostrarValorEncoder( void );
    void mostrarValoresSecundarios( void );

    void mostrarMensajeInicio();
    void mostrarLineaAlarma( uint8_t intervalo=1 );
    void mostrarLineaConsignas( void );
    uint8_t mostrarLineaValoresActuales( void );
    void mostrarLineaMenus( void );
    void mostrarOpcionMenuSecundario( void );

    void mostrarMensajeAlarma (const char * const * message);
    void setModo ( uint8_t modo );
  
    uint8_t modoMaquinaActual = 1;      //setupLCD
    uint8_t modoMaquinaAnterior = 0;    //parada
    //uint8_t modoMaquinaPrevioUsado = 0; //parada

    void printDebug (String debug);
  

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
