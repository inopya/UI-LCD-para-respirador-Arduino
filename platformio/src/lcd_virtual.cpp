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


// solucion adaptada para usar Lcd20x4 en modo 4 bits sin libreria.
// Envia los caracteres a imprimir a un buffer cuando una impresion es solicitada
// por el programa y posteriormente se aprovechan pequeños intervalos de tiempo
// entre llamadas de un ciclo principal para ir mostrando 'poco a poco' dichos caracteres en pantalla
// de manera que la tarea de imprimir consuma su tiempo necesario en pequeños fragmentos.
//
// basada en:
//       LCD driver respirator, de Alberto ferrer
//       https://gitlab.com/alfersi/lcdhal/-/tree/master
// y en:
//       https://forum.arduino.cc/index.php?topic=287644.0




#include <Arduino.h>
#include "lcd_virtual.h"
#include "pinout.h"
#include "definiciones.h"




#define SetBit(data,y)    data |= (1 << y) 

//LiquidCrystal            lcd(9,8,4,5,6,7);            // pines arduino
//LiquidCrystal            lcd(rs,e,d4,d5,d6,d7);       // pines lcd



//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//***************************************************************************************************
//         FUNCION DE CONFIGURACION
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
LCD_CON_BUFFER::LCD_CON_BUFFER ( uint8_t pin_rs, uint8_t pin_e, uint8_t pin_d4, uint8_t pin_d5, uint8_t pin_d6, uint8_t pin_d7)
{
    pinMode(pin_d4,OUTPUT);
    pinMode(pin_d5,OUTPUT);
    pinMode(pin_d6,OUTPUT);
    pinMode(pin_d7,OUTPUT);
    pinMode(pin_e,OUTPUT);
    pinMode(pin_rs,OUTPUT);
}



//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//                          BLOQUE DE FUNCIONES AUXILIARES
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 




//===================================================================================================
//      ACCESO 'RAW' PARA ESCRITURA EN EL LCD  ( MODO 4 bits )
//===================================================================================================

void LCD_CON_BUFFER::lcdRawWrite( char cmd, uint8_t cmdtype )
{
  
  bitWrite(PORTH,6,cmdtype);



  bitWrite(PORTG,5,getBit(cmd,4));
  bitWrite(PORTE,3,getBit(cmd,5));
  bitWrite(PORTH,3,getBit(cmd,6));
  bitWrite(PORTH,4,getBit(cmd,7));

  // bitWrite(PORTG,5,(cmd & 0b00010000)>>4);
  // bitWrite(PORTE,3,(cmd & 0b00100000)>>5);
  // bitWrite(PORTH,3,(cmd & 0b01000000)>>6);
  // bitWrite(PORTH,4,(cmd & 0b10000000)>>7);

  bitWrite(PORTH,5,0);  
  //delayMicroseconds(1);
  bitWrite(PORTH,5,1);
  //delayMicroseconds(1);
  bitWrite(PORTH,5,0);
  //delayMicroseconds(70);  

  bitWrite(PORTG,5,getBit(cmd,0));
  bitWrite(PORTE,3,getBit(cmd,1));
  bitWrite(PORTH,3,getBit(cmd,2));
  bitWrite(PORTH,4,getBit(cmd,3));

  // bitWrite(PORTG,5,(cmd & 0b00000001)>>0);
  // bitWrite(PORTE,3,(cmd & 0b00000010)>>1);
  // bitWrite(PORTH,3,(cmd & 0b00000100)>>2);
  // bitWrite(PORTH,4,(cmd & 0b00001000)>>3); 

  bitWrite(PORTH,5,0);  
  //delayMicroseconds(1);
  bitWrite(PORTH,5,1);
  //delayMicroseconds(1);
  bitWrite(PORTH,5,0);
  //delayMicroseconds(37);      // (30-100 us) pausa Obligatoria solo si escribimos más de un caracter por llamada.
                                // En caso contrario este tiempo necesario para estabilizar, seria "automatico"
                                // ya que se supone que las llamadas a esta funcion no son inferiores de 100 us.
                                // Todo dependera de que queramos priorizar:
                                // por ejemplo escribir en bloques de 5 caracteres nos supondría unas pausas por
                                // bloque de unos 0.185 ms, pero haria muy eficiente el uso del buffer.
                                // Por otra parte llamadas de un solo caracter no supondrian nigun retraso extra
                                // en los tiempos de impresion, pero podria ocasionar problemas de buffer
                                // si hacemos impresiones muy continuas en el lcd, (cosa no deseable 
                                // ya que las tasas de refrescos de este tipo de pantallas es algo lenta 
                                // y se apreciarian molestos parpadeos).
}


//===================================================================================================
//      BORRADO COMPLETO DEL LCD
//===================================================================================================

