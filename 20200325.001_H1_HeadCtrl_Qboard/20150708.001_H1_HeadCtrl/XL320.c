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

#include <string.h>
#include "asf.h"
#include "user.h"
#include "dxl_pro.h"
#include "XL320.h"

volatile uint8_t rx_servo[SERVO_BUFFER_LENGTH];
volatile uint8_t tx_servo[SERVO_BUFFER_LENGTH];

XL320_StatusPacket StatusPacket;
struct usart_module *UART_Instance;

ServoCfg ServoDfltCfg = {
	.EEPROM = {
					.Model = {0x5e, 0x01}, 
					.FirmVersion =  0x1e,
					.ID = 0x01, 
					.BaudRate = 0x03, 
					.ReturnDelay = 0xfa, 
					.CW_Limit = {0x00, 0x00}, 
					.CCW_Limit = {0xff, 0x03}, 
					.GhostByte = 0x04, 
					.CtrlMode = 0x02, 
					.Temp_Limit = 0x41, 
					.LowVolt_Limit = 0x3c, 
					.TopVolt_Limit = 0x5a, 
					.Torque_Max = {0xff, 0x03}, 
					.Return_Level = 0x02, 
					.Alarm_ShutDown = 0x03, 
					.Dummy = 0x00
				},
	.LED = 0x0,
	.D_Gain = 0,
	.I_Gain = 0,
	.P_Gain = 32,
	.Home_Position = 512,
	.Moving_Speed = 1023,
	.TorqueLimit = 1023
};

ServoCfg ServoConfig[2];

void sendData(uint8_t *buffer, uint16_t length)
{
	if(UART_Instance == NULL)
		return;
	usart_write_buffer_wait(UART_Instance, buffer, length);
}

void SwitchComDir(uint32_t direction)
{
	if(UART_Instance == NULL)
		return;

	if(direction)
	{
		usart_enable_transceiver(UART_Instance, USART_TRANSCEIVER_TX);
		usart_disable_transceiver(UART_Instance, USART_TRANSCEIVER_RX);
	}
	else
	{
		usart_enable_transceiver(UART_Instance, USART_TRANSCEIVER_RX);
		usart_disable_transceiver(UART_Instance, USART_TRANSCEIVER_TX);
	}
}

void ClearSerialBuffer(void)
{
	if(UART_Instance == NULL)
		return;
	usart_disable_transceiver(UART_Instance, USART_TRANSCEIVER_RX);
	usart_enable_transceiver(UART_Instance, USART_TRANSCEIVER_RX);
}

int32_t GetRegisterLength(uint8_t Reg)
{
	switch(Reg)
	{
		case XL_VERSION:
		case XL_ID:
		case XL_BAUD_RATE:
		case XL_RETURN_DELAY_TIME:
		case XL_CONTROL_MODE:
		case XL_LIMIT_TEMPERATURE:
		case XL_DOWN_LIMIT_VOLTAGE:
		case XL_UP_LIMIT_VOLTAGE:
		case XL_RETURN_LEVEL:
		case XL_ALARM_SHUTDOWN:
		case XL_TORQUE_ENABLE:
		case XL_LED:
		case XL_D_GAIN :
		case XL_I_GAIN :
		case XL_P_GAIN :
		case XL_PRESENT_VOLTAGE:
		case XL_PRESENT_TEMPERATURE:
		case XL_REGISTERED_INSTRUCTION:
		case XL_MOVING:
		case XL_HARDWARE_ERROR:
			return 1;
			break;
		default:
			return 2;
			break;
	}
}

