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
 
#ifndef __CAP1203_H__
#define __CAP1203_H__

#define CAP1203ADDR             0x28        //0x28 - not shifted

/*****************************Registers**************************/
#define MAIN_CTRL_REG           0x00
#define GEN_STATUS              0x02
#define SENSOR_INPUTS           0x03
#define NOISE_FLAG              0x0A
#define SENS1DELTACOUNT         0x10
#define SENS2DELTACOUNT         0x11
#define SENS3DELTACOUNT         0x12
#define SENSITIVITY             0x1F
#define CONFIG1                 0x20
#define SENSINPUTEN             0x21
#define SENSINCONF1             0x22
#define SENSINCONF2             0x23
#define AVERAGE_SAMP_CONF       0x24
#define CAL_ACTIV               0x26
#define INT_ENABLE              0x27
#define REPEAT_RATE             0x28
#define MULTITOUCH              0x2A
#define MULTIPATCONF            0x2B
#define MULTIPATTERN            0x2D
#define BASECOUNT               0x2E
#define RECALCONFIG             0x2F
#define S1THRESHOLD             0x30
#define S2THRESHOLD             0x31
#define S3THRESHOLD             0x32
#define SENSTHRESHOLD           0x38

#define STANDBYCHAN             0x40
#define STANDBYCONF             0x41
#define STANDBY_SENS            0x42
#define STANDBY_THRE            0x43
#define CONFIG2                 0x44
#define S1BASECOUNT             0x50
#define S2BASECOUNT             0x51
#define S3BASECOUNT             0x52
#define PWR_BUTTON              0x60
#define PWR_CONFIG              0x61
#define S1INPCAL                0xB1    
#define S2INPCAL                0xB2
#define S3INPCAL                0xB3
#define S1CALLSB                0xB9

#define PRODUCT_ID              0xFD
#define MAN_ID                  0xFE
#define REV                     0xFF    

/************************MAIN CTRL REG********************************/
#define STBY                    0x20
#define SLEEP                   0x08
#define INT                     0x01
/************************GEN_STATUS REG*******************************/
#define BC_OUT                  0x40
#define ACAL_FAIL               0x20
#define PWR                     0x10
#define MULT                    0x04
#define MTP                     0x02
#define TOUCH                   0x01

/************************SENSINPUTEN REG*******************************/
#define CS3                     0x04
#define CS2                     0x02
#define CS1                     0x01

/************************SENSINCONF1 REG*******************************/

#define DUR560MS
#define DUR840MS
#define DUR1120MS
#define DUR1400MS
#define DUR1680MS
#define DUR2240MS
#define DUR2800MS
#define DUR3360MS
#define DUR3920MS
#define DUR4480MS
#define DUR5600MS
#define DUR6720MS


#define REP_35MS
#define REP_35MS
#define REP_35MS
#define REP_35MS
#define REP_35MS
#define REP_35MS
#define REP_35MS
#define REP_35MS
#define REP_35MS
#define REP_35MS
#define REP_35MS
#define REP_35MS

#define M_PRESS35MS
#define M_PRESS70MS
#define M_PRESS105MS
#define M_PRESS140MS
#define M_PRESS175MS
#define M_PRESS210MS
#define M_PRESS2455MS
#define M_PRESS280MS
#define M_PRESS315MS
#define M_PRESS350MS
#define M_PRESS385MS
#define M_PRESS420MS
#define M_PRESS455MS
#define M_PRESS490MS
#define M_PRESS560MS

/*********************Sensitivity Control Register***************************/

//Delta sense controls the sensitivity of a touch detection for sensor inputs enabled in the Active state
#define MOST_SENSITIVE          0x00
#define DEFAULT                 0x20
#define LEAST_SENSITIVE         0x70

/*********************Configuration Register**********************************/
#define TIMEOUT                 0x80
#define DIS_DIG_NOISE           0x20
#define DIS_ANA_NOISE           0x10
#define MAX_DUR_EN              0x08


/**********************Averaging and Sampling Configuration Register**********/

#define AVG             0x30            //default value 0b01100000      8 samples
#define SAMP_TIME       0x08            //0b00001000    1.28 ms
#define CYCLE_TIME      0x01			// default 70ms 

/************************Multiple Touch Configuration**************************/
#define MULTBLK_EN      0x80


/************************Multiple Touch Pattern Configuration**************************/

#define MTP_EN          0x80
#define MTP_TH1         0x08    
#define MTP_TH0         0x04
#define COMP_PTRN       0x02
#define MTP_ALERT       0x01

/************************Multiple Touch Pattern Register**************************/

#define CS3_PTRN        0x04
#define CS2_PTRN        0x02
#define CS1_PTRN        0x01

/***********************Power Button Configuration****************************/
#define PWR_EN          0x40
#define TIME280ms       0x00
#define TIME560ms       0x01
#define TIME1120ms      0x02
#define TIME2240ms      0x03

#define DEVICE_MODEL		0x6d
#define TOUCH_SAMPLETIME	100	// Touch get pressed button sample time


typedef enum{
ONE=0x00,
TWO=0x01,
THREE=0x02}  touch_type;

typedef enum{
B1=0x01,
B2 = 0x02,
B3= 0x03}  button_type;


typedef enum{
S1,
S2} sensitivity_type;

typedef enum
{
	T35ms = 0,
	T70ms,
	T105ms,
	T140ms,
	max_tm = 255 
}eCYCLETIME;

typedef enum
{
	STM320us = 0,
	ST640us = 1 <<2,
	ST1280us = 2 <<2,
	ST2560us= 3 <<2,
	ST_MAX = 255
}eSMPTIME;

typedef enum
{
	SMPAVG1 = 0,
	SMPAVG2 = 1 << 4,
	SMPAVG4 = 2 << 4,
	SMPAVG8 = 3 << 4,
	SMPAVG16 = 4 << 4,
	SMPAVG32 = 5 << 4,
	SMPAVG64 = 6 << 4,
	SMPAVG128 = 7 << 4,
	SMPAVG_MAX = 255
}eAVERAGE;


void CAP1203_Initialize(void);
void CAP1203_Config(uint8_t  , uint8_t  , uint8_t );
uint8_t CAP1203_ActiveMode(void);
uint8_t CAP1203_StandbyMode(void);
uint8_t CAP1203_DeepSleep(void);
uint8_t CAP1203_ResumeFromDeepSleep(void);

uint8_t CAP1203_ReadPressedButton(void);
void CAP1203_EnableInterrupt(button_type pin);
void CAP1203_SetSensitivity(sensitivity_type sensitivity);
uint8_t CAP1203_CheckSensorStatus(void);
uint8_t CAP1203_ClearInterrupt(void);
uint16_t CAP1203_ReadID(void);

void CAP1203_ConfigureMultiTouch(touch_type number,uint8_t chan);
uint8_t CAP1203_MultitouchEvent(void);

void CAP1203_SetPowerButton(button_type button);
uint8_t CAP1203_ReadPowerButton(void);

uint8_t CAP1203_GetStatusReg(void);
void CAP1203_Write(uint8_t reg, uint8_t data);
uint8_t CAP1203_Read(uint8_t reg);
uint8_t GetPressedButton(void);
void SetServoMovingFlag(uint8_t id);
void SetServoMovingDelay(uint32_t value);
uint8_t GetTouch_runtime(void);

#endif
