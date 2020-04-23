# UI LCD para respirador Arduino v1
Interfaz de usuario mediante LCD 20x4 para respirador basado en Arduino

*Todas las tareas de lectura de pulsadores y encoder, asi como refrescos de pantalla optimizados para RT*

* Para pruebas, simula la entrada de datos mediante numeros generados de forma aleatoria.

- Responde a los pulsadores. 
- Implementada la distincion entre pulsación corta y pulsación larga, (aunque es posible que no sea de interes)
- Funciona el encoder.
- Usada funcion ultraitoa para convertir a cadenas de caracteres las cifras que se envian al lcd
	Jose Luis Perez Barrales 
	https://gitlab.com/joseluisperezbarrales/
	
- Correcta modificación de consignas mediante el encoder.
- Funcionando los pitidos de aviso de pulsación de teclado (y nuevas alarmas).
- Funciona el parpadeo del menu seleccionado duarante el tiempo que dura la edición. 
- Se cancela la edición sin realizar cambios si no se pulsa ninguna tecla durante de 5 segundos.
- Se realizan cambios en las consignas si se pulsa tecla para aceptar.
- Acceso a pulsadores y encoder mediante los bit del puerto al que estan conectados


**TODO**
- Falta por implementar la muestra en pantalla del listado de alarmas
- Mejorar el tema de la libreria LCD para ganarle algunos milisegundos
- Reorganizar y simplificar el acceso a puertos para facilitar posibles cambios en los pines utilizados
