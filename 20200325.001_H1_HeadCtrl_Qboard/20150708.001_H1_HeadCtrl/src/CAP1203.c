/****************************************************************************
 * Copyright (C) 2015 Sensorian
 *                                                                          *
 * This file is part of Sensorian.                                          *
 *                                                                          *
 *   Sensorian is free software: you can redistribute it and/or modify it   *
 *   under the terms of the GNU Lesser General Public License as published  *
 *   by the Free Software Foundation, either version 3 of the License, or   *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   Sensorian is distributed in the hope that it will be useful,           *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Lesser General Public License for more details.                    *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with Sensorian.                                          *
 *   If not, see <http://www.gnu.org/licenses/>.                            *
 ****************************************************************************/
 
#include "asf.h"
#include "user.h"
#include "CAP1203.h"
#include "serialProtocol.h"
#include "XL320.h"

volatile uint32_t touch_tm = 0, press_cnt[3] = {0,0,0};
volatile uint8_t device_ID = 0, sensitive = 0, pressed_button = 0, servo_moving = 0;
volatile uint32_t press_thd = 1; 
volatile uint32_t servo_off_delay = 1000;	// Delay after servo off to enable touch update
volatile uint32_t servo_off_tm = 0;

void CAP1203_Initialize(void)
{
    CAP1203_ActiveMode();                       //All three sensors are monitored in Active mode
	device_ID = CAP1203_ReadID();
	CAP1203_Write(MULTITOUCH, 0);
	CAP1203_Config(CS1|CS2|CS3, 0x6f, SMPAVG128 | STM320us| T140ms);
	touch_tm = GetTickCount();
}

void CAP1203_Config(uint8_t en_channels, uint8_t sensitivity, uint8_t sample)
{
    CAP1203_Write(SENSINPUTEN, en_channels & 0x03);     //Set active inputs
	CAP1203_Write(CAL_ACTIV, en_channels);
    CAP1203_Write(AVERAGE_SAMP_CONF, sample);			//Setup averaging and sampling time
	CAP1203_Write(SENSITIVITY, sensitivity);
}

uint8_t CAP1203_ActiveMode(void)
{
    uint8_t status = CAP1203_Read(MAIN_CTRL_REG);
    status &= ~STBY;
    CAP1203_Write(MAIN_CTRL_REG,status);
    return CAP1203_Read(MAIN_CTRL_REG);
}

uint8_t CAP1203_StandbyMode(void)
{
    CAP1203_Write(STANDBY_SENS,0x07);       //Set sensitivity in standby mode    
    uint8_t status = CAP1203_Read(MAIN_CTRL_REG);
    status |= STBY;
    CAP1203_Write(MAIN_CTRL_REG,status);
    return CAP1203_Read(MAIN_CTRL_REG);
}

uint8_t CAP1203_DeepSleep(void)
{
    uint8_t status = CAP1203_Read(MAIN_CTRL_REG);
    status |= SLEEP;                            //Set Sleep bit
    CAP1203_Write(MAIN_CTRL_REG,status);        //Update register
    return CAP1203_Read(MAIN_CTRL_REG);
}

uint8_t CAP1203_ResumeFromDeepSleep(void)
{
    uint8_t status = CAP1203_Read(MAIN_CTRL_REG);
    status &= ~SLEEP;
    CAP1203_Write(MAIN_CTRL_REG,status);
    return CAP1203_Read(MAIN_CTRL_REG);
}

void CAP1203_ConfigureMultiTouch(touch_type number,uint8_t mulchan )
{
    CAP1203_Write(MULTITOUCH,((uint8_t)number)|MULTBLK_EN);       //Set number of simultaneous touches
    CAP1203_Write(MULTIPATCONF,MTP_EN|MTP_TH1|MTP_TH0|COMP_PTRN|MTP_ALERT);       //Enable multitouch
    CAP1203_Write(MULTIPATTERN,mulchan);
}

uint8_t CAP1203_MultitouchEvent(void)
{
    uint8_t mt = 0;
    uint8_t multi = CAP1203_Read(GEN_STATUS);
    if((multi & MULT) == MULT)
    {
        mt = 1;
    }
    return mt;
}

void CAP1203_SetPowerButton(button_type button)
{
    CAP1203_Write(PWR_BUTTON,button);
    CAP1203_Write(PWR_CONFIG,PWR_EN|TIME1120ms);        //Configure as power button in Active mode
}   


