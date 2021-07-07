# Qbo

Qbo is an open-source Robot created by '''theCorpora''' which is excellent to implement basic SAR research, IoRT prototyping and STEM.

# QBO control via Scrtachx

## To update the Scratch control:

1. Clone or unzip the repository over the **/home/pi/Documents** directory

1. Run the Scratch control startup script, to boot the 5 deamons required:

	`cd /home/pi/Documents/deamonsScripts`
	 
	`./QBO_scratch start` 
    
    To stop all processes associated with the Scratch control:
	
	`./QBO_scratch stop`
    
    To check which processes are running:
	
	`./lsqbo`
    
    There must be 5 processes: websocket_server, listen, say, PiCmd and feel

1. Open the browser and enter the following address:

	<http://scratchx.org/?url=https://thecorpora.github.io/QBOscratch/scratch_extension/robot_control.js#scratch>

1. From scratchx page open the example *test_2.sbx*    (File->Load Project)

# QBo commands

Once all the process are dead, you can do the following:

* pico2wave -l "es-ES" -2 /home/pi/Documents/pico2wave.wav "La vida es muy compleja querido amigo" && aaplay -D convertQBO /home/pi/Documents/pico2wave.wav
* python PiCmd.py -c say -t "Hola"
* python PiCmd.py -c nose -co blue
* python listen.py


# QBo guidelines

* Cuando arranca y se pone en modo interactivo, lo que hace es buscar una cara (findFace.py).  En ese punto si lo tocan en los sensores capacitivos de derecha e izquierda se mueve para esos lados.  Una vez que encuentra una cara, se pone la nariz en azul y sigue la cara (lo hace muy bien).
* Configuré todo el entorno Cloud para que use DialogFlow de Google (hasta Octubre).   Así cuando la nariz está en azul, abre el microfono, y lo que le digas lo convierte a un WAV que se lo pasa a DialogFlow desde donde recibe el SpeechToText y el texto para decir (según la configuración de DialogFlow, tipo IFTTT).  El programa que hace esto es QTalk.py.
* El dump del modo interactivo queda en interactiveMode.log
* Arreglé también el tema de las cámaras.  El problema era que había que limitar bien el tamaño de la imágen que se captura desde OpenCV para que sea sólo de 640x480.  Cambiando eso, se puede tomar sin problemas, en tiempo real, la info de las dos cámaras en stereo.  En el directorio donde están los scripts de python, están los scripts LeftEye, RightEye y Stereo.
* Está conectado a la WiFi ITBA-Segura.  Habría que volver a meter el script que ustedes habían armado para que el primer comando que tire sea la IP.  Esto igual lo tienen que ejecutar desde el propio código en python y no desde el bashrc.

* http://thecorpora.com/google-assistance-and-q-bo-interactive-mode/
* https://www.generationrobots.com/media/UserGuide-qbo.pdf
* http://scratchx.org/?url=https://thecorpora.github.io/QBOscratch/scratch_extension/robot_control.js#scratch

En https://www.luisllamas.es/programar-stm32-con-ide-de-arduino-y-st-link-v2/ describen como funciona y componentes del ST-Link v2. Dice tener compatibilidad con JTAG. 
https://hackaday.io/project/162597-st-link-clone-repurposing 

Sé que con el  ST-Link v2se puede programar la CIAA-Z3R0 (http://www.proyecto-ciaa.com.ar/devwiki/doku.php?id=desarrollo:ciaa-z3r0)
https://electronica.mercadolibre.com.ar/componentes-electronicos/stlink
Espero te sea de utilidad la informaciòn.


http://thecorpora.com/howto-hacking-updating-q-board-using-segger-mini-j-link-jtag-only-for-developers/

Este al parecer es la versión educacional de los programadores y debuggers de Segger. 
https://www.segger.com/products/debug-probes/j-link/models/j-link-edu-mini/

Como me habías comentado, las versiones profesionales de los programadores superan los USD 500, sin embargo al ser una versión recortada este cuesta aprox USD 20. 

Buscando por Amazon de USA, lo encontré a USD 30 + tax.
https://www.amazon.com/-/es/Segger-J-LINK-Mini-JTAG-Depurador/dp/B0758XRMTF/

Paso a paso para actualizar el firmware: http://thecorpora.com/community/q-board-arduino/firmware-source-code-available-guide/

