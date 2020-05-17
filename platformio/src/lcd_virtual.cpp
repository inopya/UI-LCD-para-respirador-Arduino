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




#define SetBit(data,y)    data |= (1 << y) 



//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//***************************************************************************************************
//         FUNCION DE CONFIGURACION
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
LCD_CON_BUFFER::LCD_CON_BUFFER ( uint8_t pin_rs, uint8_t pin_e, uint8_t pin_d4, 
                                      uint8_t pin_d5, uint8_t pin_d6, uint8_t pin_d7)
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
//funcion pensada para usar el LCD en modo "solo 4bits"
//pero escribimos despreocupadamente al puerto sobre los 8 biits, 
//tenerlo en cuenta para no usar el resto de bit o modificar esta funcion

  bitWrite(PUERTO_LCD_CONTROL, BIT_RS, cmdtype);

  PUERTO_LCD_DATA = (cmd>>4);   // recuerda, estamos usando solo los 4 primeros bits del puerto
                                // cargamos en ellos el nible de mayor peso de nuestro dato

  bitWrite(PUERTO_LCD_CONTROL,BIT_E,0);  
  bitWrite(PUERTO_LCD_CONTROL,BIT_E,1);
  bitWrite(PUERTO_LCD_CONTROL,BIT_E,0);

  PUERTO_LCD_DATA = cmd;        // recuerda, estamos usando solo los 4 primeros bits del puerto
                                // cargamos en ellos el nible de nemor peso de nuestro dato


  bitWrite(PUERTO_LCD_CONTROL,BIT_E,0);  
  bitWrite(PUERTO_LCD_CONTROL,BIT_E,1);
  bitWrite(PUERTO_LCD_CONTROL,BIT_E,0);
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
  writeIndex = 0;  
  readIndex = 0;
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
//      ALMACENAR EN BUFFER UN CAMBIO DE LA POSICION DEL CURSOR
//===================================================================================================

void LCD_CON_BUFFER::locateBuffer( char x, char y )
{
  if      (y == 0){ x = (x + 0x80); }
  else if (y == 1){ x = (x + 0xC0); }
  else if (y == 2){ x = (x + 0x94); }
  else if (y == 3){ x = (x + 0xD4); }
  else{ return;} //condicion de error

  buffer_LCD[writeIndex].comandoCaracter = x;
  buffer_LCD[writeIndex].tipo = 0;
  writeIndex++;
  if(writeIndex>=bufferLimit){ writeIndex = 0; }
}


//===================================================================================================
//      IMPRIMIR CARACTERES EN EL BUFFER DEL LCD
//===================================================================================================

void LCD_CON_BUFFER::writeToBuffer( const char *str )
{

  //for(int i=0; str[i]>0; i++){
  for(int i=0; str[i]!='\0'; i++){
    buffer_LCD[writeIndex].comandoCaracter = str[i];
    buffer_LCD[writeIndex].tipo = 1;
    writeIndex++;
    if(writeIndex==bufferLimit){
		  writeIndex=0;
		  FLAG_fin_buffer = true;
	  }
    if(FLAG_fin_buffer && writeIndex>readIndex){
      writeIndex = readIndex;
		  return;
	  } 	
  }
}


//===================================================================================================
//      RECUPERAR DATOS DEL BUFFER Y MOSTRARLOS EN EL LCD
//===================================================================================================

void LCD_CON_BUFFER::readBuffer()
{
	 if (!FLAG_fin_buffer  && readIndex == writeIndex){
		return;
	 }
	 else{
		lcdRawWrite(buffer_LCD[readIndex].comandoCaracter, buffer_LCD[readIndex].tipo);
		readIndex++;
	 }
	 if (readIndex == bufferLimit){
		readIndex = 0;
		FLAG_fin_buffer = false;
	 }	 
}	


