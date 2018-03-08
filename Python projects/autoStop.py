#!/usr/bin/env python2.7
# -*- coding: latin-1 -*-

import time
import fileinput
import sys
import os
import errno
import yaml
import subprocess

# fichero config, contiene languaje
config = yaml.safe_load(open("/home/pi/Documents/config.yml"))
print "CONFIG " + str(config)

if (config["language"] == "spanish"):
	text = "Adíos"
        speak = "pico2wave -l \"es-ES\" -w /home/pi/Documents/pico2wave.wav \"" + text + "\" && aplay -D convertQBO /home/pi/Documents/pico2wave.wav"
else:
	text = "Good bye"
        speak = "pico2wave -l \"en-US\" -w /home/pi/Documents/pico2wave.wav \"" + text + "\" && aplay -D convertQBO /home/pi/Documents/pico2wave.wav"
result = subprocess.call(speak, shell = True)
time.sleep(0.5)

result = subprocess.call("/home/pi/Documents/deamonsScripts/QBO_scratch stop > /home/pi/sampleStop.log 2>&1", shell = True)