int32_t MakePacket(uint8_t ID, eXLCMD cmd, uint8_t Address, uint8_t *pData, uint8_t nParams)
{
	uint16_t wchecksum, i = 0;

	if(UART_Instance == NULL || nParams > (SERVO_BUFFER_LENGTH - 9))
		return 0;

	tx_servo[i++] = 0xff;
	tx_servo[i++] = 0xff;
	tx_servo[i++] = 0xfd;
	tx_servo[i++] = 0x00;
	tx_servo[i++] = ID;

	switch(cmd)
	{
		case eXLCMD_READ:
			tx_servo[i++] = 7;
			break;
		case eXLCMD_WRITE:
			tx_servo[i++] = nParams + 5;
			break;
		case eXLCMD_FACTORYRESET:
			tx_servo[i++] = 4;
			break;
		default:
			tx_servo[i++] = 3;
			break;
	}

	tx_servo[i++] = 0;
	tx_servo[i++] = cmd;

	switch(cmd)
	{
		case eXLCMD_PING:
		case eXLCMD_ACTION:
		case eXLCMD_REBOOT:
		case eXLCMD_FACTORYRESET:
		break;
		default:
			tx_servo[i++] = (uint8_t)Address;
			tx_servo[i++] =	0;
		break;
	}

	if(cmd == eXLCMD_READ)
	{
		tx_servo[i++] = (uint8_t)nParams;
		tx_servo[i++] =	0;
	}

	if(pData != NULL)
		for(uint32_t x = 0; x < nParams; x++)
			tx_servo[i++] = *(pData + x);

	wchecksum = update_crc(0, (uint8_t *)tx_servo, i);
	tx_servo[i++] = (uint8_t)wchecksum;
	tx_servo[i++] = (uint8_t)(wchecksum >> 8);

	return i;
}

uint32_t Process_rdPacket(uint8_t *packet, XL320_StatusPacket *pStatus)
{
	uint32_t header;
	uint16_t checksum;

	header = (*packet << 24 | *(packet + 1) <<16 | *(packet + 2) <<8  | *(packet + 3)) & 0xfffffd00;
	pStatus->ID = *(packet + 4);
	pStatus->Length = *(packet + 6) << 8 |  *(packet + 5);
	
	if(pStatus->Length > SERVO_BUFFER_LENGTH - 6)
	return -1;
	
	pStatus->Error = 0xff;

	if(header  == 0xfffffd00)
	{
		pStatus->Instruction = *(packet + 7);
		pStatus->CRC = *(packet + (pStatus->Length + 6)) <<8 | *(packet + (pStatus->Length + 5));

		checksum = update_crc(0, packet, pStatus->Length + 5);

		if(checksum == pStatus->CRC)
		{
			pStatus->Error = *(packet + 8);
			pStatus->Params = (packet + 9);
		}
	}

	return pStatus->Error;
}

int32_t ReadRegister(uint8_t ID, eXLCMD CMD, uint8_t Reg)
{
	uint32_t result = -1;

	for(uint32_t x = 0; x < 3; x++)
	{
		if(RXsendPacket(ID, CMD, Reg, NULL, GetRegisterLength(Reg)) && Process_rdPacket((uint8_t *)rx_servo, &StatusPacket) == eNOERROR)
		{
			if(GetRegisterLength(Reg) == 1)
				result = (uint32_t)*StatusPacket.Params;
			else if(CMD == eXLCMD_PING)
				result = (uint32_t)(*StatusPacket.Params | *(StatusPacket.Params + 1) <<8) | *(StatusPacket.Params + 2) <<16;
			else
				result = (uint32_t)(*StatusPacket.Params | *(StatusPacket.Params + 1) <<8);

			break;
		}
	}
	return result;
}

int32_t sendPacket(uint16_t ID, eXLCMD cmd, uint16_t Address, uint8_t *pData, uint8_t nParams)
{
	uint8_t length = MakePacket(ID, cmd, Address, pData, nParams);
	
	if(length)
	{
		SwitchComDir(Tx_MODE);
		sendData((uint8_t *)tx_servo, length);
		SwitchComDir(Rx_MODE);
		return 1;
	}
	else 
		return 0;
}

