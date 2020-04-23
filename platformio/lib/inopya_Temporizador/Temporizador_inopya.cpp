/*
 * Version 1.0
 * Temporizador basado en millis()
 */

#include "Temporizador_inopya.h"


Temporizador_inopya::Temporizador_inopya(void)
{	
  _end_time = 0;
  _FLAG_activo = false;
}  


void Temporizador_inopya::begin(uint32_t tiempo_accion)  //tiempo recibido en millis
{ 
  if(_FLAG_activo == false){
	_start(tiempo_accion);
  }
}


void Temporizador_inopya::stop(void)
{
  _FLAG_activo = false;	
}


bool Temporizador_inopya::estado(void)
{
  if (_FLAG_activo == true){
	_actual_time = millis();
	call_time = _actual_time -_last_uptate;
	if(_FLAG_overflow == false && _actual_time > _end_time){
	  _FLAG_activo = false;
	}
	if(_FLAG_overflow == true){
	  if(_actual_time < _last_uptate){
		_FLAG_overflow = false;
	  }
	}
	_last_uptate = _actual_time;
  }
  return _FLAG_activo;
}


uint32_t Temporizador_inopya::time(void)
{
  return _actual_time;
}



void Temporizador_inopya::_start(uint32_t _tiempo_tarea)
{
  _start_time = millis();
  _last_uptate = _start_time;
  _end_time = _start_time + _tiempo_tarea -1;  	      	//tiempo en milisegundos, restamos 1 porque  estado() compara si '>'
  if(_end_time<_start_time){
	_FLAG_overflow = true;								//desbordamiento a la vista
  }
  _FLAG_activo = true; 
}
