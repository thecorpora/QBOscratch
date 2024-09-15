/*********************************************************************
 *
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2011, TheCorpora.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the TheCorpora nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.

 *
 * \author Miguel Angel Julian
 *********************************************************************/
#include "asf.h"
#include "user.h"
#include "serialProtocol.h"
#include "XL320.h"
#include "Analog.h"
#include "CAP1203.h"

const uint8_t pearsondata[] = {
0x00, 0x77, 0xee, 0x99, 0x07, 0x70, 0xe9, 0x9e, 0x0e, 0x79, 0xe0, 0x97,
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
0xb4, 0xc3, 0x5a, 0x2d };

uint8_t *rx_start;
uint8_t Serial_RXBuf[128];
uint32_t lRX_bytes;

uint8_t touch_auto_off = 0x03;	// Enables ignore touch on servo 1 and 2 when are moving  
uint16_t trig_angle = 3;

struct usart_module *UART_SerialPort_Instance = NULL;


uint8_t pearson(uint8_t *key, uint8_t len)
{
	uint8_t hash = 0;
	for (uint8_t i = 0; i < len; i++)
		hash = pearsondata[hash ^ key[i]];
	return (hash);
}

void fSerial_InitSerialProtocol(uint32_t *MasterCom_Idx)
{
	command_.commandNumber = 0;
	command_.nInputData = 0;
	command_.nOutputData = 0;
	UART_SerialPort_Instance =(void *) MasterCom_Idx;
	if(UART_SerialPort_Instance != NULL)
	{
		usart_read_buffer_job(UART_SerialPort_Instance, Serial_RXBuf, 128);
		rx_start = Serial_RXBuf;
		lRX_bytes = 128;
	}
}

bool fSerial_procesaEntrada(uint8_t* buf, uint8_t nBytes)
{
	if (nBytes < 5)
		return false;
	if (buf[0] != INPUT_FLAG)
		return false;
	if (buf[nBytes - 1] != OUTPUT_FLAG)
		return false;
	/* Disabled checksum for debug purposes
	 */
	uint8_t check = pearson(buf + 1, nBytes - 3);
	uint8_t inCheck = buf[nBytes - 2];

	if (check != inCheck)
		return false;
	
	if (buf[2] != nBytes - 5)
		return false;
	if (buf[2] > 50)
		return false;
	command_.commandNumber = buf[1];
	command_.nInputData = nBytes - 5;
	for (int i = 3; i < nBytes - 2; i++)
		command_.inputData[i - 3] = buf[i];
	return true;
}

void fSerial_processSerial()
{
	if (UART_SerialPort_Instance == NULL)
		return;

	uint8_t *incomingByte ;

	if(UART_SerialPort_Instance->remaining_rx_buffer_length != lRX_bytes)
	{
		incomingByte = (uint8_t *)UART_SerialPort_Instance->rx_buffer_ptr -1;
		lRX_bytes = UART_SerialPort_Instance->remaining_rx_buffer_length;
	}
	else 
		return;

	if (*incomingByte == INPUT_FLAG)  //Llega el flag de entrada
	{
		rx_start = (uint8_t *)UART_SerialPort_Instance->rx_buffer_ptr -1;
		return;
	}
	else if (*incomingByte == OUTPUT_FLAG)  //Llega el flag de salida
	{
		if (fSerial_procesaEntrada(rx_start, (incomingByte - rx_start) + 1))
		{
			fSerial_processCommands();
			fSerial_sendResponse();
		}
		else
			fSerial_sendNack();

		usart_read_buffer_job(UART_SerialPort_Instance, Serial_RXBuf, 128);
		rx_start = Serial_RXBuf;
		lRX_bytes = 128;
		return;
	}
	else if (isInputEscaped_)
	{
		*incomingByte += 2; //Desescapamos la entrada (ya hemos pasado la comprobación de los flags de entrada y salida)
		isInputEscaped_ = false;							//Si no estaba activo, seguimos, pues ya se ha hecho la corrección al dato de entrada
	}
	else if (*incomingByte == INPUT_ESCAPE)  //Llega el escape de entrada
	{
		UART_SerialPort_Instance->rx_buffer_ptr--;	// Retrocedemos el puntero un byte atras
		isInputEscaped_ = true;		//Activamos el escape de la entrada
		return;						//y vamos a por el siguiente dato
	}
}

