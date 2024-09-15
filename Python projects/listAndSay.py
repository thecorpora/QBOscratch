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
import QBOtalk

Qbo = QBOtalk.QBOtalk()

Listenig = True
listen_thd = 0


def SayFromFifo():
        print("Opening FIFO...")
        fifo = os.open(FIFO_say, os.O_RDONLY | os.O_NONBLOCK)
	try:
       	        data = os.read(fifo, 100)
	except OSError as oe: 
		if oe.errno != 11: #errno.EEXIST:
       			raise

       	os.close(fifo)

        if data:
	        config = yaml.safe_load(open("/home/pi/Documents/config.yml"))

                print('Read: "{0}"'.format(data))
                if config["languaje"] == "english":
                        speak = "espeak -ven+f3 \"" + data + "\" --stdout  | aplay -D convertQBO"
                elif config["languaje"] == "spanish":
                        speak = "espeak -v mb-es2 -s 120 \"" + data + "\" --stdout  | aplay -D convertQBO"
                print "Talk: " + speak
                result = subprocess.call(speak, shell = True)



def WaitForSpeech():
        global Listenig, listen_thd, FIFO_listen, FIFO_cmd
        if Listenig == False:
                return
        elif Qbo.GetAudio == True:
#                HeadServo.SetNoseColor(0)       #Off QBO nose brigth
		fifo = os.open(FIFO_cmd, os.O_WRONLY)
                os.write(fifo, "-c nose -co red")
		os.close(fifo)
                listen_thd(wait_for_stop = True)
                print("Ha llegado algo al WaitForSpeech: " + Qbo.strAudio)
		fifo = os.open(FIFO_listen, os.O_WRONLY)
		os.write(fifo, Qbo.strAudio)
		os.close(fifo)
        return


#============================================================================================================

#Qbo.SpeechText("I am ready.")
# FIFO init.
FIFO_listen = '/home/pi/Documents/pipes/pipe_listen'
FIFO_say = '/home/pi/Documents/pipes/pipe_say'
FIFO_cmd = '/home/pi/Documents/pipes/pipe_cmd'

try:
    os.mkfifo(FIFO_listen)
except OSError as oe: 
    if oe.errno != errno.EEXIST:
        raise
try:
    os.mkfifo(FIFO_cmd)
except OSError as oe: 
    if oe.errno != errno.EEXIST:
        raise

listen_thd = Qbo.StartBackListen()
#HeadServo.SetNoseColor(1)       # Set QBO nose green
fifo = os.open(FIFO_cmd, os.O_WRONLY)
os.write(fifo, "-c nose -co green")
os.close(fifo)


while True:
	SayFromFifo()
        WaitForSpeech()
        if Qbo.GetAudio == True:
	    fifo = os.open(FIFO_cmd, os.O_WRONLY)
            os.write(fifo, "-c nose -co red")
	    os.close(fifo)
            # HeadServo.SetNoseColor(0)       #Off QBO nose brigth
            time.sleep(1)
            print("StartBackListen")
            try:
                listen_thd = Qbo.StartBackListen()
#                HeadServo.SetNoseColor(1)       # Set QBO nose green
		fifo = os.open(FIFO_cmd, os.O_WRONLY)
                os.write(fifo, "-c nose -co green")
	    	os.close(fifo)
                Qbo.GetAudio = False
            except:
                print("StartBackListe EXCEPTION")
