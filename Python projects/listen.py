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

Listening = True
listen_thd = 0


def WaitForSpeech():
        global Listening, listen_thd, FIFO_listen, FIFO_cmd

	print "WaitForSpeech: Listening=" + str(Listening) + "getAudio=" + str(Qbo.GetAudio)
        if Listening == False:
                print "Listening = False"
		return
        elif Qbo.GetAudio == True:
#                HeadServo.SetNoseColor(0)       #Off QBO nose brigth
#		fifo = os.open(FIFO_cmd, os.O_WRONLY)
#                os.write(fifo, "-c nose -co red")
#		os.close(fifo)
                listen_thd(wait_for_stop = True)
                print("Ha llegado algo al WaitForSpeech: " + Qbo.strAudio)
		fifo = os.open(FIFO_listen, os.O_WRONLY)
		os.write(fifo, Qbo.strAudio)
		os.close(fifo)
		Qbo.GetAudio = False
		Listening = False
        return


#============================================================================================================

#Qbo.SpeechText("I am ready.")
# FIFO init.
FIFO_listen = '/home/pi/Documents/pipes/pipe_listen'
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
#fifo = os.open(FIFO_cmd, os.O_WRONLY)
#os.write(fifo, "-c nose -co green")
#os.close(fifo)


while True:
        time.sleep(1)
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
#		fifo = os.open(FIFO_cmd, os.O_WRONLY)
#                os.write(fifo, "-c nose -co green")
#	    	os.close(fifo)
                Qbo.GetAudio = False
            except:
                print("StartBackListe EXCEPTION")

	if Listening == False:
		listen_thd = Qbo.StartBackListen()
		Listening = True
