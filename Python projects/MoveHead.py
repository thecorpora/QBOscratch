import time
import serial #handles the serial ports
import QboCmd #holds some commands we can use for Qbo

#set up ports for communicating with servos
port = '/dev/serial0'
ser = serial.Serial(port, baudrate=115200, bytesize = serial.EIGHTBITS, stopbits = serial.STOPBITS_ONE, parity = serial.PARITY_NONE, rtscts = False, dsrdtr =False, timeout = 0)
QBO = QboCmd.Controller(ser)

print("Start Positon")
#Set a start position 
QBO.SetServo(1,511, 100)#Axis,Angle,Speed
QBO.SetServo(2,450,100)#Axis,Angle,Speed
time.sleep(1)

print("Left Positon")
#Move the head to the left
QBO.SetServo(1,725, 100)#Axis,Angle,Speed
#Pause
time.sleep(1)

print("Start Positon")
#move it back to starting point
QBO.SetServo(1,511, 100)#Axis,Angle,Speed
#Pause
time.sleep(1)

print("Right Positon")
#Move the head to the right
QBO.SetServo(1,290, 100)#Axis,Angle,Speed
#Pause
time.sleep(1)

print("Start Positon")
#move it back to starting point
QBO.SetServo(1,511, 100)#Axis,Angle,Speed
#Pause
time.sleep(1)

#moving the head up and down along Y axis

print("Up Positon")
#Move the head to the left
QBO.SetServo(2,530, 100)#Axis,Angle,Speed
#Pause
time.sleep(1)

print("Start Positon")
#move it back to starting point
QBO.SetServo(2,450,100)#Axis,Angle,Speed
#Pause
time.sleep(1)

print("Down Positon")
#Move the head to the right
QBO.SetServo(2,400, 100)#Axis,Angle,Speed
#Pause
time.sleep(1)

print("Start Positon")
#move it back to starting point
QBO.SetServo(2,450,100)#Axis,Angle,Speed
#Pause
time.sleep(1)