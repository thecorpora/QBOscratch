#!/usr/bin/env python2.7

import time
import fileinput
import sys
import os
import errno
import yaml
import subprocess

# read config file
config = yaml.safe_load(open("/home/pi/Documents/config.yml"))
print "CONFIG " + str(config)


if (config["language"] == "spanish"):
	speak = "pico2wave -l \"es-ES\" -w /home/pi/Documents/pico2wave.wav Hola && aplay -D convertQBO /home/pi/Documents/pico2wave.wav"
else:
        speak = "pico2wave -l \"en-US\" -w /home/pi/Documents/pico2wave.wav Hello && aplay -D convertQBO /home/pi/Documents/pico2wave.wav"
result = subprocess.call(speak, shell = True)
time.sleep(0.5)

if config["startWith"] == "scratch":
	if (config["language"] == "spanish"):
		text = "estoy en modo scratch."
        	speak = "pico2wave -l \"es-ES\" -w /home/pi/Documents/pico2wave.wav \"" + text + "\" && aplay -D convertQBO /home/pi/Documents/pico2wave.wav"
        else:
        	text = "I'm in scratch mode."
		speak = "pico2wave -l \"en-US\" -w /home/pi/Documents/pico2wave.wav \"" + text + "\" && aplay -D convertQBO /home/pi/Documents/pico2wave.wav"

        result = subprocess.call(speak, shell = True)

	result = subprocess.call("/home/pi/Documents/deamonsScripts/QBO_scratch start > /home/pi/scratchMode.log 2>&1", shell = True)

elif config["startWith"] == "interactive":
        if (config["language"] == "spanish"):
                text = "estoy en modo interactivo. Un momento, por favor."
                speak = "pico2wave -l \"es-ES\" -w /home/pi/Documents/pico2wave.wav \"" + text + "\" && aplay -D convertQBO /home/pi/Documents/pico2wave.wav"
        else:
                text = "I'm in interactive mode. Please wait."
                speak = "pico2wave -l \"en-US\" -w /home/pi/Documents/pico2wave.wav \"" + text + "\" && aplay -D convertQBO /home/pi/Documents/pico2wave.wav"

        result = subprocess.call(speak, shell = True)

	result = subprocess.call("/home/pi/Documents/Python\ projects/PiFaceFast.py > /home/pi/PiFaceFastMode.log 2>&1 &", shell = True)

