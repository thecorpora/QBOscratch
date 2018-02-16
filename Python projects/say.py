#!/usr/bin/env python2.7

from multiprocessing import Process, Queue
import subprocess
import os
import errno
import time
import serial
import binascii
#import QboCmd
import sys
import time
import yaml


def SayFromFile():
	print("Opening FIFO...")
    	while True:
		fifo = os.open(FIFO_say, os.O_RDONLY)
		data = os.read(fifo, 100)
		os.close(fifo)

		if data:
			config = yaml.safe_load(open("/home/pi/Documents/Python projects/config.yml"))
 
			print('Read: "{0}"'.format(data))
	                if config["languaje"] == "english":
		        	speak = "espeak -ven+f3 \"" + data + "\" --stdout  | aplay -D convertQBO"
                	elif config["languaje"] == "spanish":
		        	speak = "espeak -v mb-es2 -s 120 \"" + data + "\" --stdout  | aplay -D convertQBO"
		        print "Talk: " + speak
			
        		result = subprocess.call("../deamonsScripts/QBO_listen stop", shell = True)
        		result = subprocess.call(speak, shell = True)
        		result = subprocess.call("../deamonsScripts/QBO_listen start", shell = True)

#============================================================================================================

FIFO_say = '/home/pi/Documents/pipes/pipe_say'

#
#try:
#    os.mkfifo(FIFO_say)
#except OSError as oe: 
#    if oe.errno != errno.EEXIST:
#        raise

while True:
        SayFromFile()
