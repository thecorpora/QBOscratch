# Control de QBO mediante Scrtachx

## Para actualizar el control por Scratch:

1.- Clonar o descomprimir el repositorio sobre el directorio /home/pi/Documents

2.- Ejecutar el script de inicio del control Scratch, para arrancar los 5 deamons necesarios:
	`cd /home/pi/Documents/deamonsScripts`
	`./QBO_scratch start` 
    Para detener todos los procesos asociados al control Scratch:
	`./QBO_scratch stop`
    Para comrpobar los procesos que se están ejecutando:
	`./lsqbo`
    Tiene que haber 5 procesos:  websocket_server, listen, say, PiCmd y feel

3.- Abrir el navegador y entrar la siguiente dirección:
	<http://scratchx.org/?url=https://thecorpora.github.io/QBOscratch/scratch_extension/robot_control.js#scratch>