void LCD_CON_BUFFER::lcdClear()
{
  lcdRawWrite(0x01,0);
  delayMicroseconds(50);    // recordar que el display tarda unos 30us en estabilizarse 
                            //tras recibir un comando
}


//===================================================================================================
//      INICIALIZAR EL LCD
//===================================================================================================

void LCD_CON_BUFFER::lcdInit()
{
  lcdRawWrite(0x33,0);
  delayMicroseconds(50);      // 50-100
  lcdRawWrite(0x32,0);
  delayMicroseconds(50);      // 50-100
  lcdRawWrite(0x28,0);        // modo escritura, 4bits
  delayMicroseconds(50);      // 50-200
  lcdRawWrite(0x0C,0);
  delayMicroseconds(50);      // 50-100
  lcdRawWrite(0x06,0);
  delayMicroseconds(50);      // 50-100
  lcdRawWrite(0x01,0);        // lcd_clear
  delayMicroseconds(50);      // 50-100
}


//===================================================================================================
//      ALMACEANR EN BUFFER UN CAMBIO DE LA POSICION DEL CURSOR
//===================================================================================================

void LCD_CON_BUFFER::lcdLocate( char x, char y )
{
  if      (y == 0){ x = (x + 0x80); }
  else if (y == 1){ x = (x + 0xC0); }
  else if (y == 2){ x = (x + 0x94); }
  else if (y == 3){ x = (x + 0xD4); }
  else{ return;} //condicion de error

  buffer_LCD[writeIndex].comandoCaracter = x;
  buffer_LCD[writeIndex].tipo = 0;
  writeIndex++;
  bufferFreeSpace--;
  if(writeIndex>=bufferLimit){ writeIndex = 0; }
}


//===================================================================================================
//      IMPRIMIR CARACTERES EN EL BUFFER DEL LCD
//===================================================================================================

void LCD_CON_BUFFER::lcdWrite2buffer( const char *str )
{

  uint8_t controlLineSize=0;
  //for(int i=0; str[i]>0; i++){
  for(int i=0; str[i]!='\0'; i++){
      controlLineSize++;
  }
  //controlamos que no haya textos superiores a 20 caracteres o mayores que el buffer disponible
  if( controlLineSize>20 || controlLineSize>bufferFreeSpace ){
    //Serial.print(F("ERROR, La cadena mide: "));
    //Serial.print(controlLineSize);
    //Serial.print(F(",  Buffer libre: "));
    //Serial.println(bufferFreeSpace);
    //ver como gestionamos posibles errores de este tipo, por ahora pasamos de esa cadena
    //OJO, nos quedaria almacenado el locate asociado a ella :(
    return;
  }

  //for(int i=0; str[i]>0; i++){
  for(int i=0; str[i]!='\0'; i++){
    buffer_LCD[writeIndex].comandoCaracter = str[i];
    buffer_LCD[writeIndex].tipo = 1;
    writeIndex++;
    bufferFreeSpace--;
    if(writeIndex>=bufferLimit){ writeIndex=0;}  
  }
}


//===================================================================================================
//      RECUPERAR DATOS DEL BUFFER Y MOSTRARLOS EN EL LCD
//===================================================================================================

void LCD_CON_BUFFER::lcdReadBuffer()
{
  uint8_t contadorBuffer=0;   //para buscar cosas a imprimir como maximo en todo el buffer
  uint8_t contadorImpresiones = 0;  // podemos usarlo para 'exigir' que se imprimima mas de un caracter por ciclo
                                    // usando un numero mayor de uno en el ciclo While
                                    // pero OJO! en ese caso debemos contar con ciertas pausas en la zona
                                    // de acceso raw al lcd ( delayMicroseconds(37) en el final de la funcion,
                                    // para que se estabilicen los datos de cada caracter

  while( contadorImpresiones<1 && contadorBuffer<bufferLimit ){
    if( buffer_LCD[readIndex].tipo <2 ){
      lcdRawWrite(buffer_LCD[readIndex].comandoCaracter, buffer_LCD[readIndex].tipo);
      buffer_LCD[readIndex].tipo = 5;  //tipo nulo, asi borramos lo que ya hemos enviado el lcd
      bufferFreeSpace++;
      contadorImpresiones++;
    }
    //Serial.print(F("    Tick: "));Serial.print(contadorTicks);
    //Serial.print(F("Caracter: "));Serial.println(buffer_LCD[readIndex].comandoCaracter);
    //Serial.print(F("    tipo: "));buffer_LCD[readIndex].tipo);
    
    readIndex++;
    contadorBuffer++;
    if( readIndex>=bufferLimit ){ readIndex=0; }
  }
//  if(contadorImpresiones==0){
//    Serial.print(F("Nada en este ciclo: "));Serial.println(contadorImpresiones);
//  }
//  else{
//    Serial.print(F("En este ciclo: "));Serial.println(contadorImpresiones);
//  }
}