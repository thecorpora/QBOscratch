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

import QBOtalk


cmd = ""
angle = 0
axis = 0
speed = 0
color = ""
text = ""
idx = 0
line = ""
lang = ""
ser=""
matrix = 0
expression = ""
pid = [0, 0, 0]

# # thread function (NOT USED)
# def dispach_UI():
         
# retorna el argumento en la posicion 'idx' 
def scan_argument(index):
        if len(sys.argv) > 1:
                return sys.argv[index]
        else:
                return line.split()[index]

# retorna el numero de argumentos del ejecutable o de 'line'
def n_args(line):
        if len(sys.argv) > 1:
                return len(sys.argv)
        else:
                return len(line.split())
        

def get_command():
        global cmd
        try:
                arg = scan_argument(idx)
                #if len(sys.argv) > 1:
                #        arg = sys.argv[i+1]
                #else:
                #        arg = line.split()[i+1]

                if arg in commands:
                        cmd = str(arg)
                else:
                        print "wrong command"
        except:
                print "command param error"


def get_angle():
        global angle
        try:
                arg = scan_argument(idx)

                if int(arg) <= 800 and int(arg) >= -800:
                        angle = int(arg)
                else:
                        print "wrong angle value"
        except:
                print "wrong angle value"


def get_text():
        global text
        global idx

        text = ""
        try:
                while idx < n_args(line):
                        arg = scan_argument(idx)

                        # next command is finded
                        if arg[0] == '-':
                                break
                        idx = idx + 1
                        text += " " + arg
        except:
                print "wrong text value"

def get_pid():
        global pid
        global idx

        pid = [0, 0, 0]
	i = 0
        try:
                while idx < n_args(line):
                        arg = scan_argument(idx)

                        # next command is finded
                        if arg[0] == '-':
                                break
                        idx = idx + 1
                        pid[i] = int(arg)
			i += 1
        except:
                print "wrong pid value"
	print "PID: " + str(pid)

def get_axis():
        global axis

        arg = scan_argument(idx)

        if arg in axises:
                axis = int(arg)
        else:
                print "wrong axis value"

def get_speed():
        global speed

        try:
                arg = scan_argument(idx)

                if int(arg) >= 0 and int(arg) < 2000:
                        speed = int(arg)
                else:
                        print "wrong speed value"
        except:
                print "wrong speed value"

def get_color():
        global color

        try:
                arg = scan_argument(idx)

                if arg in colors:
                        color = arg
                else:
                        print "wrong color value"
        except:
                print "color param error"


def get_language():
        global lang

        try:
                arg = scan_argument(idx)

                if arg in languages:
                        lang = arg
                else:
                        print "wrong language value"
        except:
                print "laguage param error"

def get_mouth_expression():
        global expression

        try:
                arg = scan_argument(idx)

                if arg in expressions:
                        expression = arg
                else:
                        print "wrong expression value"
        except:
                print "expression param error"


def get_mouth_matrix():
	global matrix
	global idx

	print "GET_MOUTH_MATRIX"
	print "idx: " + str(idx)
	try:
		i = 0
		while i<4:
			arg = scan_argument(idx)
			print "arg: " + arg
			matrix |= int(arg) << (8*(3-i))
			print "arg matrix: " + str(matrix)
			i = i + 1
			idx = idx + 1
	except:
		print "mouth matrix param error"


def say():
        print ""


def help():
        global cmd

        cmd = "help"
        print " "
        print "Options:"
        print "-c [command] servo, nose, say, mouth, listen or voice"
        print "-a [angle] from -180 to 180"
        print "-x [axis] 1 or 2"
        print "-s [speed] from 1 to 2000"
        print "-co [color] none, red, green or blue"
	print "-l [language] english or spanish"
	print "-m [matrix] mouth leds matrix"
	print "-e [expression] smile, sad or serious"
        print " "
        print "EXAMPLES: "
        print "-c servo -a 30 -x 1 -s 200" 
        print "-c nose -co red" 
        print " "
 

def CommandOK_Action():
        global config
        global ser
        global HeadServo
	global color

	FIFO_say = '/home/pi/Documents/pipes/pipe_say';

#    /* create the FIFO (named pipe) */
#    mkfifo(myfifo, 0666);


        if cmd == "servo" and angle != 0 and axis != 0 and speed >= 0:
                print "Sending: " + cmd + "(" + str(axis) + "," + str(angle) + "," + str(speed) + ")"
		HeadServo.SetServo(axis, angle, speed)
        elif cmd == "move" and angle != 0 and axis != 0:
		HeadServo.SetAngle(axis, angle)
        elif cmd == "move_rel" and angle != 0 and axis != 0:
		HeadServo.SetAngleRelative(axis, angle)
	elif cmd == "nose" and color != "":
                print "Sending: " + cmd + "(" + str(color) + ")"
                # se apaga el led de la nariz
                # llamada al comando "nose"
                if (color == "none"):
			HeadServo.SetNoseColor(0)
                if (color == "red"):
			HeadServo.SetNoseColor(2)
                if (color == "blue"):
			HeadServo.SetNoseColor(1)
                if (color == "green"):
			HeadServo.SetNoseColor(4)
        elif cmd == "say" and text != "":