void fSerial_processCommands()
{
	isInputCorrect_ = true;
	switch (command_.commandNumber)
	{
	case GET_VERSION:
		if (command_.nInputData != 0)
		{
			isInputCorrect_ = false;
		}
		else
		{
			command_.nOutputData = 2;
			command_.outputData[0] = BOARD_ID;
			command_.outputData[1] = LIBRARY_VERSION;
		}
		break;
	case SET_MOUTH_VALUE:
		if (command_.nInputData != 4)
		{
			isInputCorrect_ = false;
		}
		else
		{
			// uint32_t data = command_.inputData[0] <<24 | command_.inputData[1] <<16 | command_.inputData[2] <<8 | command_.inputData[3];
			command_.nOutputData = 0;
			fMouth_SetData(*((uint32_t *)command_.inputData));
		}
		break;
	case SET_STATE:
		if (command_.nInputData != 1)
		{
			isInputCorrect_ = false;
		}
		else
		{
			command_.nOutputData = 0;
			fMouth_SetNoseColor(command_.inputData[0]);
		}
		break;
	
	case GET_TOUCH: 
		if (command_.nInputData != 0)
			isInputCorrect_ = false;
		else
		{
			command_.nOutputData = 1;
			command_.outputData[0] = GetPressedButton();
		}
		break;
	case SET_TOUCH_PARAMS:
		if (command_.nInputData != 3)
		{
			isInputCorrect_ = false;
		}
		else
		{
			command_.nOutputData = 0;
			CAP1203_Config(command_.inputData[0], command_.inputData[1], command_.inputData[2]);
		}
		break;
	case SET_MIC_INPUT:
		if (command_.nInputData != 1)
		{
			isInputCorrect_ = false;
		}
		else
		{
			command_.nOutputData = 0;
			if(command_.inputData[0] != SetMicIndex(command_.inputData[0]))
				isInputCorrect_ = false;
		}
		break;

	case GET_MIC_REPORT:
		if (command_.nInputData != 0)
		{
			isInputCorrect_ = false;
		}
		else
		{
			uint16_t *pData = (uint16_t *)&command_.outputData[0];
			float *pRMS = GetRMSdata();
			command_.nOutputData = 6;

			*pData++ = (uint16_t)(*pRMS++ * 10000);
			*pData++ = (uint16_t)(*pRMS++ * 10000);
			*pData++ = (uint16_t)(*pRMS++ * 10000);
		}
		break;

	case RESET_SERVO:
		if (command_.nInputData != 1)
			isInputCorrect_ = false;
		else
		{
			command_.nOutputData = 0;
			ResetServo(command_.inputData[0]);
		}
		break;

	case SET_SERVO:
		if (command_.nInputData != 5)
		{
			isInputCorrect_ = false;
		}
		else
		{

			uint16_t angleMilliseconds = *(command_.inputData + 1) | *(command_.inputData + 2) <<8;
			uint16_t servoSpeed = *(command_.inputData + 3)  | *(command_.inputData + 4) << 8;

			command_.nOutputData = 0;

			SetServoMovingFlag(command_.inputData[0] & touch_auto_off);

			setServoSpeed(command_.inputData[0], servoSpeed);
			Delay_ms(2);
			moveServo(command_.inputData[0], angleMilliseconds);
		}
		break;
		
	case SET_SERVO_ANGLE_REL:  // tres parametros: axis, angle_0, angle_1.
		if (command_.nInputData != 3)
		{
			isInputCorrect_ = false;
		}
		else
		{
			int16_t rel_angle, dest_angle;
			command_.nOutputData = 0;
			
			rel_angle = (int16_t)((command_.inputData[2] << 8) | command_.inputData[1]);

			// If relative movement is <trig_angle then auto touch off isn´t trigger 
			if(abs(rel_angle) > trig_angle)
				SetServoMovingFlag(command_.inputData[0] & touch_auto_off);

			// get current servo position.
			dest_angle = ReadRegister(command_.inputData[0], eXLCMD_READ, XL_PRESENT_POSITION);
			dest_angle += rel_angle;
			moveServo(command_.inputData[0], dest_angle);
		}
		break;

	case SET_SERVO_ENABLE:
		{
			if (command_.nInputData != 2)
			isInputCorrect_ = false;
			else
			{
				command_.nOutputData = 0;
				ServoEnableTorque(command_.inputData[0], command_.inputData[1]);
			}
		}
		break;

	case SET_SERVO_ANGLE:
	case SET_SERVO_SPEED:        
	case SET_SERVO_CW_LIM:
	case SET_SERVO_CCW_LIM:
		if (command_.nInputData != 3)
			isInputCorrect_ = false;
		else
		{
			uint8_t getReg = XL_CW_ANGLE_LIMIT_L;

			if(command_.commandNumber == SET_SERVO_CCW_LIM)
				getReg = XL_CCW_ANGLE_LIMIT_L;
			else if(command_.commandNumber == SET_SERVO_ANGLE)
			{
				getReg = XL_GOAL_POSITION;
				SetServoMovingFlag(command_.inputData[0] & touch_auto_off);
			}
			else if(command_.commandNumber == SET_SERVO_SPEED)
				getReg = XL_GOAL_SPEED;

			command_.nOutputData = 0;
			sendPacket(command_.inputData[0], eXLCMD_WRITE, getReg, &command_.inputData[1], 2);
		}
		break;

	case SET_SERVO_ID:
		if(command_.inputData[1] > 252)
		{
			isInputCorrect_ = false;
			break;
		} 

	case SET_SERVO_BAUD:

		if (command_.nInputData != 2)
			isInputCorrect_ = false;
		else
		{
			uint8_t getReg = XL_ID;

			if(command_.commandNumber == SET_SERVO_BAUD)
			{
				getReg = XL_BAUD_RATE;
				command_.inputData[1] &= 0x03;
			}
			command_.nOutputData = 0;
			sendPacket(command_.inputData[0], eXLCMD_WRITE, getReg, &command_.inputData[1], 1);
		}
		break;

	case SET_SERVO_PID:
		if (command_.nInputData != 4)
			isInputCorrect_ = false;
		else
		{
			command_.nOutputData = 0;
			sendPacket(command_.inputData[0], eXLCMD_WRITE, XL_D_GAIN, &command_.inputData[1], 3);
		}
		break;

	case SET_SERVO_LED:
		if (command_.nInputData != 2)
		isInputCorrect_ = false;
		else
		{
			command_.nOutputData = 0;
			setServoLED(command_.inputData[0], command_.inputData[1]);
		}
		break;

	case GET_SERVO_POSITION:
	case GET_SERVO_SPEED:
	case GET_SERVO_LOAD:
	case GET_SERVO_CW_LIM:
	case GET_SERVO_CCW_LIM:
		if (command_.nInputData != 1)
			isInputCorrect_ = false;
		else
		{
			uint8_t getReg;
			int32_t value;
			switch(command_.commandNumber)
			{
				case GET_SERVO_POSITION:
					getReg = XL_PRESENT_POSITION;
					break;
				case GET_SERVO_SPEED:
					getReg = XL_PRESENT_SPEED;
					break;
				case GET_SERVO_LOAD:
					getReg = XL_PRESENT_LOAD;
					break;
				case GET_SERVO_CW_LIM:
					getReg = XL_CW_ANGLE_LIMIT_L;
					break;
				case GET_SERVO_CCW_LIM:
					getReg = XL_CCW_ANGLE_LIMIT_L;
					break;
				default:
					command_.nOutputData = 0;
					isInputCorrect_ = false;
					return;
			}
			
			value = ReadRegister(command_.inputData[0], eXLCMD_READ, getReg);
			if(value != -1)
			{
				command_.nOutputData = 2;
				uint8_t* intToBytes = (uint8_t*) &value;
				command_.outputData[0] = intToBytes[0];
				command_.outputData[1] = intToBytes[1];
			}
			else 
				isInputCorrect_ = false;
		}
		break;

	case GET_SERVO_TEMP:
	case GET_SERVO_VOLTAGE:
	case SERVO_ISMOVING:
	case GET_HARDWARE_ERROR:
		if (command_.nInputData != 1)
			isInputCorrect_ = false;
		else
		{
			uint8_t getReg; 
			int32_t value;

			switch(command_.commandNumber)
			{
				case GET_SERVO_VOLTAGE:
					getReg = XL_PRESENT_VOLTAGE;
					break;
				case GET_SERVO_TEMP:
					getReg = XL_PRESENT_TEMPERATURE;
					break;
				case SERVO_ISMOVING:
					getReg = XL_MOVING;
					break;
				default:
					getReg = XL_HARDWARE_ERROR;
			}
		
			value = ReadRegister(command_.inputData[0], eXLCMD_READ, getReg);
			if(value != -1)
			{
				command_.nOutputData = 1;
				uint8_t* intToBytes = (uint8_t*) &value;
				command_.outputData[0] = intToBytes[0];
			}
			else
				isInputCorrect_ = false;
		}
		break;

	case GET_SERVO_BYTE_REG:
	case GET_SERVO_WORD_REG:
		if (command_.nInputData != 2)
			isInputCorrect_ = false;
		else if(command_.inputData[1] < 52)
		{
			int32_t value  = ReadRegister(command_.inputData[0], eXLCMD_READ, command_.inputData[1]);;

			if(value != -1)
			{
				uint8_t* intToBytes = (uint8_t*) &value;

				if(command_.commandNumber == GET_SERVO_BYTE_REG)
				{
					command_.nOutputData = 1;
					command_.outputData[0] = intToBytes[0];
				}
				else if(command_.commandNumber == GET_SERVO_WORD_REG)
				{
					command_.nOutputData = 2;
					command_.outputData[0] = intToBytes[0];
					command_.outputData[1] = intToBytes[1];
				}
				else
					isInputCorrect_ = false;
			}
		}
		break;

	case GET_HEAD_SERVOS:
		if (command_.nInputData != 0)
			isInputCorrect_ = false;
		else
		{
			volatile int16_t servoPosition = 0;  
			volatile uint8_t* intToBytes; 

			command_.nOutputData = 4;
			servoPosition = ReadRegister(1, eXLCMD_READ, XL_PRESENT_POSITION);
			intToBytes = (uint8_t*) &servoPosition;
			command_.outputData[0] = intToBytes[0];
			command_.outputData[1] = intToBytes[1];

			servoPosition = ReadRegister(2, eXLCMD_READ, XL_PRESENT_POSITION);
			intToBytes = (uint8_t*) &servoPosition;
			command_.outputData[2] = intToBytes[0];
			command_.outputData[3] = intToBytes[1];
		}
		break;

	case SET_SERVO_SWITCH:
		if (command_.nInputData != 1)
			isInputCorrect_ = false;
		else
		{
			command_.nOutputData = 0;
			ioport_set_pin_level(SERVO_EN, command_.inputData[0]);
		}
		break;

	case SET_ENABLE_SPEAKER:
		if (command_.nInputData != 1)
			isInputCorrect_ = false;
		else
		{
			command_.nOutputData = 0;
			ioport_set_pin_level(SPK_EN, command_.inputData[0]);
		}
		break;

	case SET_MEAN_RMS:
		if (command_.nInputData != 1)
			isInputCorrect_ = false;
		else
		{
			command_.nOutputData = 0;
			SetRMSorMEAN((uint32_t)command_.inputData[0]);
		}
		break;

	case SET_USB2SERVO_FWD:
		if (command_.nInputData != 1)
			isInputCorrect_ = false;
		else
		{
			command_.nOutputData = 0;
			EnableUSBPortForwarding((uint32_t)command_.inputData[0]);
		}
		break;

	case SET_ADC_REF:
		if (command_.nInputData != 1)
			isInputCorrect_ = false;
		else
		{
			command_.nOutputData = 0;
			SetAdcReference((uint32_t)command_.inputData[0]);
		}
		break;

	case SET_DAC_REF:
		if (command_.nInputData != 1)
			isInputCorrect_ = false;
		else
		{
			command_.nOutputData = 0;
			SetDACReference((uint32_t)command_.inputData[0]);
		}
		break;

	case SET_TOUCH_AUTO_OFF:
		if (command_.nInputData != 4)
			isInputCorrect_ = false;
		else
		{
			uint16_t delay =  (uint16_t)((command_.inputData[2] << 8) | command_.inputData[1]);
			
			trig_angle = (uint16_t)( 0x00ff & command_.inputData[3]);

			command_.nOutputData = 0;

			touch_auto_off = command_.inputData[0];
			SetServoMovingDelay((uint32_t)delay);			
		}
		break;

	default:
		command_.nOutputData = 0;
		isInputCorrect_ = false;
		break;
	}
}

