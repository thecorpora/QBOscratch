import QboCmd

port = '/dev/serial0'  
ser = serial.Serial(port, baudrate=115200, bytesize = serial.EIGHTBITS, stopbits = serial.STOPBITS_ONE, parity = serial.PARITY_NONE, rtscts = False, dsrdtr =False, timeout = 0)

HeadServo = QboCmd.Controller(ser)

