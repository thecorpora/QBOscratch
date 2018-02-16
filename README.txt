Para actualizar el control por Scratch:

1.- Copiar los archivos del directorio "Python projects" dentro de "/home/pi/Documents/Python projects"
	Dar permisos de ejecución a los archivos python:
		cd Python\ projects
		chmod 777 *.py

2.- Copiar los archivos del directorio "websocketServer" a "/home/pi/Documents/websocketServer"
	Permisos para websocket_server:
		cd websocketServer
		chmod 777 websocket_server

3.- Copiar el directorio "deamonsScrpits" a "/home/pi/Documents"
	Dar permiso de ejecución a los scrpits de "deamonsScrpits":
		cd deamonsScripts
		chmod 777 *

4.- Crear un directorio para las pipes de comunicación interna.
	mkdir /home/pi/Documents/pipes

5.- Ejecutar el script de inicio del control Scratch, para arrancar los 5 deamons necesarios:
	cd /home/pi/Documents/deamonsScripts
	./QBO_scratch start
    Para detener todos los procesos asociados al control Scratch:
	./QBO_scratch stop
    Para comrpobar los procesos que se están ejecutando:
	./lsqbo
    Tiene que haber 5 procesos:  websocket_server, listen, say, PiCmd y feel

6.- Abrir el navegador y entrar la siguiente dirección:
	http://scratchx.org/?url=https://rperaltadiaz.github.io/Qbo/robot_control.js#scratch