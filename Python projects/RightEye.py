#coding: latin-1

import cv2
import time,datetime

cap = cv2.VideoCapture(1)
cap.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH, 320)		# I have found this to be about the highest-
cap.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT, 240)	# resolution you'll want to attempt on the pi



while (True):
    ret, frame = cap.read()
    
    cv2.imshow('Video Stream', frame)
    
    key = cv2.waitKey(1) & 0xFF
    
    if (key == ord('q')):
        break
        
    
cap.release()
        
cv2.destroyAllWindows()