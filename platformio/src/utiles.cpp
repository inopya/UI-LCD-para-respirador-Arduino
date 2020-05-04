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
  
//                *******    IMPLEMENTACION DE LOS METODOS DE CADA CLASE    *******

//   .............................................................................................
//   .............................................................................................
//   mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// */

#include "definiciones.h"
#include "utiles.h"
#include "pinout.h"   //que los pines al final vengan en los objetos !!

#include "lcd_virtual_v2.h"

//creaacion del objeto 'lcd con buffer de escritura', para uso interno de la clase panel de control
//LCD_CON_BUFFER  lcd(PIN_RS,PIN_E,PIN_D7,PIN_D6,PIN_D5,PIN_D4);//pines lcd(rs,e,d4,d5,d6,d7)

LCD_CON_BUFFER  lcd(PIN_RS,PIN_E,PIN_D4,PIN_D5,PIN_D6,PIN_D7);//pines lcd(rs,e,d4,d5,d6,d7)



// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// .............................................................................................
//                                    PANEL_CONTROL
// .............................................................................................
// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm




// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//      <<<  UPDATE  >>>     METODO DE ENTRADA DESDE EL PROGRAMA 
// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::update()
{
  _contadorTicksReloj++;

  lcd.lcdReadBuffer(); //obtiene un caracter del buffer de impresion y lo envia  al lcd

  updatePulsadores();  //esto actualiza las variables tipoPulsacion y _estadoActualPulsadores



  int8_t lecturaEncoder = leerEncoder();

  if(FLAG_MODO_EDICION){

    parpadeoMenuAlPulsar();     // iniciar el proceso de edicion de consignas/parpadeo del menu
    modificarConsigna();        // ver si hay actividad en el encoder y hay que modificar alguna consigna
  
    _valorEncoder+=lecturaEncoder;
    //Control de limites de las consignas. Se asignan al pulsar. Se controlan aqui
    if( _valorEncoder<_valorEncoderMin ) { _valorEncoder=_valorEncoderMin; }
    if( _valorEncoder>_valorEncoderMax ) { _valorEncoder=_valorEncoderMax; }

    if( lecturaEncoder!=0 || hayPrimeraPulsacionValida ){  //asi me evito trabajo inutil...
      
      if(indiceMenuSecundario!=4){
        mostrarValorEncoder();
      }
      else{
        recorrerAlarmas();
        //mostrarListadoAlarmas();  //esta la llmaré desde dentro de la anterior??
                                    //o mejor aun fusionarlas
      }
      
      
    }
  
  }
  else{  //no estamos editando un menu, mostramos opciones del menu seleccionable del encoder, si procede
    if(indiceMenuSecundario!=4){
      timeOutMenuSecundario++;
      //   RECUPERAMOS LA OPCION ALARM
      if(timeOutMenuSecundario>5000){  //sacar esto como parametro?
        indiceMenuSecundario=4;
        mostrarOpcionMenuSecundario();  // muestra el texto ALARM, RAMPA, TRIGG, TIDAL... y lo que se necesite
        mostrarValoresSecundarios();    // muestra su "consigna" asociada entre parentesis
      }
    }
    if(lecturaEncoder!=0){
      static uint8_t indiceMenuSecundarioAnterior = 0;
      indiceMenuSecundario += lecturaEncoder;
      if (indiceMenuSecundario<4){indiceMenuSecundario=7;}
      if (indiceMenuSecundario>7){indiceMenuSecundario=4;}
      if(indiceMenuSecundario!=indiceMenuSecundarioAnterior){
        mostrarOpcionMenuSecundario();  // muestra el texto ALARM, RAMPA, TRIGG, TIDAL... y lo que se necesite
        mostrarValoresSecundarios();    // muestra su "consigna" asociada entre parentesis
      }
    }
  }

  
  if(_contadorTicksReloj%1000==0 && indiceMenuSecundario==4 ){   //  ¿¿ ha pasado "un segundo" en mi universo ??
      //actualizarContadorAlarmas();   //   - en ese caso refrescar las alarmas si es necesario
      mostrarValoresSecundarios();   //Mejor con estafuncion
  }

  if(_contadorTicksReloj%5000==0){   //  ¿¿ han pasado "5 segundos" en mi universo ??
      mostrarLineaValoresActuales(); //   - en ese caso refrescar los datos si es necesario
  }
}


// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

