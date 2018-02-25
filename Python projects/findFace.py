#!/usr/bin/env python2.7

from multiprocessing import Process, Queue
import time
import cv2
import serial
import binascii
import QboCmd
import sys
import time
import os
import errno

## Initial Head position

Xcoor = 511
Ycoor = 600
Facedet = 0


no_face_tm = time.time()
face_det_tm = time.time()
face_not_found_idx = 0

webcam = cv2.VideoCapture(0)				# Get ready to start getting images from the webcam
webcam.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH, 320)		# I have found this to be about the highest-
webcam.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT, 240)	# resolution you'll want to attempt on the pi
#webcam.set(cv2.CV_CAP_PROP_BUFFERSIZE, 2)		# frame buffer storage

if not webcam:
	print "Error opening WebCAM"
	sys.exit(1)

frontalface = cv2.CascadeClassifier("haarcascade_frontalface_alt2.xml")		# frontal face pattern detection
profileface = cv2.CascadeClassifier("haarcascade_profileface.xml")		# side face pattern detection

face = [0,0,0,0]	# This will hold the array that OpenCV returns when it finds a face: (makes a rectangle)
Cface = [0,0]		# Center of the face: a point calculated from the above variable
lastface = 0		# int 1-3 used to speed up detection. The script is looking for a right profile face,-
			# a left profile face, or a frontal face; rather than searching for all three every time,-
			# it uses this variable to remember which is last saw: and looks for that again. If it-
			# doesn't find it, it's set back to zero and on the next loop it will search for all three.-
			# This basically tripples the detect time so long as the face hasn't moved much.

time.sleep(1)		# Wait for them to start


FIFO_findFace = '/home/pi/Documents/pipes/pipe_findFace'

try:
    os.mkfifo(FIFO_findFace)
except OSError as oe: 
    if oe.errno != errno.EEXIST:
        raise


fr_time = 0
while True:
#	print "frame time: " + str(time.time() - fr_time)
	time.sleep(0.5) # capture image every 500ms
	fr_time = time.time()

	faceFound = False       # This variable is set to true if, on THIS loop a face has already been found
                                # We search for a face three diffrent ways, and if we have found one already-
                                # there is no reason to keep looking.
        if not faceFound:
                if lastface == 0 or lastface == 1:
			t_ini = time.time()
                        while time.time()-t_ini < 0.01: # wait for present frame
				t_ini = time.time()
				aframe = webcam.read()[1]       #       there seems to be an issue in OpenCV or V4L or my webcam-
#				print "t: " + str(time.time()-t_ini)
                        fface = frontalface.detectMultiScale(aframe,1.3,4,(cv2.cv.CV_HAAR_DO_CANNY_PRUNING + cv2.cv.CV_HAAR_FIND_BIGGEST_OBJECT + cv2.cv.CV_HAAR_DO_ROUGH_SEARCH),(60,60))
                        if fface != ():                 # if we found a frontal face...
#				print "FAAACEEEE"
                                lastface = 1            # set lastface 1 (so next loop we will only look for a frontface)
                                for f in fface:         # f in fface is an array with a rectangle representing a face
                                        faceFound = True
                                        face = f

        if not faceFound:                               # if we didnt find a face yet...
                if lastface == 0 or lastface == 2:      # only attempt it if we didn't find a face last loop or if-
                        t_ini = time.time()
                        while time.time()-t_ini < 0.01: # wait for present frame
				t_ini = time.time()
				aframe = webcam.read()[1]       #       there seems to be an issue in OpenCV or V4L or my webcam-
#				print "tp: " + str(time.time()-t_ini)
                        pfacer = profileface.detectMultiScale(aframe,1.3,4,(cv2.cv.CV_HAAR_DO_CANNY_PRUNING + cv2.cv.CV_HAAR_FIND_BIGGEST_OBJECT + cv2.cv.CV_HAAR_DO_ROUGH_SEARCH),(80,80))

                        if pfacer != ():                # if we found a profile face...
#				print "PROFILE FAAACEEEE"
                                lastface = 2
                                for f in pfacer:
                                        faceFound = True
                                        face = f

        if not faceFound:               # if no face was found...-
		face_not_found_idx += 1
		if (face_not_found_idx > 3):
			face_not_found_idx = 0
	                lastface = 0            #       the next loop needs to know
	                face = [0,0,0,0]        # so that it doesn't think the face is still where it was last loop
	                if Facedet != 0:
	                        Facedet = 0
	                        no_face_tm = time.time()
#	                        print "No face.!"
	                elif(time.time() - no_face_tm > 10):
	                        Cface[0] = [0,0]
	                        no_face_tm = time.time()
        else:
                x,y,w,h = face
                Cface = [(w/2+x),(h/2+y)]       # we are given an x,y corner point and a width and height, we need the center
#		print "face ccord: " + str(Cface[0]) + "," + str(Cface[1])
		fifo = os.open(FIFO_findFace, os.O_WRONLY)
                os.write(fifo, str(Cface[0]) + "," + str(Cface[1]) + "\n")
                if Facedet == 0:
			Facedet = 1
                        face_det_tm = time.time()
                        #print "Face detected.!"
