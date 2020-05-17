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


// /*
//   mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//   .............................................................................................   
//   .............................................................................................
//                                    PANEL_CONTROL

  
//                *******    IMPLEMENTACION DE LOS METODOS DE CADA CLASE    *******

//   .............................................................................................
//   .............................................................................................
//   mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// */

using namespace std;

#define BUFFER_CIRCULAR

#include "definiciones.h"
#include "utiles.h"
#include "pinout.h"   

#include "lcd_virtual.h"

//creaacion del objeto 'lcd con buffer de escritura', para uso interno de la clase panel de control
//LCD_CON_BUFFER  lcd(PIN_RS,PIN_E,PIN_D7,PIN_D6,PIN_D5,PIN_D4);//pines lcd(rs,e,d4,d5,d6,d7)

LCD_CON_BUFFER  lcd(PIN_RS,PIN_E,PIN_D4,PIN_D5,PIN_D6,PIN_D7);//pines lcd(rs,e,d4,d5,d6,d7)





// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//                                       ZUMBADOR
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm


void PANEL_CONTROL::beep(uint16_t tiempo=60)   
{
  //genera un pitido de duracion 'tiempo'
  static uint16_t duracion;
  static boolean FLAG_beep_anterior;
  if(FLAG_beep==false){
    FLAG_beep_anterior = false;
    FLAG_beep = true;
    duracion=tiempo;
  }
  
  static uint16_t controlBeep;
  if(controlBeep<duracion){ //cada ciclo, en teoria 1 ms
    controlBeep++;
  }
  else{
    FLAG_beep = false;
    controlBeep = 0;
  }
  if(FLAG_beep!=FLAG_beep_anterior){
    FLAG_beep_anterior = FLAG_beep;
    bitWrite(PORTB,4,FLAG_beep);
    //digitalWrite(PIN_zumbador, FLAG_beep);
  }
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::setModo( uint8_t modo )
{
  modoMaquinaAnterior = modoMaquinaActual;
  modoMaquinaActual = modo; //establecer el modo
  if(FLAG_MODO_DEBUG){
    Serial.print(">>  Modo Anterior:");
    Serial.print(modoMaquinaAnterior);
    Serial.print(", Modo Actual:");
    Serial.println(modoMaquinaActual);
  }
}
    


// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//      <<<  UPDATE  >>>     METODO DE ENTRADA DESDE EL PROGRAMA 
// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


void PANEL_CONTROL::update()
{

  


  //-------------------------------------------------------------
  //-------------------------------------------------------------
  if( _contadorTicksReloj%5 != 0 ){ 
   //return; 
  }
  //-------------------------------------------------------------
  //-------------------------------------------------------------

  /* <<<< Incrementar nuestro propio 'marcapasos'  >>>>  */
  _contadorTicksReloj++;


   /* <<<< refrescar el LCD  >>>>  */
  #ifdef BUFFER_CIRCULAR
    // -- version buffer circular
    lcd.readBuffer(); //obtiene un caracter del buffer de impresion y lo envia  al lcd
  #else
    // -- version pantalla viertual
    lcd.readPantallaVirtual();
  #endif


  static bool FLAG_slowTime = true;
  // if(_contadorTicksReloj%5==0){
  //   FLAG_slowTime=!FLAG_slowTime; 
  // }
  
  // leer pulsadores solo cada cierto tiempo ? 
  //(en este caso borrar manualmente la pulsacion al pasar por aqui)
  // tipoPulsacion = 0;
  // estadoPulsadores = 0;
  // static uint8_t intervaloPulsador;
  // intervaloPulsador++;
  // if(intervaloPulsador>=5){   // compararlo "contra" el tiempo que queremos dejarlo abandonado
  //   updatePulsadores(intervaloPulsador);  //esto actualiza  tipoPulsacion y estadoActualPulsadores
  //   intervaloPulsador = 0;
  // }

  /* <<<<  indicar con el buzzer todas las pulsaciones??  >>>>  */
  // if(tipoPulsacion==1){
  //   beep(80); //pitido corto
  // }
  // if(tipoPulsacion==2){
  //   beep(280);  //pitido largo
  // }


  /* <<<< CONTROL DEL ESTADO ACTUAL  >>>>  */

  //no leer los pulsadores ni el encoder en los mementos de cambio de estado
  //para evitar perder interacciones que se deben arrastrar
  

  /*   OJO NO LEER LOS PULSADORES ENTRE CAMBIOS DE ESTADO, (o ME ARRUINA CIERTAS DETECCIONES) */
  if( modoMaquinaActual == modoMaquinaAnterior){
    /* <<<< actualizar encoder  >>>>  */
     if(FLAG_slowTime){  leerEncoder(); }

    /* <<<< actualizar pulsadores  >>>>  */
    updatePulsadores(); //si queremos leerlos cada ciertos ticks, revisar el codigo comentado arriba
    if( FLAG_MODO_DEBUG && estadoPulsadoresActual!=0 ){
        Serial.print(F("tipoPulsacion: "));Serial.print(tipoPulsacion);
        Serial.print(F(" ,estadoPulsadores viejo: "));Serial.print(estadoPulsadoresAnterior);
        Serial.print(F(" ,estadoPulsadores nuevo: "));Serial.println(estadoPulsadoresActual);
    }

  }


  /* <<<< CAMBIOS DE ESTADO  >>>>  */
  switch (modoMaquinaActual)
  {
  case MODE_RUN:
    //if(FLAG_slowTime){    ModoRun();  }
    ModoRun();
    break;
  case MODE_EDICION_CONSIGNAS:
    ModoEdicionConsignas();
    break;
  case MODE_SETUP_LCD: 
    setupLcd();
    mostrarMensajeInicio();
    lcdPausa(5000);
    lcd.lcdClear();
    setModo(MODE_WAIT_USER);
    break;
  case MODE_WAIT_USER:
    ModoWaitUser();
    break;
  case MODE_CONFIG_RESPIRATOR: 
    ModoConfigRespirador();
    break; 
  case MODE_REVISION_ALARMAS:
    ModoRevisarAlarmas();
    break;    
  case MODE_PAUSE: 
    //  sin implementar aun
    break;
  case MODE_RECRUIT: 
    ModoRecruit();   //por ahora no tenemos codio apra este modo
    break;
  case MODE_ERROR:
    ModoError();
    break; 
  case MODE_STOP:
    //  sin implementar aun
    break; 
  default:
    ModoError();
  //  ¿¿  necesitamos aqui algo de este tipo  ??
    if( FLAG_MODO_DEBUG ){
      Serial.println(F("ERROR DESCONOCIDO"));
    }
    for(;;){};      
    break;
  }


  /* <<<< indicar con el buzzer pulsaciones que conllevan acciones reales >>>>  */
  if( tipoPulsacion>0 && modoMaquinaActual!=modoMaquinaAnterior ){ 
    beep(BEEP_CORTO);
  }

  /* <<<< para poder mantener el beep durante su duracion estipulada  >>>>  */
  if( FLAG_beep==true ){ beep(); }

}


// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx





// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//                                       PANTALLA
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

void PANEL_CONTROL::mostrarMensajeInicio()
{
  pantallaPrint(0,0, msg_lineaRespirator2020);
  pantallaPrint(0,1,msg_dePersonasParaPersonas_L1);
  pantallaPrint(0,2,msg_dePersonasParaPersonas_L2);
  pantallaPrint(0,3, msg_lineaInopya);
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::lcdPausa(uint16_t pausa)
{
  //solucion temporal. Hacer algo mas elegante
  while(pausa>2){
    lcd.readBuffer();
    pausa--;
    delay(1);
  }
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::mostrarLineaAlarma(uint8_t intervalo)
{
  static bool FLAG_parpadeoLineaAlarma = false;
  static uint16_t contadorParpadeoAlarma;  //_contadorTicksReloj
  contadorParpadeoAlarma+=intervalo;
  if( contadorParpadeoAlarma > FRECUENCIA_PARPADEO_LINEA_ALARMA ){
    contadorParpadeoAlarma = 0;
    FLAG_parpadeoLineaAlarma = !FLAG_parpadeoLineaAlarma;
  }
  else{ return; }

  if( FLAG_parpadeoLineaAlarma){ pantallaPrint(0,0, msg_hayAlarmas); }
  else{ pantallaPrint(0,0, msg_lineaVacia);}

  return;
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::mostrarLineaMenus()
{
  pantallaPrint(0,3,  parametrosConfiguracion[ID_MENU_PEEP].etiqueta);  //msg_peep
  lcdPrint(parametrosConfiguracion[ID_MENU_PICO].etiqueta);
  lcdPrint(parametrosConfiguracion[ID_MENU_FR].etiqueta);
  lcdPrint(parametrosConfiguracion[ID_MENU_ALARMA].etiqueta);

  //pantallaPrint(5,3,  parametrosConfiguracion[3].etiqueta);  //msg_pico
  //pantallaPrint(10,3, parametrosConfiguracion[4].etiqueta);  //msg_rf
  //pantallaPrint(15,3, parametrosConfiguracion[9].etiqueta);  //msg_alarm

  return;
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::mostrarOpcionMenuSecundario()
{
  //esto solo muestra en pantalla el texto del menu secundadio
  pantallaPrint(15,3, parametrosConfiguracion[indiceMenuSecundario].etiqueta);

  return;
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::mostrarLineaConsignas()
{
  pantallaPrint(0,1, msg_lineaVacia);

  ultraitoa(parametrosConfiguracion[2].consigna , msg_valorItoA);
  pantallaPrint(0,1, msg_valorItoA);

  ultraitoa(parametrosConfiguracion[3].consigna , msg_valorItoA);
  pantallaPrint(5,1, msg_valorItoA);

  ultraitoa(parametrosConfiguracion[4].consigna , msg_valorItoA);
  pantallaPrint(10,1, msg_valorItoA);

  return;
}


// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void  PANEL_CONTROL::printDebug (String debug)
{
  Serial.println(debug);
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

uint8_t  PANEL_CONTROL::mostrarLineaValoresActuales()
{

  //return;

  static uint8_t actuacionLotes = 0;

  actuacionLotes++;
  if(actuacionLotes==4){ actuacionLotes=0; }

  if(actuacionLotes==1){


    //  esto es atrezo PARA IR SIMULANDO ALARMAS
    //si los valores se salen de rango los vuelvo a la consigna
    if( valorActualPeep<parametrosConfiguracion[ID_MENU_PEEP].min ){
      listaDeAlarmas[++contadorALARMAS]=ERR_PEEP_BAJO;
      valorActualPeep = parametrosConfiguracion[ID_MENU_PEEP].consigna;
    }                            
    else if( valorActualPeep>parametrosConfiguracion[ID_MENU_PEEP].max ){
      listaDeAlarmas[++contadorALARMAS]=ERR_PEEP_ALTO;
      valorActualPeep = parametrosConfiguracion[ID_MENU_PEEP].consigna;
    }

    if ( valorActualPeep!=valorAnteriorPeep ){
      ultraitoa(valorActualPeep, msg_valorItoA);
      pantallaPrint(0,2, msg_valorItoA);
    }

    valorAnteriorPeep = valorActualPeep;
  }

  else if(actuacionLotes==2){
   
    //  esto es atrezo PARA IR SIMULANDO ALARMAS
    //si los valores se salen de rango los vuelvo a la consigna
    if( valorActualPico<parametrosConfiguracion[ID_MENU_PICO].min ) {
      listaDeAlarmas[++contadorALARMAS]=ERR_PICO_BAJO;
      valorActualPico = parametrosConfiguracion[ID_MENU_PICO].consigna;
    }

    else if( valorActualPico>parametrosConfiguracion[ID_MENU_PICO].max ){
      listaDeAlarmas[++contadorALARMAS]=ERR_PICO_ALTO;
      valorActualPico = parametrosConfiguracion[ID_MENU_PICO].consigna;
    }

    if ( valorActualPico!=valorAnteriorPico ){
      ultraitoa(valorActualPico, msg_valorItoA);
      pantallaPrint(5,2, msg_valorItoA);
    }

    valorAnteriorPico = valorActualPico;
  }


  else if(actuacionLotes==3){

    //  esto es atrezo PARA IR SIMULANDO ALARMAS
    //si los valores se salen de rango los vuelvo a la consigna

    if( valorActualRpm<parametrosConfiguracion[ID_MENU_FR].min ) {
      listaDeAlarmas[++contadorALARMAS]=ERR_RPM_BAJO;
      valorActualRpm = parametrosConfiguracion[ID_MENU_FR].consigna;
    }

    else if( valorActualRpm>parametrosConfiguracion[ID_MENU_FR].max ){
      listaDeAlarmas[++contadorALARMAS]=ERR_RPM_ALTO;
      valorActualRpm = parametrosConfiguracion[ID_MENU_FR].consigna;
    }

    if( valorActualRpm!=valorAnteriorRpm ){
      ultraitoa(valorActualRpm, msg_valorItoA);
      pantallaPrint(10,2, msg_valorItoA);
    }
    valorAnteriorRpm = valorActualRpm;
  }

  parametrosConfiguracion[ID_MENU_ALARMA].consigna=contadorALARMAS;
  parametrosConfiguracion[ID_MENU_ALARMA].max=contadorALARMAS;
  
  return actuacionLotes;

}


// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::ultraitoa( int valor , char *salida)
{
  //Jose Luis Perez Barrales 
  //https://gitlab.com/joseluisperezbarrales/
  //con algunos retoques inopya
  
  bool FLAG_centenas = true;

  /*   CENTENAS   */
    int contcentenas=-1;
    if( valor > 999 )
      return;
    if( valor <0 )
      return;

    while( valor>=0 )
    {
      valor-=100;
      contcentenas++;
    }
    //  En este punto  valor es negativo , le sumamos 100 y estamos en el rango { 0 , 99 }
    valor+=100;

    if(contcentenas>0){
      *salida = '0'+contcentenas;  // añadimos las centenas a la cadena
    }
    else{
      FLAG_centenas = false;
      *salida = ' ';                // si no hay centenas, sustituimos por un espacio
    }
                                    //si sustituimos el if anterior por esta linea
    //*salida = '0'+contcentenas;   // forzamos cero en las centenas para numeros de dos cifra
    salida++;

  /*   DECENAS   */
    contcentenas=-1;
    while( valor>=0 )
    {
      valor-=10;
      contcentenas++;
    }
    //  En este punto  valor es negativo , le sumamos 10 y estamos en el rango { 0 , 9 }
    valor+=10;


    if(FLAG_centenas==false){
      if(contcentenas>0){
      *salida = '0'+contcentenas;  // añadimos las decenas a la cadena
      }
      else{
      *salida = ' ';                // si no hay decenas, sustituimos por un espacio
      }
    }
    else{
      *salida = '0'+contcentenas;  
    }
                                    //si sustituimos el if anterior por esta linea
    //*salida = '0'+contcentenas;   // forzamos cero en las decenas para numeros de una cifra

    salida++;

    /*   UNIDADES   */
    *salida = '0' + valor;        // añadimos las unidades
    salida++;
    *salida=0;                    // añadir fin de cadena
}


// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::mostrarValoresSecundarios()
{
  // muestra entre parenteris en la linea 2 (0,1,2..) 
  // el valor de alarmas o de consignas secundarias al girar el encoder en modo run
  //previo a editarlas


  int8_t valorParaMostrar = 0;
  static int8_t ultimoMostrado = 255;
  
  valorParaMostrar=parametrosConfiguracion[indiceMenuSecundario].consigna;

  if(valorParaMostrar!=ultimoMostrado){
    ultraitoa(valorParaMostrar, msg_valorItoA);
    pantallaPrint(15,2, "(");
    lcdPrint(msg_valorItoA);
    lcdPrint(")");
    ultimoMostrado = valorParaMostrar;
  }
}






// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx



//                   MODOS OPERATIVOS DEL RESPIRADOR



// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm




// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::setupLcd()
{
  lcd.lcdInit();

  if(FLAG_MODO_DEBUG){
    Serial.println(F("SISTEMA LCD ACTIVADO"));
  }

  estadoPulsadoresActual=0;  
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::ModoWaitUser()
{
  //casi definitivo  
  if(modoMaquinaAnterior!=MODE_WAIT_USER){
    lcd.lcdClear();
    pantallaPrint(0,0, msg_pantallaWaitUSer_L0);  
    pantallaPrint(0,2, msg_lineaVacia);  
    pantallaPrint(0,3, msg_pantallaWaitUSer_L3);
    //Serial.print(modoMaquinaAnterior); Serial.print(","); Serial.println(modoMaquinaActual); 
    MostarReloj(1);

    setModo(MODE_WAIT_USER);
    if(FLAG_MODO_DEBUG){
      Serial.println(F("MODE_WAIT_USER ( <config    run> )"));
    }
    
  }

  if( _contadorTicksReloj%1000==0 ){ MostarReloj(1); }

  switch (estadoPulsadoresActual)
  {
  case 1:  //go to run mode
    //tipoPulsacion=0; //anulamos la pulsacion detectatada
    //estadoPulsadores = 0;
    setModo(MODE_RUN);  //entra de lleno en el modo run
    break;      
  case 4:  //return to config mode
    setModo(MODE_CONFIG_RESPIRATOR);//vuelve al modo configuracion
    break;
  }

}


// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::MostarReloj(uint8_t linea)
{

  static uint8_t segundos=0;
  static uint8_t minutos=0;  
  static uint8_t horas=0;    
  static uint8_t minutosOld = 100;
  static uint8_t horasOld = 100;

  //if( _contadorTicksReloj%1000!=0 ){ return; }
  segundos++;
  if(segundos>59){
    segundos=0;
    minutos++;
    if(minutos>59){
      minutos=0;
      horas++;
      if(horas>99){
        horas=0;
      }
    }
  }


  //Serial.print("valores de las maquinas en reloj: ");
  //Serial.print(modoMaquinaAnterior); Serial.print(","); Serial.println(modoMaquinaActual); 
  //delay(1000);
  ultraitoa(segundos, msg_valorItoA);
  if(segundos<10){ 
    pantallaPrint(11,linea,msg_valorItoA);
    pantallaPrint(12,linea,"0");
  }
  else{ pantallaPrint(11,linea,msg_valorItoA); }
  pantallaPrint(11,linea,":");

  if(minutos!=minutosOld || modoMaquinaAnterior!=modoMaquinaActual ){
    ultraitoa(minutos, msg_valorItoA);
    if(minutos<10){ 
      pantallaPrint(8,linea,msg_valorItoA);
      pantallaPrint(9,linea,"0");
    }
    else{ pantallaPrint(8,linea,msg_valorItoA); }
    pantallaPrint(8,linea,":");
  }

  if( horas!=horasOld || modoMaquinaAnterior!=modoMaquinaActual ){
    ultraitoa(horas, msg_valorItoA);
    if(horas<10){ 
      pantallaPrint(5,linea,msg_valorItoA);
      pantallaPrint(6,linea,"0");
    }
    else{ pantallaPrint(5,linea,msg_valorItoA); }
  }

  horasOld=horas;
  minutosOld=minutos;

}




// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void PANEL_CONTROL::ModoRecruit()
{
  static bool FLAG_runRecruit;

  if( modoMaquinaAnterior != MODE_RECRUIT ){
    FLAG_runRecruit  = false;
    timeOutMenu = 0;
    lcd.lcdClear();
    setModo(MODE_RECRUIT);
    segundos_RecruitTime = 40;
    //segundos=0;
    //minutos=0;
    //horas=0;
    estadoPulsadoresActual=0;  //evitar arrastrar pulsaciones entre modos. 

    pantallaPrint(0,0, "MODO RECRUIT < OFF >"); 
    pantallaPrint(6,1, "00:00:40      ");

    pantallaPrint(0,3, "<CANCEL     RECRUIT>"); 
    _valorAnteriorEncoder = -1;
    if(FLAG_MODO_DEBUG){
      Serial.println(F("MODE_RECRUIT"));
    }
  }


  if( FLAG_runRecruit ){
    if( _contadorTicksReloj%1000==0 ){ FLAG_runRecruit = MostarCuentaAtras(1); }
    if( !FLAG_runRecruit ) {
      timeOutMenu=0;
      beep(BEEP_EXTRA_LARGO);
      pantallaPrint(16,0, "FF > ");  
      pantallaPrint(6,1, "00:00:40 ");
      segundos_RecruitTime = 40;
      //setModo(MODE_WAIT_USER); 
    }
  }


  switch (estadoPulsadoresActual)
  {
  case 1:
    FLAG_runRecruit  = true;
    pantallaPrint(16,0, "N > "); 
    beep(BEEP_CORTO);
    break;      
  case 4: 
    segundos_RecruitTime = 40;   //por si acaso lo reseteo aqui tambien
    setModo(MODE_RUN);//vuelve al modo configuracion
    break;
  }

  //  CONTROL DE TIMEOUT PARA SALIR DE LA VISUALIZACION SECUNDARIAS
  if( !FLAG_runRecruit ) {  //este flag es apra no incrementar timeOutMenu cuando no he tocado nada
    timeOutMenu++;
    if (timeOutMenu > TIMEOUT_MENU_RECRUIT ) {
      setModo(MODE_RUN);
    }
  }


}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

bool PANEL_CONTROL::MostarCuentaAtras(uint8_t linea)
{
  //segundos_RecruitTime //son globales apra podr manipularlos fuera de aqui
  static int8_t minutos=0;  
  static int8_t horas=0;    
  static uint8_t minutosOld = 100;
  static uint8_t horasOld = 100;
  static bool FLAG_cuentaAtrasActiva = true;

  
  //if( _contadorTicksReloj%1000!=0 ){ return; }
  segundos_RecruitTime--;
  if(segundos_RecruitTime<0){
    if(minutos>0){
      segundos_RecruitTime=59;
      minutos--;
    }
    if(minutos<0){
      if(horas>0){
        minutos=0;
        horas--;
      }
    }
  }


  if(horas==0 && minutos==0 && segundos_RecruitTime==0 ){ 
    FLAG_cuentaAtrasActiva = false; 
    segundos_RecruitTime = 40;  //reset del tiempo apra la proxima vez??
  }
  else{FLAG_cuentaAtrasActiva = true; }


  ultraitoa(segundos_RecruitTime, msg_valorItoA);
  if(segundos_RecruitTime<10){ 
    pantallaPrint(11,linea,msg_valorItoA);
    pantallaPrint(12,linea,"0");
  }
  else{ pantallaPrint(11,linea,msg_valorItoA); }
  pantallaPrint(11,linea,":");

  if(minutos!=minutosOld || modoMaquinaAnterior!=modoMaquinaActual ){
    ultraitoa(minutos, msg_valorItoA);
    if(minutos<10){ 
      pantallaPrint(8,linea,msg_valorItoA);
      pantallaPrint(9,linea,"0");
    }
    else{ pantallaPrint(8,linea,msg_valorItoA); }
    pantallaPrint(8,linea,":");
  }

  if( horas!=horasOld || modoMaquinaAnterior!=modoMaquinaActual ){
    ultraitoa(horas, msg_valorItoA);
    if(horas<10){ 
      pantallaPrint(5,linea,msg_valorItoA);
      pantallaPrint(6,linea,"0");
    }
    else{ pantallaPrint(5,linea,msg_valorItoA); }
  }

  horasOld=horas;
  minutosOld=minutos;

  return FLAG_cuentaAtrasActiva;
}





   
// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//                      en proceso!!!
// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void PANEL_CONTROL::ModoConfigRespirador()
{
  static uint8_t consignaEnEdicion  = 1; 
  
  if( modoMaquinaAnterior != MODE_CONFIG_RESPIRATOR ){
    lcd.lcdClear();
    setModo(MODE_CONFIG_RESPIRATOR);
    estadoPulsadoresActual=4; //forzamos una primera pulscion para regenerar menu 

    pantallaPrint(0,0, msg_pantallaSetupRespirador_L0); 

    pantallaPrint(7,3, "NEXT>   ENTER"); //<8 , 4> , 1
    _valorAnteriorEncoder = -1;
    if(FLAG_MODO_DEBUG){
      Serial.println(F("MODE_CONFIG_RESPIRATOR"));
    }
  }


  // VISUALIZAR VALORES QUE SE ESTAN MODIFICANDO

  if ( estadoPulsadoresActual==1 ){   //ACEPTAR
    beep(BEEP_CORTO);
    parametrosConfiguracion[consignaEnEdicion].consigna=_valorEncoder;
    estadoPulsadoresActual=2;
  } 

  if (estadoPulsadoresActual==2 || estadoPulsadoresActual==4 ){ 
    if (estadoPulsadoresActual==2 && consignaEnEdicion<MAXIMA_OPCION_PRIMARIA ){consignaEnEdicion++;}       
    if (estadoPulsadoresActual==4 && consignaEnEdicion>MINIMA_OPCION_PRIMARIA ){consignaEnEdicion--;} 
    if ( consignaEnEdicion<MINIMA_OPCION_PRIMARIA+1 ) { consignaEnEdicion=MINIMA_OPCION_PRIMARIA; }  
    if ( consignaEnEdicion>MAXIMA_OPCION_PRIMARIA ) { consignaEnEdicion=MAXIMA_OPCION_PRIMARIA; }
    pantallaPrint(0,2, parametrosConfiguracion[consignaEnEdicion].etiqueta);
    lcdPrint(" --> (");
    _valorEncoderMin = parametrosConfiguracion[consignaEnEdicion].min;
    _valorEncoderMax = parametrosConfiguracion[consignaEnEdicion].max;
    _valorEncoder = parametrosConfiguracion[consignaEnEdicion].consigna;
    _valorAnteriorEncoder = -1;
    if(consignaEnEdicion==MINIMA_OPCION_PRIMARIA+1){pantallaPrint(0,3, "<BACK");}
    if(consignaEnEdicion==MINIMA_OPCION_PRIMARIA){pantallaPrint(0,3, "     ");}
    if(consignaEnEdicion==MAXIMA_OPCION_PRIMARIA){pantallaPrint(7,3, "     ");}
    if(consignaEnEdicion==MAXIMA_OPCION_PRIMARIA-1){pantallaPrint(7,3, "NEXT>");}
  }

  if( _sentidoGiroEncoder!=0 ) { 
    if(_valorEncoder==0 && _sentidoGiroEncoder<0){_valorEncoder=0;}
    else{ _valorEncoder+=_sentidoGiroEncoder; }
  }
  
  if(_valorEncoder != _valorAnteriorEncoder ){
    checkEncoderLimits();
    if(consignaEnEdicion==ID_MENU_SEXO){
      if(_valorEncoder==1){ pantallaPrint(11,2, " M "); }
      else{ pantallaPrint(11,2, " H "); }
    }
    else if(consignaEnEdicion==ID_MENU_RECRUIT){
      if(_valorEncoder>0){ pantallaPrint(11,2, "On "); }
      else{ pantallaPrint(11,2, "Off"); }
    }
    else{
      ultraitoa(_valorEncoder, msg_valorItoA);
      pantallaPrint(11,2, msg_valorItoA);
    }
    lcdPrint(")");
    _valorAnteriorEncoder  =_valorEncoder;
  }

  if ( estadoPulsadoresActual==32 ){
    setModo(MODE_WAIT_USER);
    return;
  }

}



// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


void PANEL_CONTROL::ModoRun()
{
  static bool FLAG_primeraEntradaRun = false; 
  static bool FLAG_toqueteando = false; 

  if( modoMaquinaAnterior!=MODE_RUN ){
    FLAG_primeraEntradaRun=true;
    if( modoMaquinaAnterior!=MODE_EDICION_CONSIGNAS ) {lcd.lcdClear(); }
    
    
    setModo(MODE_RUN);
    mostrarLineaConsignas();              //mostar consignas (en la linea 1)
    mostrarLineaMenus();                  //mostar menus     (en la linea 3)
    
    indiceMenuSecundario=ID_MENU_ALARMA;
    mostrarValoresSecundarios();      
    estadoPulsadoresActual=0;  //evitar arrastrar pulsaciones entre modos. (Hacerlo mas elegante)
  
    if(FLAG_MODO_DEBUG){
      Serial.println(F("MODE_RUN"));
    }

    //para asegurar el refresco de la linea de valores actuales
    valorAnteriorPeep = valorAnteriorPico = valorAnteriorRpm = 0; 
    

  }

  if(contadorALARMAS>0){ mostrarLineaAlarma(); }   //>0 Mensaje parpadeo, aviso de que hay alarmas

  static bool FLAG_actualizar_valores_actuales = false;
  if( _contadorTicksReloj%FRECUENCIA_REFRESCO_VALORES==0 || FLAG_primeraEntradaRun ) {      //  ¿¿ han pasado "5 segundos" en mi universo ??
    FLAG_actualizar_valores_actuales = true;
  }

  if(FLAG_actualizar_valores_actuales){
    static uint8_t FLAG_estado_actualizacion = 1;
    FLAG_estado_actualizacion = mostrarLineaValoresActuales();      //   - en ese caso refrescar los datos si es necesario)
    if(FLAG_estado_actualizacion==0){ FLAG_actualizar_valores_actuales=false; }
  }

  //if((_contadorTicksReloj%FRECUENCIA_REFRESCO_NUEVAS_ALARMAS==0 ||FLAG_primeraEntradaRun) && !FLAG_toqueteando){
  if( _contadorTicksReloj%FRECUENCIA_REFRESCO_NUEVAS_ALARMAS==0 && !FLAG_toqueteando ){
    mostrarValoresSecundarios();      //   - en ese caso refrescar los datos si es necesario
  }


  if(estadoPulsadoresActual>0){
    if(estadoPulsadoresActual==ID_PULSADOR_ENCODER && indiceMenuSecundario==ID_MENU_ALARMA){
      setModo(MODE_REVISION_ALARMAS);
    }
    else if(estadoPulsadoresActual==ID_PULSADOR_ENCODER && indiceMenuSecundario==ID_MENU_RECRUIT){
      setModo(MODE_RECRUIT);
    }
    else{
      setModo(MODE_EDICION_CONSIGNAS);
    }
    return; 
  }


  // VISUALIZAR OPCIONES DE MENU SECUNDARIO CON EL ENCODER
  static uint8_t durezaEncoderEnMenu = 20;  //cambalavhe para insensibilizar al encoder
  if( _sentidoGiroEncoder!=0 ) {

    durezaEncoderEnMenu += _sentidoGiroEncoder;
    if(durezaEncoderEnMenu<19 || durezaEncoderEnMenu>21) { durezaEncoderEnMenu=20; }
    else {  return; }

    timeOutMenu = 0;  //reset del timeout
    FLAG_toqueteando = true; 
    static uint8_t indiceMenuSecundarioAnterior = 0;
    indiceMenuSecundario += _sentidoGiroEncoder;
    if ( indiceMenuSecundario<MINIMA_OPCION_SECUNDARIA ) { indiceMenuSecundario=MAXIMA_OPCION_SECUNDARIA; }
    if ( indiceMenuSecundario>MAXIMA_OPCION_SECUNDARIA ) { indiceMenuSecundario=MINIMA_OPCION_SECUNDARIA; }
    if ( indiceMenuSecundario!=indiceMenuSecundarioAnterior ) {
      mostrarOpcionMenuSecundario();  // muestra el texto ALARM, RAMPA, TRIGG, TIDAL... y lo que se necesite
      if(indiceMenuSecundario!=ID_MENU_RECRUIT){ mostrarValoresSecundarios(); }
      else{ pantallaPrint(15,2, "(Off)");  }
    }
  }

  //  CONTROL DE TIMEOUT PARA SALIR DE LA VISUALIZACION SECUNDARIAS
  if( FLAG_toqueteando ) {  //este flag es apra no incrementar timeOutMenu cuando no he tocado nada
    timeOutMenu++;
    if (timeOutMenu > TIMEOUT_MENU_SECUNDARIO ) {
      indiceMenuSecundario=ID_MENU_ALARMA; //RECUPERAMOS LA OPCION ALARM
      timeOutMenu = 0;        //reset del timeout, que es global (por ahora)
      FLAG_toqueteando = false;
      mostrarOpcionMenuSecundario();  // muestra el texto ALARM, RAMPA, TRIGG, TIDAL... y lo que se necesite
      mostrarValoresSecundarios();
    }
  }

  FLAG_primeraEntradaRun=false;
}



// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::ModoEdicionConsignas()
{
  /*  <<<<        CONTROL DE PRIMERA ENTRADA EN EL MODO    >>>>   */
  
  static uint8_t pulsacionEntradaModoEdicion = 0;
  static bool FLAG_recienllegados = false; 
  static uint8_t consignaEnEdicion  = ID_MENU_ALARMA;  //por defecto Aalrma, por poner una....
  //static bool FLAG_toqueteando = false; 
    
  if(modoMaquinaAnterior!=MODE_EDICION_CONSIGNAS){
    pulsacionEntradaModoEdicion = estadoPulsadoresActual;
    FLAG_recienllegados=true;
    setModo(MODE_EDICION_CONSIGNAS);
  
    pantallaPrint(0,0," -- MoDo EdIcIoN -- ");  //quizas se deba mantener el parpadeo de alarma?
    timeOutMenu=0;// reseteamos timeout an entrar en el modo por primera vez

    if(FLAG_MODO_DEBUG){
      Serial.println(F("MODE_EDICION_CONSIGNAS"));
      Serial.print(F("pulsacionEntrada: ")); 
      Serial.print(pulsacionEntradaModoEdicion);
      Serial.print(F("submenu: ")); Serial.println(indiceMenuSecundario); 
    }
  }


  /*  <<<<       GESTION DE TIMEOUT   y pulsacion no valida    >>>>   */

  if(tipoPulsacion==2){  //por ahora no reaccionar a pulsacion larga
    return;
  }

  timeOutMenu++;

  if(timeOutMenu >= TIMEOUT_MENU_EDICION_CONSIGNA){ 
    setModo(MODE_RUN); //nos volvemos por donde vinimos
    if(FLAG_MODO_DEBUG){
      Serial.println(F("Edicion de  consigna  cancelada por TIMEOUT"));
    }
    return;
  }


  /*  <<<<   DECIDIR QUE OPCION DE MENU DEBE PARPADEAR    >>>>   */
  static uint8_t posicionCursor_x1;
  if( FLAG_recienllegados==true ){   // && estadoPulsadoresActual>0 
    switch (estadoPulsadoresActual)
    { //localizamos la posicion de la etiqueta en parametrosConfiguracion
      case ID_PULSADOR_PEEP:
        consignaEnEdicion = ID_MENU_PEEP;
        posicionCursor_x1 = 0; 
        break;
      case ID_PULSADOR_PICO:
        consignaEnEdicion = ID_MENU_PICO;
        posicionCursor_x1 = 5; 
        break;
      case ID_PULSADOR_FR:
        consignaEnEdicion = ID_MENU_FR;
        posicionCursor_x1 = 10; 
        break;
      case ID_PULSADOR_ENCODER:
        consignaEnEdicion = indiceMenuSecundario;
        posicionCursor_x1 = 15; 
        break;    
    }
    /*  <<<<   SI ENTRAMOS CON EL SUBMENU ALARMAS, NO CORRESPONDE EDITAR    >>>>   */
    // Actualizamos el modo de la maquina y salimos
    //(esta toma de decision ya no es necesaria, nos discrimina MODE_RUN)
    if(consignaEnEdicion==ID_MENU_ALARMA){
      setModo(MODE_REVISION_ALARMAS);
      return;
    }

    /*  <<<<   MOSTRAR TEXTO ASOCIADO A LA CONSIGNA QUE ESTAMOS MODIFICANDO    >>>>   */
    pantallaPrint(0,1, 
                  parametrosConfiguracion[consignaEnEdicion].etiqueta); 
    lcdPrint("(");
    ultraitoa(parametrosConfiguracion[consignaEnEdicion].consigna, msg_valorItoA);
    lcdPrint(msg_valorItoA);
    lcdPrint(") --> ");
    //establEcer limites del encoder
    _valorEncoderMin = parametrosConfiguracion[consignaEnEdicion].min;
    _valorEncoderMax = parametrosConfiguracion[consignaEnEdicion].max;
    _valorEncoder = parametrosConfiguracion[consignaEnEdicion].consigna;
    _valorAnteriorEncoder = -1;
    FLAG_recienllegados=false;
    estadoPulsadoresActual = 0;
  }

  /*  <<<<   VISUALIZAR EL VALOR DE CONSIGNA MIENTRAS LO MODIFICAMOS   >>>>   */
  _valorEncoder+=_sentidoGiroEncoder;
  if(_valorEncoder != _valorAnteriorEncoder){
    checkEncoderLimits();
    ultraitoa(_valorEncoder, msg_valorItoA);
    pantallaPrint(15,1, msg_valorItoA);
    _valorAnteriorEncoder = _valorEncoder;
  }


  /*  <<<<   PARPADEO:  MOSTRAR/OCULTAR LA OPCION DE MENU EN USO    >>>>   */
  static bool FLAG_toggle_parpadeo = true;
  static uint16_t contadorParpadeoMenu=0;  //_contadorTicksReloj  
  contadorParpadeoMenu++;
  if( contadorParpadeoMenu > FRECUENCIA_PARPADEO_MENU ){
    contadorParpadeoMenu = 0;
    FLAG_toggle_parpadeo = !FLAG_toggle_parpadeo;
    if( FLAG_toggle_parpadeo){ 
      pantallaPrint(posicionCursor_x1,3, 
                parametrosConfiguracion[consignaEnEdicion].etiqueta); 
    }
    else{ pantallaPrint(posicionCursor_x1,3, etiquetaBorrar);}
  }


  /*  <<<<    ASIGNACION DE CONSIGNAS  TRAS SER MODIFICADAS    >>>>        */
  if( estadoPulsadoresActual >0 && !FLAG_recienllegados ){ 
    //si pulsamos sobre el boton de lo que hay editandose, se actualiza y salimos
    if(estadoPulsadoresActual == pulsacionEntradaModoEdicion){
      parametrosConfiguracion[consignaEnEdicion].consigna = _valorEncoder;
      setModo(MODE_RUN);
    }
    //cualquier otra tecla, provoca cancelacion del proceso y salida de este modo
    else{timeOutMenu = TIMEOUT_MENU_EDICION_CONSIGNA+1;} 
  }

  //FLAG_recienllegados=false;
}


// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::ModoRevisarAlarmas()

{
 /*  <<<<        CONTROL DE PRIMERA ENTRADA EN EL MODO    >>>>   */
  
  static uint8_t consignaEnEdicion  = ID_MENU_ALARMA;  //ESTAMOS REVISANDO ALARMAS
    
  if(modoMaquinaAnterior!=MODE_REVISION_ALARMAS){
    pantallaPrint(0,0, msg_lineaVacia);
    setModo(MODE_REVISION_ALARMAS);
    timeOutMenu=0;// reseteamos timeout an entrar en el modo por primera vez
    _valorEncoderMin = parametrosConfiguracion[consignaEnEdicion].min;
    _valorEncoderMax = parametrosConfiguracion[consignaEnEdicion].max;
    _valorEncoder = parametrosConfiguracion[consignaEnEdicion].consigna;
    _valorAnteriorEncoder = -1;
    if(FLAG_MODO_DEBUG){
      Serial.println(F("MODE_REVISION_ALARMAS"));
    }
  }


  /*  <<<<       GESTION DE TIMEOUT   y/o pulsacion no valida    >>>>   */

  if(tipoPulsacion==2){  //por ahora no reaccionar a pulsacion larga
    return;
  }

  timeOutMenu++;

  if(timeOutMenu >= TIMEOUT_MENU_EDICION_CONSIGNA){ 
    setModo(MODE_RUN); //nos volvemos por donde vinimos
    if(FLAG_MODO_DEBUG){
      Serial.println(F("Salida de revision de alarmas por TIMEOUT"));
    }
    return;
  }

  /*  <<<<   VISUALIZAR EL NOMBRE DE LA ALARMA    >>>>   */
  _valorEncoder+=_sentidoGiroEncoder;
  if(_valorEncoder != _valorAnteriorEncoder){
    checkEncoderLimits();
    ultraitoa(_valorEncoder, msg_valorItoA);
    pantallaPrint(0,0,"(");
    lcdPrint(msg_valorItoA);
    lcdPrint(") ");
    lcdPrint(lista_nombres_alarmas[listaDeAlarmas[_valorEncoder]]);
    _valorAnteriorEncoder = _valorEncoder;
  }



  /*  <<<<   PARPADEO:  MOSTRAR/OCULTAR LAOPCION DE MENU EN USO    >>>>   */
  static bool FLAG_toggle_parpadeo = true;
  static uint16_t contadorParpadeoMenu=0;  //_contadorTicksReloj  
  contadorParpadeoMenu++;
  if( contadorParpadeoMenu > FRECUENCIA_PARPADEO_MENU ){
    contadorParpadeoMenu = 0;
    FLAG_toggle_parpadeo = !FLAG_toggle_parpadeo;
    if( FLAG_toggle_parpadeo){ 
      pantallaPrint(15,3, parametrosConfiguracion[consignaEnEdicion].etiqueta); 
    }
    else{ pantallaPrint(15,3, etiquetaBorrar);}
  }

}



// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


void PANEL_CONTROL::ModoError()

{
  if(modoMaquinaAnterior!=MODE_ERROR){
    setModo(MODE_ERROR);
    if(FLAG_MODO_DEBUG){
      Serial.println(F("MODE_ERROR"));
    }
    pantallaPrint(0,0,"        MODO        ");  //quizas se deba mantener el parpadeo de alarma?
    pantallaPrint(0,1,"  NO IMPLEMENTADO   ");  //quizas se deba mantener el parpadeo de alarma?
    pantallaPrint(0,2,"                    ");  //quizas se deba mantener el parpadeo de alarma?
    pantallaPrint(0,3,"o ERROR DESCONOCIDO ");  //quizas se deba mantener el parpadeo de alarma?
  }
}



// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::pantallaPrint(uint8_t x, uint8_t y,  const char *info)
{ 
  
  #ifdef BUFFER_CIRCULAR
    // -- version buffer circular 
    // -- por defecto un buffer de 120 'posiciones'. Si te da problemas 
    //-- incrementalo en "lcd_virtual_mixta.h"
    lcd.locateBuffer(x, y);
    lcd.writeToBuffer(info);  //guarda en el buffer el texto que ha de mostrarse en el lcd
  #else
    // -- version pantalla virtual  Usa unicamente un buffer de 80 'posiciones'
    lcd.writePantallaVirtual(x, y, info);
  #endif
}


// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::lcdPrint( const char *info)
{ 
  
  #ifdef BUFFER_CIRCULAR
    lcd.writeToBuffer(info);  //guarda en el buffer el texto que ha de mostrarse en el lcd
  #else
    // -- version pantalla virtual  Usa unicamente un buffer de 80 'posiciones'
    return;
  #endif
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


PANEL_CONTROL::PANEL_CONTROL(bool pullup)


{
  _FLAG_PULLUP = pullup;
  
    // constructor, por defecto pulldown
    // usamos aqui los pines solo para el tema de pullup 
    // ya que no se controlarlo con los registros PORT

  if( _FLAG_PULLUP ){
    pinMode(PIN_peep, INPUT_PULLUP);
    pinMode(PIN_pico, INPUT_PULLUP);
    pinMode(PIN_rpm, INPUT_PULLUP);
    pinMode(PIN_CLK, INPUT_PULLUP);
    pinMode(PIN_DT, INPUT_PULLUP);
    pinMode(PIN_alarm, INPUT_PULLUP);
  }
  else{
    pinMode(PIN_peep, INPUT);
    pinMode(PIN_pico, INPUT);
    pinMode(PIN_rpm, INPUT);
    pinMode(PIN_CLK, INPUT);
    pinMode(PIN_DT, INPUT);
    pinMode(PIN_alarm, INPUT);
  } 
  _lecturaAnteriorPulsadores = 0;
}




// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//                                       BOTONERA
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm


uint8_t PANEL_CONTROL::readPin( void )      // SIN uso
{
  if( _FLAG_PULLUP ){
    return !(bitRead(PUERTO_PIN_PULSADOR_ENCODER, bitEncoderPULSADOR));
  }
  else{
    return (bitRead(PUERTO_PIN_PULSADOR_ENCODER, bitEncoderPULSADOR));
  }
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

uint8_t PANEL_CONTROL:: leerPulsadoresRaw(uint8_t intervalo)
{
  //esto habra que cambiarlo, porque parece que no estan en el mismo puerto  :(
  if( _FLAG_PULLUP ){
    _lecturaActualPulsadores = ~PINC&B0100111; //3 pulsadores + pulsador encoder
  }
  else{
    _lecturaActualPulsadores = PINC&B0100111; //3 pulsadores + pulsador encoder
  }


	/* NO existe pulsacion actual ni nada en curso*/
	if(_lecturaActualPulsadores == 0 && _lecturaAnteriorPulsadores == 0){
		return 0;      
	}

	/* INICIO de pulsacion */
	if(_lecturaActualPulsadores >0 && _lecturaAnteriorPulsadores == 0){
		_lecturaAnteriorPulsadores  = _lecturaActualPulsadores;
		return 0;      
	}

	/* Ha TERMINADO la pulsacion (o un rebote) Veamos que es... */
	if( _lecturaActualPulsadores == 0 && _lecturaAnteriorPulsadores >0 ){
    //Serial.print(_contadorInterno);     //DEBUG para ver lo que dura la pulsacion

		/* Detectar y descartar REBOTES */
    if ( _contadorInternoPulsador < PULSACION_CORTA){
			resetPulsador();
      //Serial.println(F("   << REBOTE >>"));     //DEBUG
			return 0;
		}

		/* Confirmada pulsacion LARGA */
    else if( _contadorInternoPulsador >= PULSACION_LARGA){ 
      //Serial.println(F("   LARGA"));     //DEBUG
			return 2;
		} 
      /* Confirmada pulsacion CORTA */
    else if( _contadorInternoPulsador >= PULSACION_CORTA){
      //Serial.println(F("   CORTA"));     //DEBUG
			return 1;
		} 
	}

  /*   DENTRO de una pulsacion (cuantificar su duracion con un contador)*/
  // es decir cuendo estamos en esta situacion: ( _estadoActual == 1 && _estadoAnterior == 1 )
  // _contadorInternoPulsador++;
  _contadorInternoPulsador+=intervalo;  //si usamos un intervalo de lectura, si no pues ++

  //  ( control de beep aqui parece que queda mas natural )
  // pero lo descartamos, porque las especificaciones de diseño
  // para esta maquina son que los beep se produzcan antes conbios aceptados
  // asi que o todas las pulsaciones deberan sonar. (por ejemplo las errroneas
  // o voluntariamente hechas para que se "invalide" una accion)
   if( (_contadorInternoPulsador >= PULSACION_CORTA  ) && 
       (_contadorInternoPulsador < PULSACION_CORTA+5) &&
       (FLAG_beep==false ) ){  
    //beep(80); //pitido de 80ms
  }

   if( (_contadorInternoPulsador >= PULSACION_LARGA  ) && 
       (_contadorInternoPulsador < PULSACION_LARGA+5) &&
       (FLAG_beep==false ) ){  
    //beep(280);  //pitido de 280ms
  }

  return 0;           //todavia estamos a medio leer...
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::resetPulsador() 
{
  // principalmente control y proteccion de rebotes (aunque tenga otras utilidades)
	_lecturaActualPulsadores = 0; 
	_lecturaAnteriorPulsadores = 0;
  _contadorInternoPulsador = 0;
}


// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::updatePulsadores(uint8_t intervalo)  
{
  tipoPulsacion = leerPulsadoresRaw(intervalo);
  if( tipoPulsacion > 0 ){
    _valorAnteriorEncoder = -1;         //ante nueva pulsacion ponemos el valor encoder fuera de rango
    estadoPulsadoresActual = _lecturaAnteriorPulsadores; //Ya que una pulsacion se detacta al soltar
    hayPrimeraPulsacionValida = _lecturaAnteriorPulsadores;   //Almacenamos  pulsacion a efectos de 
                                                                //aceptar o no consignas en edicion
    resetPulsador();  //reset de los tiemps y estado asociados a lectura RAW
  }
  else{ //no se ha detectado nada
    if( hayPrimeraPulsacionValida!=0 ){
      estadoPulsadoresAnterior = hayPrimeraPulsacionValida;
      hayPrimeraPulsacionValida=0;
    }
    estadoPulsadoresActual = 0;
  }
}



// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//                                       ENCODER                               
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm


int8_t PANEL_CONTROL::leerEncoder()
{
  static uint8_t estadoAnteriorCLK = 1;
  uint8_t estadoCLK = bitRead(PUERTO_PIN_ENCODER, bitEncoderCLK);    // //CLK
  
  if ( estadoCLK && !estadoAnteriorCLK ) {
    if (bitRead(PUERTO_PIN_ENCODER, bitEncoderDT))  { _sentidoGiroEncoder = 1;  }
    else { _sentidoGiroEncoder = -1;  } 
  }
  else{ _sentidoGiroEncoder = 0; }
  estadoAnteriorCLK = estadoCLK;   // Guardar valores para siguiente

  if( _sentidoGiroEncoder!=0 ){
    timeOutMenu=0;  //si manipulamos, es que estamos editando
  }
  return _sentidoGiroEncoder;
}


// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::mostrarValorEncoder()
{
  // este metodo muestra el valor que se esta editando a la derecha 
  //de la linea dos  ejemplo TIDAL <45>   37 // el 37 3es lo que mostramos y que podemos variar
  checkEncoderLimits();
  /* Refrescamos el valor editado por el encoder solo si ha cambiado */
  if(_valorEncoder !=_valorAnteriorEncoder){
    ultraitoa(_valorEncoder , msg_valorItoA);
    pantallaPrint(12,1, etiquetaBorrar);
    pantallaPrint(12,1, msg_valorItoA);
    _valorAnteriorEncoder = _valorEncoder;
  }
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::checkEncoderLimits()
{
  // este metodo muestra el valor que se esta editando a la derecha 
  //de la linea dos  ejemplo TIDAL <45>   37 // el 37 3es lo que mostramos y que podemos variar
  if(_valorEncoder<_valorEncoderMin){         // limite de seguridad inferior,
    _valorEncoder = _valorEncoderMin;         // (queda asignado en funcion de las consignas)
  }
  else if(_valorEncoder>_valorEncoderMax){    // limite de seguridad superior,
    _valorEncoder=_valorEncoderMax;           // (queda signado en funcion de las consignas)
  }
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


void PANEL_CONTROL::Fastitoa(unsigned int InputInteger, char* OutputString)
{
  unsigned char Digit;

  //TODO ensure that OutputString is long enough or cap the number of digits printer

  Digit=0;
  while(InputInteger>=10000)
  {
    InputInteger-=10000;
    Digit++;
  }
  OutputString[0]=Digit+0x30;
  Digit=0;
  while(InputInteger>=1000)
  {
    InputInteger-=1000;
    Digit++;
  }
  OutputString[1]=Digit+0x30;
  Digit=0;
  while(InputInteger>=100)
  {
    InputInteger-=100;
    Digit++;
  }
  OutputString[2]=Digit+0x30;
  Digit=0;
  while(InputInteger>=10)
  {
    InputInteger-=10;
    Digit++;
  }
  OutputString[3]=Digit+0x30;
  Digit=0;
  while(InputInteger>=1)
  {
    InputInteger--;
    Digit++;
  }
  OutputString[4]=Digit+0x30;
}