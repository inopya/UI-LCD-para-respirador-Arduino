/*
 * Version 1.0
 * Temporizador basado en millis()
 */
 
#ifndef Temporizador_inopya_h
#define Temporizador_inopya_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Temporizador_inopya
{
  /*  ------ public ------  */    
  public:
    Temporizador_inopya(void); 
	
	void begin(uint32_t tiempo_accion);	

    bool estado(void);
	uint32_t time(void);
    
	void stop(void);
	uint16_t call_time = 0;

  /*  ------ private ------  */              
  private:
    bool _FLAG_activo = false;
    unsigned long _start_time;
    unsigned long _last_uptate;
    unsigned long _actual_time;
    unsigned long _end_time;
    bool _FLAG_overflow = false;
    
    void _start(uint32_t _tiempo_tarea);  
};


#endif 

