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

  updatePulsadores();

  if(FLAG_parpadeo_edicion){
    //FLAG_MODO_EDICION = true;
    parpadeoMenuAlPulsar();     // iniciar el prod¡cesod e ediciond e consignas/parpadeo del menu
    modificarConsigna();        // ver si hay actividad en el encoder y hay que modificar alguna consigna
  
    _valorEncoder+=leerEncoder();

    if(FLAG_MOSTAR_ALARMAS==true){
      mostrarListadoAlarmas();
      return;
    }
    else{
      mostrarValorEncoder();
    }
  
  }

  
  if(_contadorTicksReloj%1000==0){   //  ¿¿ ha pasado "un segundo" en mi universo ??
      actualizarContadorAlarmas();   //   - en ese caso refrescar las alarmas si es necesario
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

    _valorAnteriorEncoder = _valorEncoder;
    /* PARCHE TEMPORAL PARA PRUEBAS */
    ultraitoa(_valorEncoder , msg_valorItoA);
    pantallaPrint(15,1, erase_msg);
    pantallaPrint(16,1, msg_valorItoA);
  }

}




// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::modificarConsigna()
{

  if(tipoPulsacion==2){  //no reaccionar a pulsacion larga
    return;
  }


  static  char buffer_msg_parpadeo[11];
  static uint8_t posicionCursor_x;

  //primera pulsacion , inicio de edicion de consigas
  if(estadoPulsadores>0 && _pulsacionAnterior==0){
    
    if(FLAG_MODO_DEBUG){
      Serial.println(F("Inicio de manipulacion consigna... "));
    }
   
    FLAG_MOSTAR_ALARMAS = false;

    switch (estadoPulsadores) {
      case 1:  //rpm
        strncpy(buffer_msg_parpadeo, msg_rpm,11);
        strncat(buffer_msg_parpadeo, "<",11);
        ultraitoa(consignaRPM,msg_valorItoA);
        strncat(buffer_msg_parpadeo, msg_valorItoA,11);
        strncat(buffer_msg_parpadeo, ">",11);
        posicionCursor_x = 0;
        _valorEncoder=consignaRPM;
        break;
      case 2:  //pico
        strncpy(buffer_msg_parpadeo, msg_pico,11);
        strncat(buffer_msg_parpadeo, "<",11);
        ultraitoa(consignaPICO,msg_valorItoA);
        strncat(buffer_msg_parpadeo, msg_valorItoA,11);
        strncat(buffer_msg_parpadeo, ">",11);
        posicionCursor_x = 0;
          _valorEncoder=consignaPICO;
        break;
      case 4:   //peep >> msg_parpadeo = msg_peep;
        strncpy(buffer_msg_parpadeo, msg_peep,11);
        strncat(buffer_msg_parpadeo, "<",11);
        ultraitoa(consignaPEEP,msg_valorItoA);
        strncat(buffer_msg_parpadeo, msg_valorItoA,11);
        strncat(buffer_msg_parpadeo, ">",11);
        posicionCursor_x=0;
          _valorEncoder=consignaPEEP;
        break;
      case 32:   //alarmas >> msg_parpadeo = msg_peep;
        _valorEncoder = contadorALARMAS;   // Así revisamos las alarma de la mas reciete a la mas antigüa
        FLAG_MOSTAR_ALARMAS = true;
        //return;
    }

    if(FLAG_MOSTAR_ALARMAS){
      return;
    }
    pantallaPrint(posicionCursor_x,1, buffer_msg_parpadeo);
    pantallaPrint(10,1, erase_msg);
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
  if(FLAG_beep==false){
    duracion=tiempo;
  }
  FLAG_beep = true;
  static uint16_t controlBeep;
  if(controlBeep<duracion){ //cada ciclo, en teoria 1 ms
    controlBeep++;
  }
  else{
    FLAG_beep = false;
    controlBeep = 0;
  }
  digitalWrite(PIN_zumbador, FLAG_beep);
}




// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//                                       PANTALLA
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm


void PANEL_CONTROL::mostrarLineaMenus()
{
  pantallaPrint(0,3,  msg_peep);
  pantallaPrint(5,3,  msg_pico);
  pantallaPrint(10,3, msg_rpm);
  pantallaPrint(15,3, msg_alarm);
  return;
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::mostrarLineaConsignas()
{
  ultraitoa(consignaPEEP , msg_valorItoA);
  pantallaPrint(0,1, erase_msg);
  pantallaPrint(0,1, msg_valorItoA);

  ultraitoa(consignaPICO , msg_valorItoA);
  pantallaPrint(5,1, erase_msg);
  pantallaPrint(5,1, msg_valorItoA);

  ultraitoa(consignaRPM , msg_valorItoA);
  pantallaPrint(10,1, erase_msg);
  pantallaPrint(10,1, msg_valorItoA);

  pantallaPrint(15,1, erase_msg);
  return;
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::parpadeoMenuAlPulsar()
{

  if(tipoPulsacion==2){  //por ahora no reaccionar a pulsacion larga
    return;
  }

  static uint16_t duracion_parpadeo_en_curso=0;  //lleva la cuenta de tiempo de cada medio parpadeo en curso
  //contador_parpadeos lo usaremos para contar ciclos de parpadeo
  static bool FLAG_toggle_parpadeo = false;
  duracion_parpadeo_en_curso++;

  static  char buffer_msg_parpadeo[6] = "-----";
  static uint8_t posicionCursor_x;


  //si update nos llama y tiene algo apra nosotros...
  if(estadoPulsadores>0){
    contador_parpadeos = 0;
    if(FLAG_MODO_DEBUG){
      Serial.println("Inicio de manipulacion teclado...");
    }
    switch (estadoPulsadores) {
      case 1:  //rpm
        strncpy(buffer_msg_parpadeo, msg_rpm, 5);
        posicionCursor_x = 10;
        break;
      case 2:  //pico
        strncpy(buffer_msg_parpadeo, msg_pico, 5);
        posicionCursor_x = 5;
        break;
      case 4:   //peep 
        //msg_parpadeo = msg_peep;
        strncpy(buffer_msg_parpadeo, msg_peep, 5);
        posicionCursor_x = 0;
        break;
      case 32:   //peep 
        //msg_parpadeo = msg_peep;
        strncpy(buffer_msg_parpadeo, msg_alarm, 5);
        posicionCursor_x = 15;
        break;
    } 
  }

  if(contador_parpadeos >= numeroMaximoParpadeos){ 
    FLAG_parpadeo_edicion = false;
    _pulsacionAnterior = 0;

    //recuperamos la liena de consignas
    mostrarLineaConsignas();

    //por ahora a veces tengo efectos raros y al salir  no se actualiza el que debe
    //pantallaPrint(posicionCursor_x,3, buffer_msg_parpadeo);  //msg_parpadeo
    //así que mejor refresco toda la linea  de menus :()
    mostrarLineaMenus();

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
      pantallaPrint(posicionCursor_x,3, erase_msg);
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
      *salida = ' ';                // si no hay centenas, sustituimos por un espacio
    }
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

    *salida = '0'+contcentenas;   // añadimos las decenas (reusamos la variable "contcentenas")
    salida++;

    /*   UNIDADES   */
    *salida = '0' + valor;        // añadimos las unidades
    salida++;
    *salida=0;                    // añadir fin de cadena
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::actualizarContadorAlarmas()
{
  static  char buffer_msg_alarmas[6];
  static uint8_t contadorALARMAS_anterior=250;  //para que sea distinto del cero inicial :)
  if(contadorALARMAS!=contadorALARMAS_anterior){
    strncpy(buffer_msg_alarmas, "(",6);
    ultraitoa(contadorALARMAS, msg_valorItoA);
    strncat(buffer_msg_alarmas, msg_valorItoA,6);
    strncat(buffer_msg_alarmas, ")",6);

    pantallaPrint(15,2, erase_msg);
    pantallaPrint(15,2, buffer_msg_alarmas);

    contadorALARMAS_anterior = contadorALARMAS;
    beep(500);  // ¿un pitido largo para advertir de una nueva alarma? 
  }
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::setupPantalla()
{
  /* Inicializar el LCD con el número de  columnas y filas del LCD */
  lcd.begin(20,4);

  //lcd.clear();

  lcd.setCursor(0,0);
  //          "0123456789ABCDEFGIJK"
  lcd.print(F("RespiMenu v1  inopya"));

  mostrarLineaConsignas();   //mostar consignas (en la linea 1)

  lcd.setCursor(0,3);
  //          "0123456789ABCDEFGIJK"   //gia de espacio disponible
  lcd.print(F("PEEP PICO RPM  ALARM"));
  if(FLAG_MODO_DEBUG){
    Serial.print("peep: ");Serial.print(consignaPEEP); 
    Serial.print(" ,pico: ");Serial.print(consignaPICO); 
    Serial.print(" ,rpm: ");Serial.println(consignaRPM); 
  }
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::pantallaPrint(uint8_t x, uint8_t y,  char *info)
{
  //return;  //DEBUG, para medir tiempos sin la pantalla
  lcd.setCursor(x, y);
  lcd.print(info);

}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::pantallaPrint2(uint8_t x, uint8_t y, const char *info[])  //  SIN uso
{
  lcd.setCursor(x, y);
  //          "0123456789ABCDEFGIJK"
  //lcd.print(info);
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

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

uint8_t PANEL_CONTROL::leerPulsadores()   
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
    beep(80); //pitido de 60ms
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

uint8_t PANEL_CONTROL::get()   //  SIN uso
{
  return(estadoPulsadores);

  switch (estadoPulsadores) {
    case 1:
      //estadoPulsadores=0; //puede ser interesante borrar la leer o dejarlo para algo??
      return 1;    //sale del switch
    case 2:
      //estadoPulsadores=0;
      return 2;
    case 4: 
      //estadoPulsadores=0; 
      return 4;
  }
  return 0;
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::updatePulsadores()
{
  tipoPulsacion = leerPulsadores();
  static uint8_t teclaOK=0;
  if( tipoPulsacion > 0 ){
    _valorAnteriorEncoder = -1;     //ante nueva pulsacion ponemos el valor encoder fuera de rango
    if(FLAG_parpadeo_edicion==false){         //renombrar a FLAG_MODO_EDICION?
      teclaOK = _estadoAnteriorPulsadores;    //Almacenamos  pulsacion a efectos de 
    }                                         //aceptar o no consignas en edicion
    estadoPulsadores = _estadoAnteriorPulsadores; //Ya que una pulsacion se detacta al soltar
    reset();                                      //es el estado anterior el que nos dice
    FLAG_parpadeo_edicion = true;                 //la tecla que se habia pulsado
  }
  else{
    if(teclaOK!=0){
      _pulsacionAnterior = teclaOK;
      teclaOK=0;
      if(FLAG_MODO_DEBUG){
        Serial.print(F("Carga de _pulsacionAnterior: "));
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

  if(_sentidoGiroEncoder!=0){
    if(contador_parpadeos%2==0){
      contador_parpadeos=0;  //mantene el parpadeo mientras el usuario manupula
    }
    else{
      contador_parpadeos=1;
    }
  }
  return _sentidoGiroEncoder;
}

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void PANEL_CONTROL::asignarCosignaYterminarEdicion()
{
  switch (_pulsacionAnterior) {
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
  }

  if(FLAG_MODO_DEBUG){
    Serial.println(F("Edicion de consignas ACEPTADA"));
  }    

  contador_parpadeos = 200;  //un numero elevado, (uint_8), para romper el ciclo de edicion
  _pulsacionAnterior = 0;   //se borra tambien en la salida de edicion, pero no importa...
}


// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//   ¿¿  FUSIONAR al menos en parte  con "mostrarListadoAlarmas()""  ??

void PANEL_CONTROL::mostrarValorEncoder()
{
  if(_valorEncoder<40){         // limite de seguridad inferior,
    _valorEncoder = 40;         // (deberemos asignarlo en funcion de las consignas)
  }
  else if(_valorEncoder>120){   // limite de seguridad superior,
    _valorEncoder=120;          // (deberemos asignarlo en funcion de las consignas)
  }
  /* Refrescamos el valor editado por el encoder solo si ha cambiado */
  if(_valorEncoder !=_valorAnteriorEncoder){
    ultraitoa(_valorEncoder , msg_valorItoA);
    pantallaPrint(12,1, erase_msg);
    pantallaPrint(12,1, msg_valorItoA);
    _valorAnteriorEncoder = _valorEncoder;
  }
}