int32_t RXsendPacket(uint16_t ID, eXLCMD cmd, uint16_t Address, uint8_t *pData, uint8_t nParams)
{
	uint32_t timeout_tm, lbytes = 0;
	
	if(!sendPacket(ID, cmd, Address, pData, nParams))
		return 0;

	timeout_tm =  GetTickCount();
	
	usart_read_buffer_job(UART_Instance, (uint8_t *)rx_servo, SERVO_BUFFER_LENGTH);

	while(TimeDiff(timeout_tm, GetTickCount()) < 2)
	{
		if(UART_Instance->remaining_rx_buffer_length != lbytes)
		{
			lbytes = UART_Instance->remaining_rx_buffer_length;
			timeout_tm = GetTickCount();
		}
	}
	usart_abort_job(UART_Instance, USART_TRANSCEIVER_RX);
	lbytes = SERVO_BUFFER_LENGTH - lbytes;
	return lbytes;
}

int32_t ReadServoEEPROM(uint8_t ID, ServoCfg *pCfg)
{
	if(RXsendPacket(ID, eXLCMD_READ, XL_MODEL_NUMBER_L, NULL, sizeof(ServoEEPROM)) && Process_rdPacket((uint8_t *)rx_servo, &StatusPacket) == eNOERROR)
	{
		memcpy(pCfg, StatusPacket.Params, sizeof(ServoEEPROM));
		return 0;
	}
	return -1;
}

int32_t PingServo(uint8_t ID)
{
	if(RXsendPacket(ID, eXLCMD_PING, 0, NULL, 0) && Process_rdPacket((uint8_t *)rx_servo, &StatusPacket) == eNOERROR)
	{
		return (uint32_t)(*StatusPacket.Params | *(StatusPacket.Params + 1) <<8) | *(StatusPacket.Params + 2) <<16;
	}
	return -1;
}

void ServoEnableTorque(uint8_t ID, uint16_t value)
{
	uint16_t data = value & 0x01;
	sendPacket(ID, eXLCMD_WRITE, XL_TORQUE_ENABLE, (uint8_t *)&data, GetRegisterLength(XL_TORQUE_ENABLE));
}

void moveServo(uint8_t ID, uint16_t value)
{
	uint16_t data = value;

	if (data < 0)
		data = 0;
	else if(data > 1023)
		data = 1023;

	sendPacket(ID, eXLCMD_WRITE, XL_GOAL_POSITION, (uint8_t *)&data, GetRegisterLength(XL_GOAL_POSITION));
}

void setServoID(uint8_t ID, uint16_t value)
{
	uint16_t data = value;
	sendPacket(ID, eXLCMD_WRITE, XL_ID, (uint8_t *)&data, GetRegisterLength(XL_ID));
}

void setServoSpeed(uint8_t ID, uint16_t value)
{
	uint16_t data = value;
	sendPacket(ID, eXLCMD_WRITE, XL_GOAL_SPEED, (uint8_t *)&data, GetRegisterLength(XL_GOAL_SPEED));
}

void setServoLED(uint8_t ID, eLed_Color led_color)
{
	uint16_t data = led_color;
	sendPacket(ID, eXLCMD_WRITE, XL_LED, (uint8_t *)&data, GetRegisterLength(XL_LED));
}

void setServoPID(uint8_t ID, uint8_t P, uint8_t D, uint8_t I)
{
	uint8_t data[3] = {P, I, D};
	for(uint32_t x = 0; x < 3; x++)
		sendPacket(ID, eXLCMD_WRITE, XL_D_GAIN + x, (uint8_t *)&data[x], GetRegisterLength(XL_D_GAIN + x));
}

void setServoTorque(uint8_t ID, uint16_t value)
{
	uint16_t data = value;
	sendPacket(ID, eXLCMD_WRITE, XL_GOAL_TORQUE, (uint8_t *)&data, GetRegisterLength(XL_GOAL_TORQUE));
}

void ResetServo(uint8_t ID)
{
	sendPacket(ID, eXLCMD_REBOOT, 0, NULL, 0);
}

void XL320_Init(struct usart_module * Instance)
{
	UART_Instance = Instance;
	ReadServoEEPROM(1, &ServoConfig[0]);
	ReadServoEEPROM(2, &ServoConfig[1]);
	setServoPID(1, 10, 1, 2);
	setServoPID(2, 10, 1, 2);
}



