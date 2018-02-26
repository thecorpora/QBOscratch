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

