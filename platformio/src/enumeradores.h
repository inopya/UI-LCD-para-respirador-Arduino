
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//              ENUMERADORES    en uso
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

//TIMEOUTS  
enum enumTimeouts
{  
  TIMEOUT_MENU_SECUNDARIO       =  5000,
  TIMEOUT_MENU_EDICION_CONSIGNA =  6000,
  TIMEOUT_MENU_RECRUIT          = 15000, 
};

//FRECUENCIAS DE PARPADEO  
enum enumFrecuenciasParpadeo
{  
  FRECUENCIA_PARPADEO_MENU = 517,
  FRECUENCIA_PARPADEO_LINEA_ALARMA = 856,
  FRECUENCIA_REFRESCO_VALORES = 5020,
  FRECUENCIA_REFRESCO_NUEVAS_ALARMAS = 5040,
};


//DURACION PITIDOS
enum enumDuracionBeep
{  
  BEEP_CORTO = 60,
  BEEP_LARGO = 210,
  BEEP_EXTRA_LARGO = 750,
};



enum enumEstadosDisplay   
{
  MODE_STOP               =   0,    // parada de la maquina por fin de uso
  MODE_SETUP_LCD          =   1,    // setup inicial que incluye solo al LCD
  MODE_WAIT_USER          =   2,    // setup inicial que incluye solo al LCD
  MODE_CONFIG_RESPIRATOR  =   3,    // parametros iniciales del respirador, sexo, altura, limites pico...
  MODE_RUN                =   4,    // modo normal de funcionamiento
  MODE_EDICION_CONSIGNAS  =   5,    // modo edicion consignas
  MODE_ACEPTAR_CONSIGNAS  =   6,    // ¿hace falta?  //por ahora lo resulevo en edicion
  MODE_REVISION_ALARMAS   =   7,    // revision de alarmas actuales
  MODE_RECRUIT            =   8, 
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
  ID_MENU_SEXO    = 0,   // indices de las etiquetas de los menus en 
  ID_MENU_PESO    = 1,   // la lista 'parametrosConfiguracion
  ID_MENU_PEEP    = 2,   
  ID_MENU_PICO    = 3,  
  ID_MENU_FR      = 4,  
  ID_MENU_TIDAL   = 5,   
  ID_MENU_RAMPA   = 6,  
  ID_MENU_TRIGG   = 7,
  ID_MENU_RECRUIT = 8,
  ID_MENU_ALARMA  = 9,
};




//  ---------------  ENUMERADORES    sin uso por ahora  ---------------
enum enumPinType
{
   PIN_TYPE_PULLUP   = 0,
   PIN_TYPE_PULLDOWN = 1,
};


enum enumAlarmCode
{
  ERR_PEEP_BAJO    =   1,
  ERR_PEEP_ALTO    =   2,
  ERR_PICO_BAJO    =   3,
  ERR_PICO_ALTO    =   4,
  ERR_RPM_BAJO     =   5,
  ERR_RPM_ALTO     =   6,
  ERR_TIDAL        =   7,
  ERR_TRIGGER      =   8,
  ERR_RAMPA        =   9,
  ERR_BATERIA_LOW  =  10,
  ERR_DESCONOCIDO  = 100,
   
};