#		/* write "Hi" to the FIFO */
	 	print("Opening FIFO..." + FIFO_say)
		fifo_say = os.open(FIFO_say, os.O_WRONLY)
		os.write(fifo_say, text)
		os.close(fifo_say)
#     		with open(fifo_say, 'w') as fifo:
#                	print("FIFO opened")
#			fifo.write(text)		
#			fifo.close()
                print "Saying: " + text
        elif cmd == "voice":
		config = yaml.safe_load(open("/home/pi/Documents/config.yml"))
		print "CONFIG " + str(config)

		# actualizacion del fichero config
                config["language"] = lang
                with open('/home/pi/Documents/config.yml', 'w') as f:
                        yaml.dump(config, f)
                print "Setting: " + cmd + " = " + str(lang)
                f.close()
        elif cmd == "mouth":
		if (matrix != 0):
			print "Sending " + cmd + "(" + str(matrix) + ")"
			HeadServo.SetMouth(matrix)
		elif (expression != ""):
			print "Sending " + cmd + "(" + expression + ")"
			if expression == "smile":
				HeadServo.SetMouth(0x110E00)
			elif expression == "sad":
				HeadServo.SetMouth(0x0E1100)
			elif expression == "serious":
				HeadServo.SetMouth(0x1F1F00)
			elif expression == "love":
				HeadServo.SetMouth(0x1B1F0E04)
	elif cmd == "pid":
		if (axis != 0):
			HeadServo.SetPid(axis, pid[0], pid[1], pid[2])
	elif cmd == "listen":
		listen_thd = Qbo.StartBack()
                

	else:
                print "Command error. Type ? to help"


options = {"-c"  : get_command,
           "-a"  : get_angle,
           "-x"  : get_axis,
           "-s"  : get_speed,
           "-t"  : get_text,
           "-co" : get_color,
           "-l"  : get_language,
	   "-m"  : get_mouth_matrix,
	   "-e"  : get_mouth_expression,
	   "-p"  : get_pid,
           "?" : help,
           "help" : help,
           "-h" : help,
}

commands = {"servo",
            "nose",
            "say",
            "voice",
	    "mouth",
	    "listen",
	    "pid",
	    "move",
	    "move_rel",
}


axises = {"1", "2"}

colors = {"none", "red", "green", "blue"}

languages = {"english", "spanish"}

expressions = {"smile", "sad", "serious", "love"}

# start read key thread
#thread.start_new_thread( dispach_UI,() )

#take stdin arguments
#print "numero de argumentos:" + str(len(sys.argv))
# scan arguments 
cmd = ""
angle = 0
axis = 0
speed = 0
color = ""

# fichero config, contiene language
config = yaml.safe_load(open("/home/pi/Documents/config.yml"))
print "CONFIG " + str(config)

# escanea los argumentos del ejecutable
for arg in sys.argv:
        idx = idx + 1
        if (arg) in options:
                options[arg]()

# se abre el puerto serie
port = '/dev/serial0'

try:
        # Open serial port
        ser = serial.Serial(port, baudrate=115200, bytesize = serial.EIGHTBITS, stopbits = serial.STOPBITS_ONE, parity = serial.PARITY_NONE, rtscts = False, dsrdtr =False, timeout = 0)
        print "Open serial port sucessfully."
        print(ser.name)
        HeadServo = QboCmd.Controller(ser)
except:
        print "Error opening serial port."
        sys.exit()

# Qbo speak and voice recognition init.
# Qbo = QBOtalk.QBOtalk()

# if parameters in command then execute action
if len(sys.argv) > 1: 
        CommandOK_Action()


# FIFO init.
FIFO_cmd = '/home/pi/Documents/pipes/pipe_cmd'

try:
    os.mkfifo(FIFO_cmd)
except OSError as oe:
    if oe.errno != errno.EEXIST:
        raise

# scan stdin
if len(sys.argv) == 1: 
        line = ""
        while (1):
                idx = 0
	        print("Opening FIFO..." + FIFO_cmd)
        	with open(FIFO_cmd) as fifo_cmd:
                	print("FIFO opened" + FIFO_cmd)
       	                line = fifo_cmd.read()
			fifo_cmd.close()
               	        if len(line) == 0:
                       	        print("Writer closed")
                               	continue
                       	print('line_cmd: ' + line)
#                line = raw_input('QBO_>> ')

			if (line == "exit" or line == "quit"):
				sys.exit()
	                cmd = ""
	                angle = 0
	                axis = 0
	                speed = 0
	                color = ""
			matrix = 0
			expression = ""

	                # escanea los argumentos de la linea de comandos
	                for word in line.split():
	                        idx = idx + 1
	                        if word in options:
	                                options[word]()
                     
	                # Gestiona los comandos correctos
	                # Se realizan diferentes operaciones:
	                #    - Envios a la placa por puerto serie.
	                #    - LLamadas al sistema de la raspi para que ejecute el programa 'espeak'.
	                CommandOK_Action()

sys.exit()




#====================================================================================================

