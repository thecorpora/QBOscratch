/*
 Code based on:
 Dynamixel.cpp - Ax-12+ Half Duplex USART Comunication
 Copyright (c) 2011 Savage Electronics.
 And Dynamixel Pro library for OpenCM-9.04 made by ROBOTIS, LTD.

 Modified to work only with Dynamixel XL-320 actuator.

 Modifications made by Luis G III for HelloSpoon robot.
 Webpage: http://hellospoonrobot.com
 Twitter: @HelloSpoon
 Youtube: http://youtube.com/user/hellospoonrobot

 This file can be used and be modified by anyone, 
 don't forget to say thank you to OP!
 
 */

#ifndef XL320_H_
#define XL320_H_

#include "asf.h"
#include <stdint.h>

#define SERVO_BUFFER_LENGTH	64

/*EEPROM Area*/
#define XL_MODEL_NUMBER_L           0
#define XL_MODEL_NUMBER_H           1
#define XL_VERSION                  2
#define XL_ID                       3
#define XL_BAUD_RATE                4
#define XL_RETURN_DELAY_TIME        5
#define XL_CW_ANGLE_LIMIT_L         6
#define XL_CW_ANGLE_LIMIT_H         7
#define XL_CCW_ANGLE_LIMIT_L        8
#define XL_CCW_ANGLE_LIMIT_H        9
#define XL_CONTROL_MODE             11
#define XL_LIMIT_TEMPERATURE        12
#define XL_DOWN_LIMIT_VOLTAGE       13
#define XL_UP_LIMIT_VOLTAGE         14
#define XL_MAX_TORQUE_L             15
#define XL_MAX_TORQUE_H             16
#define XL_RETURN_LEVEL             17
#define XL_ALARM_SHUTDOWN           18

/*RAM Area*/
#define XL_TORQUE_ENABLE            24
#define XL_LED                      25
#define XL_D_GAIN    				27
#define XL_I_GAIN      				28
#define XL_P_GAIN    				29
#define XL_GOAL_POSITION			30
#define XL_GOAL_SPEED	            32
#define XL_GOAL_TORQUE 		        35
#define XL_PRESENT_POSITION         37
#define XL_PRESENT_SPEED            39
#define XL_PRESENT_LOAD             41
#define XL_PRESENT_VOLTAGE          45
#define XL_PRESENT_TEMPERATURE      46
#define XL_REGISTERED_INSTRUCTION   47
#define XL_MOVING                   49
#define XL_HARDWARE_ERROR           50
#define XL_PUNCH                    51

#define Tx_MODE                     1
#define Rx_MODE                     0

typedef enum tag_ledColor
{
	eLED_OFF = 0,
	eLED_RED,
	eLED_GREEN,
	eLED_YELLOW,
	eLED_BLUE,
	eLED_PINK,
	eLED_CIAN,
	eLED_WHITE,
}eLed_Color;

typedef enum tag_eBAUDRATE
{
	BAUD_9600 = 0,
	BAUD_57600,
	BAUD_115200,
	BAUD_1M,
	MAX_BAUD = 0xff
}eBAUDRATE;

typedef enum tag_XLERROR
{
	eXLERROR_FAIL = 1,		//  failed to process the sent Instruction Packet
	eXLERROR_CMD,			//  undefined Instruction has been used  or When Action has been used without Reg Write
	eXLERROR_CRC,			//  CRC of the sent Packet does not match
	eXLERROR_DATARANGE,		//  Data to be written in the corresponding Address is outside the range of the minimum/maximum value
	eXLERROR_DATALENGTH,	//  attempt to write Data that is shorter than the data length of the corresponding Address
	eXLERROR_DATALIMIT,		//  Data to be written in the corresponding Address is outside of the Limit value
	ACCESSERROR				// attempt to write a value in an Address that is Read Only or has not been defined
							// read a value in an Address that is Write Only or has not been defined
							//  write a value in the ROM domain while in a state of Torque Enable(ROM Lock)
}eXLERROR;