//                        METODO PENDIENTE DE IMPLEMENTAR  
//        por ahora es solo un contador que cambia con el giro del encoder


void PANEL_CONTROL::mostrarListadoAlarmas()
{

  if( _valorEncoder<1 ){
    _valorEncoder=1;  //limite de seguridad inferior
  }
  else if( _valorEncoder>contadorALARMAS ){
    _valorEncoder=contadorALARMAS;
  }
  
  if( _valorEncoder !=_valorAnteriorEncoder ){

    mostrarLineaAlarma();             //ahora mismo es atrezo :)
    
    if(_valorEncoder<10){ //ahora mismo es atrezo :)
      mostrarMensajeAlarma(_valorEncoder); //ahora mismo es atrezo :)
    } //ahora mismo es atrezo :)
    else{ //ahora mismo es atrezo :)
      uint8_t i = random(0, 10); //ahora mismo es atrezo :)
      mostrarMensajeAlarma(i); //ahora mismo es atrezo :)
    }

    _valorAnteriorEncoder = _valorEncoder;
    /* PARCHE TEMPORAL PARA PRUEBAS */
    ultraitoa(_valorEncoder , msg_valorItoA);
    pantallaPrint(15,0, lista_menus[0]);
    pantallaPrint(16,0, msg_valorItoA);

  }

}




// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::modificarConsigna()
{

  //esta funcion muestra en nombre de la consigna que se esta modificando 
  // entre <>  su valor actual y a la derecha el valor nuevo que estamos
  //introducioendo con el encoder  ejemplo:  TIDAL <50>    48

  if(tipoPulsacion==2){  //no reaccionar a pulsacion larga
    return;
  }


  //static  
  char buffer_msg_temporal[11];
  static uint8_t posicionCursor_x;

  //primera pulsacion , inicio de edicion de consigas
  if(estadoPulsadores>0 && _pulsacionAnterior==0){
    
    if(FLAG_MODO_DEBUG){
      Serial.println(F("Inicio de manipulacion consigna... "));
    }
   
    switch (estadoPulsadores) {
      case 1:  //rpm
        strncpy(buffer_msg_temporal, lista_menus[3],11);
        strncat(buffer_msg_temporal, "<",11);
        ultraitoa(consignaRPM,msg_valorItoA);
        strncat(buffer_msg_temporal, msg_valorItoA,11);
        strncat(buffer_msg_temporal, ">",11);
        posicionCursor_x = 0;
        _valorEncoder=consignaRPM;
        _valorEncoderMin = consignaRPMminima;
        _valorEncoderMax = consignaRPMmaxima; 
         
        break;
      case 2:  //pico
        strncpy(buffer_msg_temporal, lista_menus[2],11);
        strncat(buffer_msg_temporal, "<",11);
        ultraitoa(consignaPICO,msg_valorItoA);
        strncat(buffer_msg_temporal, msg_valorItoA,11);
        strncat(buffer_msg_temporal, ">",11);
        posicionCursor_x = 0;
        _valorEncoder=consignaPICO;
        _valorEncoderMin = consignaPICOminima;
        _valorEncoderMax = consignaPICOmaxima;
        break;
      case 4:   //peep >> msg_parpadeo = msg_peep;
        strncpy(buffer_msg_temporal, lista_menus[1],11);
        strncat(buffer_msg_temporal, "<",11);
        ultraitoa(consignaPEEP,msg_valorItoA);
        strncat(buffer_msg_temporal, msg_valorItoA,11);
        strncat(buffer_msg_temporal, ">",11);
        posicionCursor_x=0;
        _valorEncoder=consignaPEEP;
        _valorEncoderMin = consignaPEEPminima;
        _valorEncoderMax = consignaPEEPmaxima;
        break;
      case 32:   //A ver, esto es case 32_elegir submenu
        switch (indiceMenuSecundario){
          case 4:  //ALARM
            _valorEncoder = contadorALARMAS;   // Así revisamos las alarma de la mas reciete a la mas antigüa
            _valorEncoderMin = 0;
            _valorEncoderMax = contadorALARMAS;
            mostrarListadoAlarmas();
            return;  //volver aqui para evitar imprimir cosas indebidas
            break;
          case 5: //TIDAL
            strncpy(buffer_msg_temporal, lista_menus[5],11);
            strncat(buffer_msg_temporal, "<",11);
            ultraitoa(consignaTIDAL,msg_valorItoA);
            strncat(buffer_msg_temporal, msg_valorItoA,11);
            strncat(buffer_msg_temporal, ">",11);
            posicionCursor_x=0;
            _valorEncoder = consignaTIDAL;   // Así revisamos las alarma de la mas reciete a la mas antigüa
            _valorEncoderMin = consignaTIDALminimo;
            _valorEncoderMax = consignaTIDALmaximo;
            break;
          case 6: //TRIGGER
            strncpy(buffer_msg_temporal, lista_menus[6],11);
            strncat(buffer_msg_temporal, "<",11);
            ultraitoa(consignaTRIGGER,msg_valorItoA);
            strncat(buffer_msg_temporal, msg_valorItoA,11);
            strncat(buffer_msg_temporal, ">",11);
            posicionCursor_x=0;
            _valorEncoder = consignaTRIGGER;   // Así revisamos las alarma de la mas reciete a la mas antigüa
            _valorEncoderMin = consignaTRIGGERminimo;
            _valorEncoderMax = consignaTRIGGERmaximo;
            break; 
          case 7:  //RAMPA
            strncpy(buffer_msg_temporal, lista_menus[7],11);
            strncat(buffer_msg_temporal, "<",11);
            ultraitoa(consignaRAMPA,msg_valorItoA);
            strncat(buffer_msg_temporal, msg_valorItoA,11);
            strncat(buffer_msg_temporal, ">",11);
            posicionCursor_x=0;
            _valorEncoder = consignaRAMPA;   // Así revisamos las alarma de la mas reciete a la mas antigüa
            _valorEncoderMin = consignaRAMPAminimo;
            _valorEncoderMax = consignaRAMPAmaximo;
            break; 
          }          

        break;
    
    }

    _valorAnteriorEncoder = -1;  //nos aseguramos que vamos a refrescar el encoder a la primera 
    pantallaPrint(posicionCursor_x,1, buffer_msg_temporal);
    pantallaPrint(10,1, lista_menus[0]);
    return;
  }

  if( estadoPulsadores>0 && estadoPulsadores==_pulsacionAnterior ){
    // EDICION DE CONSIGNAS ACEPTADA
    asignarCosignaYterminarEdicion();
    return;
  }

  if( estadoPulsadores>0 && estadoPulsadores!=_pulsacionAnterior ){
    // EDICION DE CONSIGNAS CANCELADA
    if(FLAG_MODO_DEBUG){
      Serial.println(F("Edicion de consigna CANCELADA por USUARIO"));
    }
    
    contador_parpadeos = 200;  //un numero elevado, (uint_8), para romper el ciclo de edicion
    _pulsacionAnterior = 0;   //se borra tambien en la salida de edicion, pero no importa...
  }
}




// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx



//                METODOS  MAS O MENOS ESTABLES Y QUE NO SE VANA A TOCAR POR AHORA



// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm






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




// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//                                       PANTALLA
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

void PANEL_CONTROL::mostrarLineaCabecera()
{
  char msg_texto[21]= "RespiMenu v1  inopya";   //10
  pantallaPrint(0,0, msg_texto);
  return;
}

void PANEL_CONTROL::mostrarLineaAlarma()
{
  char msg_texto[21]= "  >>  ALARMAS n     ";   //10
  pantallaPrint(0,0, msg_texto);
  return;
}

void PANEL_CONTROL::mostrarLineaMenus()
{
  pantallaPrint(0,3,  lista_menus[1]);  //msg_peep
  pantallaPrint(5,3,  lista_menus[2]);  //msg_pico
  pantallaPrint(10,3, lista_menus[3]);  //msg_rpf
  pantallaPrint(15,3, lista_menus[4]);  //msg_alarm
  return;
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::mostrarOpcionMenuSecundario()
{
  //esto solo muestra en pantalla el texto del menu secundadio
  //y espera que pulsemos
  pantallaPrint(15,3, lista_menus[indiceMenuSecundario]);
  timeOutMenuSecundario = 0;
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::mostrarLineaConsignas()
{
  ultraitoa(consignaPEEP , msg_valorItoA);
  pantallaPrint(0,1, lista_menus[0]);  //erase_msg
  pantallaPrint(0,1, msg_valorItoA);

  ultraitoa(consignaPICO , msg_valorItoA);
  pantallaPrint(5,1, lista_menus[0]);
  pantallaPrint(5,1, msg_valorItoA);

  ultraitoa(consignaRPM , msg_valorItoA);
  pantallaPrint(10,1, lista_menus[0]);
  pantallaPrint(10,1, msg_valorItoA);

  pantallaPrint(15,1, lista_menus[0]);
  return;
}


// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void  PANEL_CONTROL::mostrarLineaValoresActuales()
{
  if (valorActualPeep^valorAnteriorPeep){
    ultraitoa(valorActualPeep, msg_valorItoA);
    pantallaPrint(0,2, msg_valorItoA);
  }
  if (valorActualPico^valorAnteriorPico){
    ultraitoa(valorActualPico, msg_valorItoA);
    pantallaPrint(5,2, msg_valorItoA);
  }
  if(valorActualRpm^valorAnteriorRpm ){
    ultraitoa(valorActualRpm, msg_valorItoA);
    pantallaPrint(10,2, msg_valorItoA);
  }
  valorAnteriorPeep = valorActualPeep;
  valorAnteriorPico = valorActualPico;
  valorAnteriorRpm = valorActualRpm;
  
  //return;

  //PARA IR SIMULANDO ALARMAS
  //si los valores se salen de rango los vuelvo a la consigna
  if( (valorActualPeep<consignaPEEPminima) || 
                              (valorActualPeep>consignaPEEPmaxima) ){
    contadorALARMAS++;
    valorActualPeep = consignaPEEP;
  }
  if( (valorActualPico<consignaPICOminima) || 
                              (valorActualPico>consignaPICOmaxima) ){
    contadorALARMAS++;
    valorActualPico = consignaPICO;
  }
  if( (valorActualRpm<consignaRPMminima) || 
                              (valorActualRpm>consignaRPMmaxima) ){
    contadorALARMAS++;
    valorActualRpm = consignaRPM;
  }

}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::parpadeoMenuAlPulsar()
{

  if(tipoPulsacion==2){  //por ahora no reaccionar a pulsacion larga
    return;
  }

  static uint16_t duracion_parpadeo_en_curso=0;  //lleva la cuenta de tiempo de cada medio parpadeo en curso
  
  static bool FLAG_toggle_parpadeo = false;
  duracion_parpadeo_en_curso++;

  static  char buffer_msg_parpadeo[6] = "-----";
  static uint8_t posicionCursor_x;


  //si update nos llama y tiene algo apra nosotros...
  if( estadoPulsadores>0 ){
    contador_parpadeos = 0;
    if(FLAG_MODO_DEBUG){
      if(hayPrimeraPulsacionValida){
        Serial.println("manipulacion teclado -> Entrando");
      }
      else{
        Serial.println("manipulacion teclado -> Saliendo");
      }
    }
    switch (estadoPulsadores) {
      case 1:  //rpm
        strncpy(buffer_msg_parpadeo, lista_menus[3], 5);
        posicionCursor_x = 10;
        break;
      case 2:  //pico
        strncpy(buffer_msg_parpadeo, lista_menus[2], 5);
        posicionCursor_x = 5;
        break;
      case 4:   //peep 
        //msg_parpadeo = msg_peep;
        strncpy(buffer_msg_parpadeo, lista_menus[1], 5);
        posicionCursor_x = 0;
        break;
      case 32:   //alarm o menu secundario 
        //msg_parpadeo = msg_peep;
        strncpy(buffer_msg_parpadeo, lista_menus[indiceMenuSecundario], 5);
        posicionCursor_x = 15;
        break;
    } 
  }

  if(contador_parpadeos >= numeroMaximoParpadeos){ 
    FLAG_MODO_EDICION = false;
    _pulsacionAnterior = 0;
    indiceMenuSecundario = 4;  //por defecto dejamos la opcion del encoder en ALARM

// uint32_t inicio = micros();

    //recuperamos la liena de consignas
    mostrarLineaConsignas();

    //por ahora a veces tengo efectos raros y al salir  no se actualiza el que debe
    //pantallaPrint(posicionCursor_x,3, buffer_msg_parpadeo);  //msg_parpadeo
    //así que mejor refresco toda la linea  de menus :()
    mostrarLineaMenus();

    mostrarLineaCabecera();  //atrezo temporal...

// uint32_t fin = micros();
// uint32_t tiempo_empleado = (fin-inicio);
// Serial.print("tiempo al soltar 392 utiles.cpp: ");
// Serial.println(tiempo_empleado);

    if(contador_parpadeos<200){  // para evirar un doble pitido si salimos aceptando
      beep();  //pequeño beep al terminar la edicion de un campo
      //en caso contrario >=200, nos conformamos con el 'bip' que hizo el boton
      if(FLAG_MODO_DEBUG){
        Serial.println(F("Edicion de  consigna  cancelada por TIMEOUT"));
      }
    }
    return;
  }

  // control de parpadeo, cuando toca mostrar u ocultar la opcion de menu en uso
  if(duracion_parpadeo_en_curso>_duracion_parpadeo){
    contador_parpadeos++;
    duracion_parpadeo_en_curso = 0;
    FLAG_toggle_parpadeo = !FLAG_toggle_parpadeo;
    if(FLAG_toggle_parpadeo){
      pantallaPrint(posicionCursor_x,3, lista_menus[0]);
    }
    else{
      pantallaPrint(posicionCursor_x,3, buffer_msg_parpadeo);  //msg_parpadeo
    }
  }
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::ultraitoa( int valor , char *salida )
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

void PANEL_CONTROL::actualizarContadorAlarmas()
{
  return;  //ha quedado obsoleta
  // static  char buffer_msg_alarmas[6];
  // static uint8_t contadorALARMAS_anterior=250;  //para que sea distinto del cero inicial :)
  // if(contadorALARMAS!=contadorALARMAS_anterior){
  //   strncpy(buffer_msg_alarmas, "(",6);
  //   ultraitoa(contadorALARMAS, msg_valorItoA);
  //   strncat(buffer_msg_alarmas, msg_valorItoA,6);
  //   strncat(buffer_msg_alarmas, ")",6);

  //   pantallaPrint(15,2, lista_menus[0]);  //borraar
  //   pantallaPrint(15,2, buffer_msg_alarmas);

  //   contadorALARMAS_anterior = contadorALARMAS;
  //   //beep(500);  // ¿un pitido largo para advertir de una nueva alarma? 
  // }
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::mostrarValoresSecundarios()
{
  //muestra entre parenteris en la linea 2 (0,1,2..) el valor de alarmas
  // o de consignas secundarias
  //static  
  char buffer_msg[6];
  int8_t valorParaMostrar = 0;
  static int8_t ultimoMostrado = 0;

  switch (indiceMenuSecundario)
  {
  case 4:
    valorParaMostrar=contadorALARMAS;
    break;
   case 5:
    valorParaMostrar=consignaTIDAL;
    break; 
  case 6:
    valorParaMostrar=consignaTRIGGER;
    break;
  case 7:
    valorParaMostrar=consignaRAMPA;
    break;
  }

  if(valorParaMostrar!=ultimoMostrado){
    ultraitoa(valorParaMostrar, msg_valorItoA);
    strncpy(buffer_msg, "(",6);
    strncat(buffer_msg, msg_valorItoA,6);
    strncat(buffer_msg, ")",6);

    //pantallaPrint(15,2, lista_menus[0]);  //borrar por si quedan restps de lo anterior
    pantallaPrint(15,2, buffer_msg);
    ultimoMostrado = valorParaMostrar;
  }
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::setupPantalla()
{
  lcd.lcdInit();

  // lcd.lcdLocate(0,0);
  // //          "0123456789ABCDEFGIJK"
  // lcd.lcdWrite2buffer("RespiMenu v1  inopya");

  mostrarLineaCabecera();     //mostar cabecera  (en la linea 0)
  mostrarLineaConsignas();    //mostar consignas (en la linea 1)
  mostrarLineaMenus();        //mostar menus     (en la linea 3)

  if(FLAG_MODO_DEBUG){
    Serial.println("SISTEMA DE CONTROL INICIADO");
  }
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::pantallaPrint(uint8_t x, uint8_t y,  const char *info)
{
  lcd.lcdWrite2buffer(x, y, info);  //guarda en el buffer el texto que ha de mostrarse en el lcd
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
  _estadoAnteriorPulsadores = 0;
}

void PANEL_CONTROL::mostarEdicionConsignas()   //  SIN uso
{
  return;
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

uint8_t PANEL_CONTROL:: leerPulsadores()
{
  //esto habra que cambiarlo, porque parece que no estan en el mismo puerto  :(
  if( _FLAG_PULLUP ){
    _estadoActualPulsadores = ~PINC&B0100111; //3 pulsadores + pulsador encoder
  }
  else{
    _estadoActualPulsadores = PINC&B0100111; //3 pulsadores + pulsador encoder
  }

  /*   para control y mantenimiento de el beep que se inicia al soltar (si no es rebote)   */
  if(FLAG_beep==true){
    beep();  
  }


	/* NO existe pulsacion actual ni nada en curso*/
	if(_estadoActualPulsadores == 0 && _estadoAnteriorPulsadores == 0){
		return 0;      
	}

	/* INICIO de pulsacion */
	if(_estadoActualPulsadores >0 && _estadoAnteriorPulsadores == 0){
		_estadoAnteriorPulsadores  = _estadoActualPulsadores;
		return 0;      
	}

	/* Ha TERMINADO la pulsacion (o un rebote)*/
	if( _estadoActualPulsadores == 0 && _estadoAnteriorPulsadores >0 ){
    //beep_v2(false);   //termianr beep al soltar pulsador (si no habia acabado)
    //Serial.print(_contadorInterno);     //DEBUG para ver loq ue dura la pulsacion

		/* Detectar y descartar REBOTES */
    if ( _contadorInternoPulsador < PULSACION_CORTA){
			reset();
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
  //        ( _estadoActual == 1 && _estadoAnterior == 1 )
  _contadorInternoPulsador++;

  //   ( control de beep aqui parece que queda mas natural )
   if( (_contadorInternoPulsador >= PULSACION_CORTA  ) && 
       (_contadorInternoPulsador < PULSACION_CORTA+5) &&
       (FLAG_beep==false ) ){  
    beep(80); //pitido de 80ms
  }

   if( (_contadorInternoPulsador >= PULSACION_LARGA  ) && 
       (_contadorInternoPulsador < PULSACION_LARGA+5) &&
       (FLAG_beep==false ) ){  
    beep(280);  //pitido de 280ms
  }

  return 0;           //todavia estamos a medio leer...
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::reset() 
{
	_estadoActualPulsadores = 0; 
	_estadoAnteriorPulsadores = 0;
  _contadorInternoPulsador = 0;
}


// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::updatePulsadores()  
{
  tipoPulsacion = leerPulsadores();
  if( tipoPulsacion > 0 ){
    _valorAnteriorEncoder = -1;                   //ante nueva pulsacion ponemos el valor encoder fuera de rango
    if( FLAG_MODO_EDICION==false ){             //renombrar a FLAG_MODO_EDICION?
      hayPrimeraPulsacionValida = _estadoAnteriorPulsadores;        //Almacenamos  pulsacion a efectos de 
    }                                             //aceptar o no consignas en edicion
    estadoPulsadores = _estadoAnteriorPulsadores; //Ya que una pulsacion se detacta al soltar
    reset();                                      //es el estado anterior el que nos dice
    FLAG_MODO_EDICION = true;                 //la tecla que se habia pulsado
  }
  else{
    if( hayPrimeraPulsacionValida!=0 ){
      _pulsacionAnterior = hayPrimeraPulsacionValida;
      hayPrimeraPulsacionValida=0;
      if( FLAG_MODO_DEBUG ){
        Serial.print(F("Carga de _pulsacionAnterior para comparaciones: "));
        Serial.println(_pulsacionAnterior);
      }
    }
    estadoPulsadores = 0;
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
    if( contador_parpadeos%2==0 ){
      contador_parpadeos=0;  //mantene el parpadeo mientras el usuario manipula
    }
    // else{
    //   contador_parpadeos=1;
    // }
  }
  return _sentidoGiroEncoder;
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::asignarCosignaYterminarEdicion()
{
  switch ( _pulsacionAnterior ) {
    case 1:
      consignaRPM = _valorEncoder;
      break;
    case 2:
      consignaPICO = _valorEncoder;
      break;
    case 4: 
      //estadoPulsadores=0; 
      consignaPEEP = _valorEncoder;
      break;
    case 32: 
      //estadoPulsadores=0; 
      switch ( indiceMenuSecundario )
      {
      case 5:
        consignaTIDAL = _valorEncoder;
        break;
      case 6:
        consignaTRIGGER = _valorEncoder;
        break;
      case 7:
        consignaRAMPA = _valorEncoder;
        break;
      }

      break;
  }

  indiceMenuSecundario = 4;     //para recuperar el valor de las alarmas
  mostrarValoresSecundarios();  //para recuperar el valor de las alarmas

  if( FLAG_MODO_DEBUG ){
    Serial.println(F("Edicion de consignas ACEPTADA"));
  }    

  contador_parpadeos = 200;  //un numero elevado, (uint_8), para romper el ciclo de edicion
  _pulsacionAnterior = 0;   //se borra tambien en la salida de edicion, pero no importa...
}


// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//   ¿¿  FUSIONAR al menos en parte  con "recorrerAlarmas()"  ??

void PANEL_CONTROL::mostrarValorEncoder()
{
  // este metodo muestra el valor que se esta editando a la derecha 
  //de la linea dos  ejemplo TIDAL <45>   37 // el 37 3es lo que mostramos y podemos variar
  if(_valorEncoder<_valorEncoderMin){         // limite de seguridad inferior,
    _valorEncoder = _valorEncoderMin;         // (queda asignado en funcion de las consignas)
  }
  else if(_valorEncoder>_valorEncoderMax){    // limite de seguridad superior,
    _valorEncoder=_valorEncoderMax;           // (queda signado en funcion de las consignas)
  }
  /* Refrescamos el valor editado por el encoder solo si ha cambiado */
  if(_valorEncoder !=_valorAnteriorEncoder){
    ultraitoa(_valorEncoder , msg_valorItoA);
    pantallaPrint(12,1, lista_menus[0]);
    pantallaPrint(12,1, msg_valorItoA);
    _valorAnteriorEncoder = _valorEncoder;
  }
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL:: recorrerAlarmas()
//   ¿¿  FUSIONAR al menos en parte  con "mostrarListadoAlarmas()"  ??
{
  // este metodo recorre la lsita de alarmas (solo el numero)

  char buffer_msg[6];

  static int8_t ultimoMostrado = 0;

  if(_valorEncoder<_valorEncoderMin){         // limite de seguridad inferior,
    _valorEncoder = _valorEncoderMin;         // (queda asignado en funcion de las consignas)
  }
  else if(_valorEncoder>_valorEncoderMax){    // limite de seguridad superior,
    _valorEncoder=_valorEncoderMax;           // (queda signado en funcion de las consignas)
  }
  /* Refrescamos el valor editado por el encoder solo si ha cambiado */

  if(_valorEncoder!=ultimoMostrado){
    ultraitoa(_valorEncoder, msg_valorItoA);
    strncpy(buffer_msg, "(",6);
    strncat(buffer_msg, msg_valorItoA,6);
    strncat(buffer_msg, ")",6);

    //pantallaPrint(15,2, lista_menus[0]);  //borrar por si quedan restps de lo anterior
    pantallaPrint(15,2, buffer_msg);
    ultimoMostrado = _valorEncoder;
  }



}

// void PANEL_CONTROL::mostrarMensajeAlarma (const char * const * message)
// {
// const char * ptr = reinterpret_cast<const char *>(pgm_read_ptr (message));  // pointer to message
// Serial.println(reinterpret_cast<const __FlashStringHelper *>(ptr));    // and print it
// } 

void PANEL_CONTROL::mostrarMensajeAlarma (uint8_t i)
{
    // crear una copia de la alarma indicada

    //char alarmaParaMostrar[20];
    //const char * puntero_menu_pgm = (const char *)pgm_read_byte_far(&(mensajes_alarmas[i]));
    //const char * puntero_menu_pgm = (PGM_P)pgm_read_byte_far(&(mensajes_alarmas[i]));
    //strcpy_P(alarmaParaMostrar, puntero_menu_pgm);
    //pantallaPrint(0, 0,  "pepito tenia 1 balon");
    //Serial.println(mensajes_alarmas[i]);
    //mensaje_alarma_Type alarmaParaMostrar;
    //char alarmaParaMostrar20[20];
    //alarmaParaMostrar=mensajes_alarmas[i];
    //const char (*puntero)[20]=mensajes_alarmas;
    ///const char (*puntero)[10][20]=&mensajes_alarmas;
    //alarmaParaMostrar20 = pgm_read_byte_far(puntero);

    //alarmaParaMostrar = (*puntero)[i];
    //memcpy_P(alarmaParaMostrar20, mensajes_alarmas[i],20);

    //Serial.println (alarmaParaMostrar);
    //Serial.println (mensajes_alarmas[i][0]);
    //strcpy_P(alarmaParaMostrar, (char*)pgm_read_word(&(mensajes_alarmas[i]))); 
    //Serial.println (mensajes_alarmas[i]);



}
