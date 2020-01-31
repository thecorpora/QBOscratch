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


