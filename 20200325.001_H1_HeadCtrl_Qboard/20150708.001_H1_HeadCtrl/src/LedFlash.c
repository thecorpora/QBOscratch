/*
 * LedFlash.c
 *
 * Created: 25/02/2016 8:34:31
 *  Author: agarcia
 */ 
#include <asf.h>
#include "user.h"
#include "LedFlash.h"

stLedConfig stLED[LEDS_NUMBER];

void fLedConfigDefault(stLedConfig *stLed)
{
	// Led flashing default configuring
	
	stLed->Counter = 1;
	stLed->InvertOut = 0;
	stLed->LedPin = 0xff;
	stLed->LedState = 0;
	stLed->Loop = 0;
	stLed->Off_Time = 200;
	stLed->On_Time = 20;
	stLed->StartTime = GetTickCount();
}

// Set flashing period (frequency) and duty (0 to period)

void fSetLedPeriod(stLedConfig *stLed, uint32_t period, uint32_t duty)
{
	stLed->On_Time = duty;
	stLed->Off_Time = period-duty;
}

void fStartLedFlash(stLedConfig *stLed, uint32_t nTimes)
{
	stLed->StartTime = GetTickCount();
	stLed->LedState = 1;
	if(nTimes > 0xff)stLed->Loop = 1;
	else stLed->Counter = nTimes;
}

void fStopLedFlash(stLedConfig *stLed)
{
	stLed->LedState = 0;
	stLed->Loop = 0;
	stLed->Counter =1;
}

stLedConfig* fGetLed(uint32_t nLed)
{
	if(nLed < LEDS_NUMBER)
	{
		return &stLED[nLed];
	}
	else return NULL;
}

void fProcessLed(stLedConfig *stLed)
{
	uint32_t PresentTm , EllapsedTm;
	if(!stLed->Enable)return;
	if(stLed->Counter || stLed->Loop)
	{
		PresentTm = GetTickCount();
		EllapsedTm = TimeDiff(stLed->StartTime, PresentTm);
		
		if(stLed->LedState)
		{
			if(EllapsedTm > stLed->On_Time)
			{
				
				stLed->StartTime = PresentTm;
				stLed->LedState = 0;
			}
			else if(!stLed->LedOut)
			{
				stLed->LedOut = 1;
				port_pin_set_output_level((uint32_t)stLed->LedPin, !stLed->InvertOut);
			}
		}
		else
		{
			if(EllapsedTm > stLed->Off_Time)
			{
				if(stLed->Counter)stLed->Counter--;
				stLed->StartTime = PresentTm;
				if(stLed->Counter || stLed->Loop)stLed->LedState = 1;
			}
			else if(stLed->LedOut)
			{
				port_pin_set_output_level((uint32_t)stLed->LedPin, stLed->InvertOut);
				stLed->LedOut = 0;
			}
		}
		
	}
}

void LedRunTime(void)
{
	for(uint32_t x = 0; x < LEDS_NUMBER; x++)
		fProcessLed(&stLED[x]);
}

void InitLedFlash(void)
{
	fLedConfigDefault(stLED);
	stLED[0].LedPin = (uint8_t)LED_RX;
	stLED[0].InvertOut = 1;
	stLED[0].Loop = 1;
	stLED[0].Enable = 1;

	fSetLedPeriod(&stLED[0], 990, 10);
	fStartLedFlash(&stLED[0], 1);
}