void fSerial_sendResponse()
{
	if (isInputCorrect_)
	{
		uint8_t temporalDataArray[128];
		uint8_t outData[128];
		uint8_t nOutData = 0;
		uint8_t check;
		outData[nOutData] = INPUT_FLAG;
		nOutData++;

		temporalDataArray[0] = command_.commandNumber;
		temporalDataArray[1] = command_.nOutputData;

		for (int i = 0; i < command_.nOutputData; i++)
			temporalDataArray[i + 2] = command_.outputData[i];

		check = pearson(temporalDataArray, command_.nOutputData + 2);
		temporalDataArray[command_.nOutputData + 2] = check;

		for (int i = 0; i < command_.nOutputData + 3; i++)
		{
			if (temporalDataArray[i] == INPUT_FLAG
					|| temporalDataArray[i] == INPUT_ESCAPE
					|| temporalDataArray[i] == OUTPUT_FLAG)
			{
				outData[nOutData] = INPUT_ESCAPE;
				nOutData++;
				outData[nOutData] = temporalDataArray[i] - 2;
				nOutData++;
			}
			else
			{
				outData[nOutData] = temporalDataArray[i];
				nOutData++;
			}
		}

		outData[nOutData] = OUTPUT_FLAG;
		nOutData++;

		usart_abort_job(UART_SerialPort_Instance, USART_TRANSCEIVER_RX);
		fSerial_Write(outData, nOutData);
	}
	else
	{
		fSerial_sendNack();
		isInputCorrect_ = true;
	}
}
void fSerial_Write(uint8_t *Data, uint32_t nBytes)
{
	if(UART_SerialPort_Instance == NULL)
		return;
	usart_write_buffer_wait(UART_SerialPort_Instance, Data, nBytes);
}

void fSerial_sendNack()
{
	uint8_t outData[2] = { INPUT_FLAG, OUTPUT_FLAG };
	usart_abort_job(UART_SerialPort_Instance, USART_TRANSCEIVER_RX);
	fSerial_Write(outData, 2);
}
