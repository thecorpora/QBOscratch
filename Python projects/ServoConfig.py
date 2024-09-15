#!/usr/bin/env python2.7

import argparse
import serial
import time
import QboCmd

parser = argparse.ArgumentParser()
parser.add_argument("-d","--device", help = "Servo number ID = 1:Left-Right servo 2:Up-Down Servo", type = int)
parser.add_argument("-c","--command", help = "Command: SET_SERVO_ID, SET_SERVO_CW_LIM, SET_SERVO_CCW_LIM, SET_USB2SERVO_FWD, SET_SERVO_ENABLE")
parser.add_argument("param", help = "Command parameter", type = int)
args = parser.parse_args()

comPort = serial.Serial('/dev/serial0', 115200, timeout = 0)
HeadCtrl = QboCmd.Controller(comPort)

def ChangeDeviceID(device, cmd, value):
    HeadCtrl.GetHeadCmd("SET_SERVO_LED", [device,1])
    time.sleep(0.1)
    Id = HeadCtrl.GetHeadCmd("GET_SERVO_BYTE_REG", [device, 3])
    print "Present ID", Id
    time.sleep(0.1)
    HeadCtrl.GetHeadCmd(cmd, [device,value])
    print cmd, [device,value]
    time.sleep(0.1)
    newId = HeadCtrl.GetHeadCmd("GET_SERVO_BYTE_REG", [value, 3])
    print "New ID", newId
    time.sleep(.5)
    HeadCtrl.GetHeadCmd("SET_SERVO_LED", [value,0])
    time.sleep(.1)
    return

def ChangePortFwd(cmd, value):
    fwd_en = value & 1
    if fwd_en != 0:
        print "USB to Servo port forwarding enabled", fwd_en
    else:
        print "USB to Servo port forwarding disabled", fwd_en
        
    HeadCtrl.GetHeadCmd(cmd, [device,fwd_en])
    time.sleep(.1)
    return

def ChangeServoEnable(device, cmd, value):
    servo_en = value & 1
    if servo_en != 0:
        print "Servo", device, "enabled"
        
    else:
        print "Servo", device, "disabled"
    HeadCtrl.GetHeadCmd(cmd, [device,servo_en])
    time.sleep(.1)
    return

def GetServoLimits(cmd, device):
    if cmd == "SET_SERVO_CW_LIM":
        cw_limits = HeadCtrl.GetHeadCmd("GET_SERVO_CW_LIM", device)
        if cw_limits:
            result = (cw_limits[1] << 8 | cw_limits[0])
        else :
            result = 0
    elif cmd == "SET_SERVO_CCW_LIM":
        ccw_limits = HeadCtrl.GetHeadCmd("GET_SERVO_CCW_LIM", device)
        if ccw_limits:
            result = (ccw_limits[1] << 8 | ccw_limits[0])
        else :
            result = 0
    return result

def ChangeLimit(device, cmd, value):
    limit = GetServoLimits(cmd, device)
    print "Present Limit " , limit
    time.sleep(0.1)
    HeadCtrl.GetHeadCmd("SET_SERVO_LED", [device,1])
    time.sleep(0.1)
    HeadCtrl.GetHeadCmd(cmd, [device,value])
    time.sleep(0.1)
    newLimit = GetServoLimits(cmd, device)
    if newLimit == value:
        print"Limit changed!"
    print "New Limit", newLimit
    time.sleep(.5)
    HeadCtrl.GetHeadCmd("SET_SERVO_LED", [device,0])
    return

if args.command == "SET_USB2SERVO_FWD":
    ChangePortFwd(args.command, args.param)
elif args.device == 1 or args.device == 2:
    if args.command == "SET_SERVO_ID":
        if args.param == 1 or args.param == 2:
            ChangeDeviceID(args.device, args.command, args.param)
        else:
            print "Bad setting new device number", args.param
    elif args.command == "SET_SERVO_CW_LIM" or args.command == "SET_SERVO_CCW_LIM":
        if args.param > 1023:
            print "Bad setting angle limit"
        else:
            ChangeLimit(args.device, args.command, args.param)
    elif args.command == "SET_SERVO_ENABLE":
        ChangeServoEnable(args.device, args.command, args.param)
else:
    print "Bad device number", args.device




#print HeadCtrl.GetHeadCmd("SET_USB2SERVO_FWD", 1)
##time.sleep(0.1)
##cw_limits = HeadCtrl.GetHeadCmd("GET_SERVO_CW_LIM", 2)
##print cw_limits
##time.sleep(0.025)
##
##ccw_limits = HeadCtrl.GetHeadCmd("GET_SERVO_CCW_LIM", 2)
##print ccw_limits
##time.sleep(0.025)
##
##reg_byte = HeadCtrl.GetHeadCmd("GET_SERVO_BYTE_REG", [2, 8])
##print reg_byte
##time.sleep(0.025)
##
##reg_word = HeadCtrl.GetHeadCmd("GET_SERVO_WORD_REG", [2, 8])
##print reg_word
##time.sleep(0.025)


## USB to servo port fordwarding enable

## HeadCtrl.GetHeadCmd("SET_USB2SERVO_FWD", 1)

## Set servo ID and angle limits

##print HeadCtrl.GetHeadCmd("SET_SERVO_ENABLE", [2,0])
##print HeadCtrl.GetHeadCmd("SET_SERVO_CW_LIM", [2,350])
##print HeadCtrl.GetHeadCmd("SET_SERVO_CCW_LIM", [2,600])
##print HeadCtrl.GetHeadCmd("SET_SERVO_ID", [2,3])
##print HeadCtrl.GetHeadCmd("SET_SERVO_LED", [3,4])
##time.sleep(0.5)
##print HeadCtrl.GetHeadCmd("SET_SERVO_LED", [3,0])



## Changes Servo ID number 1 to 2

# print HeadCtrl.GetHeadCmd("SET_SERVO_ID", [1, 2])

## Touch configuration : channels enabled, sensitivity, average and sample time 

##t_params = [
##    HeadCtrl.touch_channels["Channel all"],
##    85,
##    HeadCtrl.touch_average["SAMPLES 32"] | HeadCtrl.touch_sampletime["2560us"] | HeadCtrl.touch_cycletime["140 ms"]]
##
##print HeadCtrl.GetHeadCmd("SET_TOUC_PARAMS", t_params)

## USB to servo port fordwarding enable

# print HeadCtrl.GetHeadCmd("SET_USB2SERVO_FWD", 1)

## Read touch channels status
## returns 1,2,3

##while True:
##    touch = HeadCtrl.GetHeadCmd("GET_TOUCH", 0)
##    if touch:
##        if touch == [1]:
##            print ("Right ear")
##        if touch == [2]:
##            print ("Back")
##        if touch == [3]:
##            print ("Left ear")
##
##    time.sleep(.250)
    

        
        

