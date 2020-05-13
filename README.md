# UI LCD para respirador Arduino v1.3
Interfaz de usuario mediante LCD 20x4 para respirador basado en Arduino

*Todas las tareas de lectura de pulsadores y encoder, asi como refrescos de pantalla optimizados para RT*

* Para pruebas, simula la entrada de datos mediante numeros generados de forma aleatoria.

- Responde a los pulsadores. 
- Implementada la distincion entre pulsación corta y pulsación larga, (aunque es posible que no sea de interes)
- Funciona el encoder.
- Usada funcion ultraitoa (aunque modificada) para convertir a cadenas de caracteres las cifras que se envian al lcd
	Jose Luis Perez Barrales 
	https://gitlab.com/joseluisperezbarrales/
	
- Correcta modificación de consignas mediante el encoder. Establecidas variables para el control de limites de cada consigna.
- Funcionando los pitidos de aviso de pulsación de teclado (y nuevas alarmas).
- Funciona el parpadeo del menu seleccionado duarante el tiempo que dura la edición. 
- Se cancela la edición sin realizar cambios si no se pulsa ninguna tecla durante de 5 segundos.
- Se realizan cambios en las consignas si se pulsa tecla para aceptar.
- Acceso a pulsadores y encoder mediante los bit del puerto al que estan conectados
- Sustituido el acceso al lcd mediante libreria por una solucion de acceso en modo 'raw'  adaptada para usar Lcd20x4 en modo 4 bits. Los textos a imprimir se envian a un buffer cuando una impresion es solicitada por el programa y posteriormente se aprovechan pequeños intervalos de tiempo entre llamadas de un ciclo principal para ir mostrando 'poco a poco' dichos caracteres en pantalla de manera que la tarea de imprimir consuma su tiempo necesario en pequeños intervalos.
	basada en:
		LCD driver respirator, de Alberto ferrer
		https://gitlab.com/alfersi/lcdhal/-/tree/master
	y en:
		https://forum.arduino.cc/index.php?topic=287644.0

- Añadidas mas opciones de menu para configuracion de parametros 'secundarios'
- Implementada la muestra en pantalla del listado de alarmas
- Disponible pantalla de setup para parametro basicos/iniciales de la maquina,
- Disponible  FLAG_MODO_DEBUG, que permite mostrar por serial ciertas informaciones utiles para pruebas

**TODO** 
- Reorganizar y simplificar el acceso a puertos para facilitar posibles cambios en los pines utilizados
- Pasar cadenas de texto a Flash

