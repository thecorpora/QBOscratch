#coding: latin-1

import cv2
import time,datetime

capl = cv2.VideoCapture(1)
capl.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH, 320)		# I have found this to be about the highest-
capl.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT, 240)	# resolution you'll want to attempt on the pi

capr = cv2.VideoCapture(0)
capr.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH, 320)		# I have found this to be about the highest-
capr.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT, 240)	# resolution you'll want to attempt on the pi


while (True):
    ret, left = capl.read()
    ret, right = capr.read()
    
    cv2.imshow('Left', left)
    cv2.imshow('Right', right)
    
    key = cv2.waitKey(1) & 0xFF
    
    if (key == ord('q')):
        break
        
    
capl.release()
capr.release()
        
cv2.destroyAllWindows()