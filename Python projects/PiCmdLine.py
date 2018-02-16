#!/usr/bin/env python2.7


from multiprocessing import Process, Queue
import time
import fileinput
import thread
import readline
import serial

#import cv2
#import binascii
import QboCmd
import sys
import os
import errno
import yaml
import pdb

# FIFO init.
FIFO_cmd = '/home/pi/Documents/pipes/pipe_cmd'

# scan stdin and send to pipe_cmd
if len(sys.argv) == 1: 
        line = ""
        while (1):
                idx = 0
	        print("Opening FIFO...")
                line = raw_input('QBO_>> ')
        	with open(FIFO_cmd, 'w') as fifo:
                	print("FIFO opened")
			print "line: ", line
			fifo.write(line)
			if (line == "exit" or line == "quit"):
				sys.exit()
			fifo.close()
sys.exit()




#====================================================================================================

