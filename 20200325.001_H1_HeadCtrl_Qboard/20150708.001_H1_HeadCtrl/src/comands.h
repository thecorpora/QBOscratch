// Command format
// INPUT_FLAG + COMMAND + Number of Input Bytes + Input Byte[0] ... Input Byte[n] + PEARSON CHECKSUM + OUTPUT_FLAG

#define GET_VERSION 0x40
//#define ALL_SRF_UPDATE 0x41       //A
//#define GET_BRUJULA 0x42          //B  //Ya no está
//#define RESET_ODOMETRY 0x42
//#define CHANGE_I2C_DIR 0x43       //C
#define SET_MOUTH_VALUE 0x44      //D  //Para la PR2
#define SET_STATE 0x45
#define GET_TOUCH 0x46
#define SET_TOUCH_PARAMS 0x47
#define SET_MIC_INPUT 0x4A        //J  //Para la PR2
#define GET_MIC_REPORT 0x4B       //K  //Para la PR2
#define SET_SERVO_CW_LIM	0x4C
#define SET_SERVO_CCW_LIM	0x4D
#define SET_SERVO_ID 0x4E         //O
#define SET_SERVO_BAUD 0x4F
#define SET_SERVO_PID 0x50              //PID
#define SET_SERVO_LED 0x51
#define RESET_SERVO 0x52
#define SET_SERVO 0x53        
#define SET_SERVO_ENABLE 0x54
#define GET_SERVO_SPEED 0x56            //V
#define GET_SERVO_LOAD	0x57
#define GET_SERVO_TEMP	0x58
#define GET_SERVO_VOLTAGE 0x59
#define SERVO_ISMOVING	0x5a
#define GET_HARDWARE_ERROR 0x5b
#define GET_HEAD_SERVOS 0x5C
#define GET_SERVO_POSITION 0x5D
#define GET_SERVO_CW_LIM	0x5E
#define GET_SERVO_CCW_LIM	0x5F
#define GET_SERVO_BYTE_REG	0x6F
#define GET_SERVO_WORD_REG	0x70
#define SET_SERVO_ANGLE	0x71
#define SET_SERVO_SPEED	0x72
#define SET_SERVO_ANGLE_REL	0x73  // relative angle movement
#define SET_SAMPLE_RATE 0x80
#define SET_ADC_REF 0x81
#define SET_DAC_REF 0x82
#define SET_ADC_OFFSET 0x83
#define SET_SERVO_OVERCURRENT 0x84
#define SET_SERVO_SWITCH	0x85
#define SET_ENABLE_SPEAKER	0x86
#define SET_MEAN_RMS 0x87
#define SET_USB2SERVO_FWD	0x88	// USB to serial servo port fordwarding 
#define SAVE_CFG_NVRAM	0x8E		
#define RESET_SOUND 0x8F
#define SET_TOUCH_AUTO_OFF 0x90