uint8_t CAP1203_ReadPowerButton(void)
{
    uint8_t pressed = 0;
    uint8_t button = CAP1203_Read(GEN_STATUS);
    
    if (button & PWR)
    {
        pressed = 1;
    }else{
        pressed = 0;
    }
    return pressed;
}

uint8_t CAP1203_ReadPressedButton(void)
{
    uint8_t buttonPressed = 0;

    volatile uint8_t status = CAP1203_GetStatusReg();      //Check if touch bit was registered

    if (status & TOUCH)
    {
        uint8_t button = CAP1203_Read(SENSOR_INPUTS);
        switch(button)
        {
            case CS1:
                    buttonPressed = 1;
                break;
            case CS2:
                    buttonPressed = 2;
                break;
            case CS3:
                    buttonPressed = 3;
                break;
            default:
                    buttonPressed = 0;
                break;
        }
		CAP1203_Write(MAIN_CTRL_REG,0x00);      //Clear interrupt
		CAP1203_Write(CAL_ACTIV, 7);
    }   
    return buttonPressed;    
}

uint8_t CAP1203_GetStatusReg(void)
{
    uint8_t status = CAP1203_Read(GEN_STATUS);
    return status;
}

void CAP1203_EnableInterrupt(button_type pin)
{
    CAP1203_Write(REPEAT_RATE,pin);     //Enable repeat rate for the input pins
    CAP1203_Write(INT_ENABLE,pin);      //2:0 c2:c1:c0  last three bits are the specific channels
}

void CAP1203_SetSensitivity(sensitivity_type sensitivity)
{
    CAP1203_Write(0x00,sensitivity);
}

uint8_t CAP1203_CheckSensorStatus(void)
{
    uint8_t sensor = 0;

    return sensor;
}

uint8_t CAP1203_ClearInterrupt(void)
{
    uint8_t intStatus = 0x00;
    CAP1203_Read(GEN_STATUS);
    return intStatus;
}

uint16_t CAP1203_ReadID(void)
{
    uint8_t id = CAP1203_Read(PRODUCT_ID);
    uint8_t manu = CAP1203_Read(MAN_ID);
    return ((manu << 8)|id);
}


void CAP1203_Write(uint8_t reg, uint8_t data)
{
    //I2C_WriteByteRegister(reg,data);                //Write data to register reg
	I2C_WriteData(&data, reg, 1);
}

uint8_t CAP1203_Read(uint8_t reg)
{
    //uint8_t value = I2C_ReadByteRegister(reg);
	uint8_t value;
	I2C_ReadData(&value, reg, 1);
    return value;
}

uint8_t GetPressedButton(void)
{
	uint8_t aux = pressed_button;
	pressed_button = 0;
	return aux;
}

void SetServoMovingFlag(uint8_t id)
{
	servo_moving = id;
	if(id & 0x1)
	{
		setServoLED(1, 1);
		Delay_ms(2);
	}
	if(id & 0x2)
	{
		setServoLED(2, 1);
		Delay_ms(2);
	}
}

void SetServoMovingDelay(uint32_t value)
{
	servo_off_delay = value;
} 

uint8_t GetTouch_runtime(void)
{
	uint8_t value = 0, is_moving = 0;
	
	if(TimeDiff(touch_tm, GetTickCount()) > TOUCH_SAMPLETIME)
	{
		if(servo_moving)
		{
			if(servo_moving & 0x01)
				is_moving = ReadRegister(1, eXLCMD_READ, XL_MOVING);
			else if(servo_moving & 0x02)
				is_moving = ReadRegister(2, eXLCMD_READ, XL_MOVING) << 1;

			if(is_moving)
				servo_off_tm = GetTickCount();
			else if(servo_moving && TimeDiff(servo_off_tm, GetTickCount()) > servo_off_delay)
			{
				if(servo_moving & 0x1)
					setServoLED(1, 0);
				if(servo_moving & 0x2)
					setServoLED(2, 0);
				servo_moving = 0;
			}
			CAP1203_ReadPressedButton();
		}
		else 
			value = CAP1203_ReadPressedButton();

		if(value)
		{
			uint32_t idx = (value & 0x03) - 1;

			if(++press_cnt[idx] > press_thd)
			{
				pressed_button = value;
				press_cnt[idx] = 0;
			}
		}
		else
			for(uint32_t x = 0; x < 3; x++)
				if(press_cnt[x])
					press_cnt[x]--;
			
		touch_tm = GetTickCount();
		return pressed_button;
	}
	return 0;
}