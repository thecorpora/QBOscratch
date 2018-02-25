#!/usr/bin/env python2.7

import serial
import binascii
import time

class Command:
    def __init__(self, command, nInputs, Data):
        self.cmd = command
        self.Inputs = nInputs
        self.data_bytes = Data

class Controller:
    def __init__(self, serial):
        self.port = serial
        self.port.timeout = 1.0

    INPUT_FLAG =  0xFF
    OUTPUT_FLAG = 0xFE
    INPUT_ESCAPE = 0xFD
    GET_VERSION = 0x40
    SET_MOUTH_VALUE = 0x44
    SET_STATE = 0x45
    SET_SERVO = 0x53
    SET_SERVO_PID = 0x50
    SET_SERVO_ANGLE = 0x71
    SET_SERVO_SPEED = 0x72
    SET_SERVO_ANGLE_REL = 0x73


# Commands definions :('NAME',value, number of set params, number of get params)

    cmd_params = {
        "GET_VERSION": (0x40, 0, 1),
        "SET_MOUTH_VALUE": (0x44, 4, 0),
        "SET_STATE": (0x45, 1, 0),
        "GET_TOUCH": (0x46, 0, 1),
        "SET_TOUC_PARAMS": (0x47,3,0),
        "SET_MIC_INPUT": (0x4A, 1, 0),
        "GET_MIC_REPORT": (0x4B, 0, 6),
        "SET_SERVO_CW_LIM":	(0x4C, 3, 0),
        "SET_SERVO_CCW_LIM": (0x4D, 3, 0),
        "SET_SERVO_ID": (0x4E, 2, 0),
        "SET_SERVO_BAUD": (0x4F, 2, 0),
        "SET_SERVO_PID": (0x50, 4 , 0),
        "SET_SERVO_LED": (0x51, 2, 0),
        "RESET_SERVO": (0x52, 1, 0),
        "SET_SERVO": (0x53, 5, 0),
        "SET_SERVO_ANGLE_REL": (0x73, 3, 0),
        "SET_SERVO_ENABLE": (0x54,2, 0),
        "GET_SERVO_SPEED": (0x56, 1, 2),
        "GET_SERVO_LOAD": (0x57, 1, 2),
        "GET_SERVO_TEMP": (0x58, 1, 1),
        "GET_SERVO_VOLTAGE": (0x59, 1, 1),
        "SERVO_ISMOVING": (0x5a, 1, 1),
        "GET_HARDWARE_ERROR": (0x5b, 1, 1),
        "GET_HEAD_SERVOS": (0x5C, 0, 4),
        "GET_SERVO_POSITION": (0x5D, 1, 2),
        "GET_SERVO_CW_LIM": (0x5E, 1, 2),
        "GET_SERVO_CCW_LIM": (0x5F, 1, 2),
        "GET_SERVO_BYTE_REG": (0x6F, 2, 1),
        "GET_SERVO_WORD_REG": (0x70, 2, 2),
        "SET_SERVO_ANGLE": (0x71,3,0),
        "SET_SERVO_SPEED": (0x72,3,0),
        "SET_ADC_REF": (0x81, 1, 0),
        "SET_DAC_REF": (0x82, 1, 0),
        "SET_ADC_OFFSET": (0x83, 6, 0),
        "SET_SERVO_OVERCURRENT": (0x84, 1, 0),
        "SET_SERVO_POWER": (0x85, 2, 0),
        "SET_ENABLE_SPEAKER": (0x86, 1, 0),
        "SET_MEAN_RMS": (0x87, 6, 0), 
        "SET_USB2SERVO_FWD": (0x88, 1, 0),
        "RESET_SOUND": (0x8F, 5, 0)}

    touch_sampletime = {
        "320us": 0,
        "640us": 1 << 2,
        "1280us": 2 << 2,
        "2560us": 3 << 2}

    touch_average = {
        "SAMPLES 1": 0,
        "SAMPLES 2": 1 << 4,
        "SAMPLES 4": 2 << 4,
        "SAMPLES 8": 3 << 4,
        "SAMPLES 16": 4 << 4,
        "SAMPLES 32": 5 << 4,
        "SAMPLES 64": 6 << 4,
        "SAMPLES 128": 7 << 4}
    
    touch_cycletime = {
        "35 ms": 0,
        "70 ms": 1,
        "105 ms" : 2,
        "140 ms" : 3}
    
    touch_channels = {
        "Channel none": 0,
        "Channel 1": 1,
        "Channel 2": 1 << 2,
        "Channel 3": 1 << 3,
        "Channel all": 7}
        
    
 
    pearsondata = ([0x00, 0x77, 0xee, 0x99, 0x07, 0x70, 0xe9, 0x9e, 0x0e, 0x79, 0xe0, 0x97,
      0x09, 0x7e, 0xe7, 0x90, 0x1d, 0x6a, 0xf3, 0x84, 0x1a, 0x6d, 0xf4, 0x83,
      0x13, 0x64, 0xfd, 0x8a, 0x14, 0x63, 0xfa, 0x8d, 0x3b, 0x4c, 0xd5, 0xa2,
      0x3c, 0x4b, 0xd2, 0xa5, 0x35, 0x42, 0xdb, 0xac, 0x32, 0x45, 0xdc, 0xab,
      0x26, 0x51, 0xc8, 0xbf, 0x21, 0x56, 0xcf, 0xb8, 0x28, 0x5f, 0xc6, 0xb1,
      0x2f, 0x58, 0xc1, 0xb6, 0x76, 0x01, 0x98, 0xef, 0x71, 0x06, 0x9f, 0xe8,
      0x78, 0x0f, 0x96, 0xe1, 0x7f, 0x08, 0x91, 0xe6, 0x6b, 0x1c, 0x85, 0xf2,
      0x6c, 0x1b, 0x82, 0xf5, 0x65, 0x12, 0x8b, 0xfc, 0x62, 0x15, 0x8c, 0xfb,
      0x4d, 0x3a, 0xa3, 0xd4, 0x4a, 0x3d, 0xa4, 0xd3, 0x43, 0x34, 0xad, 0xda,
      0x44, 0x33, 0xaa, 0xdd, 0x50, 0x27, 0xbe, 0xc9, 0x57, 0x20, 0xb9, 0xce,
      0x5e, 0x29, 0xb0, 0xc7, 0x59, 0x2e, 0xb7, 0xc0, 0xed, 0x9a, 0x03, 0x74,
      0xea, 0x9d, 0x04, 0x73, 0xe3, 0x94, 0x0d, 0x7a, 0xe4, 0x93, 0x0a, 0x7d,
      0xf0, 0x87, 0x1e, 0x69, 0xf7, 0x80, 0x19, 0x6e, 0xfe, 0x89, 0x10, 0x67,
      0xf9, 0x8e, 0x17, 0x60, 0xd6, 0xa1, 0x38, 0x4f, 0xd1, 0xa6, 0x3f, 0x48,
      0xd8, 0xaf, 0x36, 0x41, 0xdf, 0xa8, 0x31, 0x46, 0xcb, 0xbc, 0x25, 0x52,
      0xcc, 0xbb, 0x22, 0x55, 0xc5, 0xb2, 0x2b, 0x5c, 0xc2, 0xb5, 0x2c, 0x5b,
      0x9b, 0xec, 0x75, 0x02, 0x9c, 0xeb, 0x72, 0x05, 0x95, 0xe2, 0x7b, 0x0c,
      0x92, 0xe5, 0x7c, 0x0b, 0x86, 0xf1, 0x68, 0x1f, 0x81, 0xf6, 0x6f, 0x18,
      0x88, 0xff, 0x66, 0x11, 0x8f, 0xf8, 0x61, 0x16, 0xa0, 0xd7, 0x4e, 0x39,
      0xa7, 0xd0, 0x49, 0x3e, 0xae, 0xd9, 0x40, 0x37, 0xa9, 0xde, 0x47, 0x30,
      0xbd, 0xca, 0x53, 0x24, 0xba, 0xcd, 0x54, 0x23, 0xb3, 0xc4, 0x5d, 0x2a,
      0xb4, 0xc3, 0x5a, 0x2d])

    # Compute pearson of data buffer starting at offset and ending at length position
    
    def pearson(self, data, length, offset):
        hash = 0
        if offset > length:
            return 0
        for i in range(offset, length):
            hash = self.pearsondata[hash ^ (data[i] & 0xff)]
        return hash

    # Check special token INPUT, OUTPUT and adds input scape secuence
    
    def InputEscape(self, char, data):
        if  char >= self.INPUT_ESCAPE:
            data.append(self.INPUT_ESCAPE)
            data.append(char - 2)
        else:
            data.append(char)
        return data
    
    def EllapsedTime(self, start):
        return time.time() - start

    # Send QBO servo command to a serial port
    
    def ReadResponse(self):
        isInputEscaped = False
        bExit = False
        wait_start = time.time()
        rx_data = []
        while True:
            rdBuffer = self.port.read()
            
            for i in rdBuffer:
                x = ord(i)
                if x == self.INPUT_FLAG:
                    rx_data = []
                    isInputEscaped = False
                elif x == self.OUTPUT_FLAG:
                    rx_data.append(x)
                    bExit = True
                    break;
                elif isInputEscaped:
                    rx_data[len(rx_data) - 1] += 2
                    isInputEscaped = False
                elif rx_data and rx_data[len(rx_data) - 1] == self.INPUT_ESCAPE:
                    isInputEscaped = True
                    continue

                rx_data.append(x)

            ellapsed_tm = self.EllapsedTime(wait_start)
            if bExit == True or ( ellapsed_tm > 0.249):
                break;
        return rx_data
    
    def ProcessRxData(self, data):
        nBytes = len(data)
        rx_param = []
        
        if nBytes < 5:  
            return 0
        elif data[0] != self.INPUT_FLAG or data[-1] != self.OUTPUT_FLAG:
            return 0
        
        check = self.pearson(data, nBytes - 2, 1)
        inCheck = data[nBytes - 2]

        if check != inCheck or data[2] != nBytes - 5 or data[2] > 50:
            return 0

        for x in range(3, nBytes - 2):
            rx_param.append(data[x])

        return rx_param

    def GetHeadCmd(self, cmd, cmd_buffer):
        if self.cmd_params.has_key(cmd) == False:
            return 0
        nParams = self.cmd_params[cmd][1]
        tx_buffer = []
        
        if nParams > 1:
            tx_buffer.extend(cmd_buffer)
        elif nParams == 1:
            tx_buffer.append(cmd_buffer)
        for x in range(3):
            self.port.reset_input_buffer()    
            self.SendCmdQBO(Command(self.cmd_params[cmd][0], nParams, tx_buffer))
            rxBuffer = self.ReadResponse()
            if len(rxBuffer) >= 5:
                break
            else:
                print "NACK ",rxBuffer
        if len(rxBuffer) < 5:
            print" Error reading response.!"
        rdParam = self.ProcessRxData(rxBuffer)
        return rdParam
        
    def SendCmdQBO(self, cmd):
        tmp_buffer = [self.INPUT_FLAG, cmd.cmd, cmd.Inputs]
        tx_buffer = [self.INPUT_FLAG, cmd.cmd, cmd.Inputs]
        
        if cmd.Inputs > 1:
            nParam= len(cmd.data_bytes)
        elif cmd.Inputs == 1:
            nParam = 1
        else :
            nParam = 0
            
        for i in range(nParam):
            if nParam== 1:
                data = cmd.data_bytes #[0]
            else:
                data = cmd.data_bytes[i]

            if data > 255:
                tmp_buffer.append(data & 0xff)
                tmp_buffer.append((data >> 8) & 0xff)
            else :
                tmp_buffer.append(data)

        tmp_buffer.append(self.pearson(tmp_buffer, cmd.Inputs + 3, 1))
        tmp_buffer.append(self.OUTPUT_FLAG)

        for i in range(3, len(tmp_buffer) - 1):
       
            if  tmp_buffer[i] >= self.INPUT_ESCAPE:
                tx_buffer.append(self.INPUT_ESCAPE)
                tx_buffer.append(tmp_buffer[i] - 2)
            else:
                tx_buffer.append(tmp_buffer[i])
        tx_buffer.append(self.OUTPUT_FLAG)
        databuffer = bytearray(tx_buffer)
        self.port.write(databuffer)