typedef enum tag_XLCMD
{
	eXLCMD_PING = 1,
	eXLCMD_READ,
	eXLCMD_WRITE,
	eXLCMD_REGWRITE,
	eXLCMD_ACTION,
	eXLCMD_FACTORYRESET,
	eXLCMD_REBOOT = 8,
	eXLCMD_SYNCREAD = 0x82,
	eXLCMD_SYNCWRITE,
	eXLCMD_BULKREAD = 0x92,
	eXLCMD_BULKWRITE,
	eXLCMD_MAX = INT8_MAX
}eXLCMD;

typedef enum tag_eXL320_ERROR
{
	eNOERROR = 0,
	eRESULT,			//  Result Fail, failed to process the sent Instruction Packet
	eINSTRUCTION,		// Instruction Error, undefined Instruction has been used, action has been used without Reg Write
	eCRC,				// CRC Error, CRC of the sent Packet does not match
	eDATARANGE,			// Data Range Error, Data to be written in the corresponding Address is outside the range of the minimum/maximum value
	eDATALENGTH,		// Data Length Error,  attempt to write Data that is shorter than the data length of the corresponding Address
	eDATALIMIT,			// Data Limit Error, Data to be written in the corresponding Address is outside of the Limit value
	eACESS,				// Access Error: 	Attempt to read / write a value in an Address that is Read / Write Only or has not been defined
						// Attempt to write a value in the ROM domain while in a state of Torque Enable(ROM Lock)
	eMAXVALUE = INT8_MAX
}eXL320_ERROR;

typedef	struct tag_ServoEEPROM
{
	uint8_t Model[2];
	uint8_t FirmVersion;
	uint8_t ID;
	uint8_t BaudRate;
	uint8_t ReturnDelay;
	uint8_t CW_Limit[2];
	uint8_t CCW_Limit[2];
	uint8_t GhostByte;
	uint8_t CtrlMode;
	uint8_t	Temp_Limit;
	uint8_t LowVolt_Limit;
	uint8_t TopVolt_Limit;
	uint8_t Torque_Max[2];
	uint8_t Return_Level;
	uint8_t Alarm_ShutDown;
	uint8_t Dummy;
}ServoEEPROM;

typedef struct tag_ServoCfg
{
	ServoEEPROM EEPROM;
	uint8_t LED;
	uint8_t D_Gain;

	uint8_t I_Gain;
	uint8_t P_Gain;
	uint16_t Home_Position;

	uint16_t Moving_Speed;
	uint16_t TorqueLimit;
}ServoCfg;


typedef struct tag_XL320_StatusPacket
{
	uint16_t ID;			// Response device ID
	uint16_t Length;		// Packet Length	
	uint8_t	Instruction;	// Instruction response
	eXL320_ERROR Error;		// Response error
	uint16_t CRC;			// Packet CRC
	uint8_t *Params;		// Pointer to responsed params
}XL320_StatusPacket;

void sendData(uint8_t *, uint16_t);
void SwitchComDir(uint32_t);
void ClearSerialBuffer(void);

int32_t GetRegisterLength(uint8_t );
int32_t ReadRegister(uint8_t, eXLCMD, uint8_t);
int32_t MakePacket(uint8_t, eXLCMD, uint8_t, uint8_t *, uint8_t);
int32_t sendPacket(uint16_t, eXLCMD, uint16_t, uint8_t *, uint8_t);
int32_t RXsendPacket(uint16_t, eXLCMD, uint16_t, uint8_t *, uint8_t);
uint32_t Process_rdPacket(uint8_t *, XL320_StatusPacket *);

int32_t ReadServoEEPROM(uint8_t ID, ServoCfg *pCfg);
int32_t PingServo(uint8_t);
void ServoEnableTorque(uint8_t, uint16_t);
void moveServo(uint8_t , uint16_t);
void setServoID(uint8_t , uint16_t);
void setServoSpeed(uint8_t , uint16_t);
void setServoLED(uint8_t , eLed_Color);
void setServoPID(uint8_t ID, uint8_t P, uint8_t D, uint8_t I);
void setServoTorque(uint8_t , uint16_t);
void ResetServo(uint8_t ID);

void XL320_Init(struct usart_module *);


#endif
