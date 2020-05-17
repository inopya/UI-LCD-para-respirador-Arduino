
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



#ifndef lcd_virtual_h
#define lcd_virtual_h



typedef struct
	{
		char comandoCaracter;
		unsigned char tipo;
	}
	lcdBufferData;  

class LCD_CON_BUFFER
{
 
    public:

        LCD_CON_BUFFER ( uint8_t pin_rs, uint8_t pin_e, uint8_t pin_d4, uint8_t pin_d5, uint8_t pin_d6, uint8_t pin_d7);

        uint32_t contadorTicks = 0;                 // para pruebas
 

        static const uint8_t bufferLimit = 120;     // tamaño del buffer destinado a la pantalla
                                                    // equivale a 80 caracteres + 40 locates 
      
        lcdBufferData buffer_LCD[bufferLimit];      // reserva de memoria para el buffer de caracteres del lcd
		
		

        uint8_t writeIndex = 0;   // Global porque lo necesitan un par de metodos
        uint8_t readIndex = 0;    // Global por pereza (podria ser 'static' dentro de el metodo 'read')

        void lcdRawWrite( char cmd, uint8_t cmdtype );
        void lcdClear();
        void lcdInit();

		void locateBuffer( char x, char y );
		void writeToBuffer( const char* str );
        void readBuffer();
    private:
		bool FLAG_fin_buffer = false;
	
};

#endif