#        self.port.flush()
        
        return databuffer

    # Mounts protocol data to set servo position&speed  and call sends
    
    def SetServo(self, Axis, Angle, Speed):
        cmd_buffer = ([ Axis, Angle & 0xff ,  (Angle >>8) & 0xff, Speed & 0xff, (Speed >>8) & 0xff])
        return self.SendCmdQBO(Command(self.SET_SERVO, len(cmd_buffer), cmd_buffer))

    # Mounts protocol data to set QBO position angle

    def SetAngle(self, Axis, Angle):
        cmd_buffer = ([ Axis, Angle & 0xff ,  (Angle >>8) & 0xff])
        return self.SendCmdQBO(Command(self.SET_SERVO_ANGLE, len(cmd_buffer), cmd_buffer))

    # Mounts protocol data to set QBO position relative angle

    def SetAngleRelative(self, Axis, Angle):
        cmd_buffer = ([ Axis, Angle & 0xff ,  (Angle >>8) & 0xff])
	print "SetAnlgeRelative: (" + str(Axis) + "," + str(Angle) + ")"
        return self.SendCmdQBO(Command(self.SET_SERVO_ANGLE_REL, len(cmd_buffer), cmd_buffer))

    # Mounts protocol data to set QBO nose state color

    def SetNoseColor(self, color):
        return self.SendCmdQBO(Command(self.SET_STATE, 1, color & 0xff))

    # Mounts protocol data to set QBO mouth 
        
    def SetMouth(self, matrix):
        cmd_buffer = ([(matrix>>24) & 0xff, (matrix>>16) & 0xff, (matrix>>8) & 0xff, matrix & 0xff])
        return self.SendCmdQBO(Command(self.SET_MOUTH_VALUE, 4, cmd_buffer)) 
    
    def SetPid(self, Axis, pid_p, pid_i, pid_d):
        cmd_buffer = ([Axis, pid_p, pid_i, pid_d])
	print "SetPid " + str(cmd_buffer)
        return self.SendCmdQBO(Command(self.SET_SERVO_PID, len(cmd_buffer), cmd_buffer)) 
    

        
        

