/*
 * IncFile1.h
 *
 * Created: 25/02/2016 8:33:56
 *  Author: agarcia
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_

#define LEDS_NUMBER 1

typedef struct
{
	uint32_t Counter:8;
	uint32_t nIter:8;
	uint32_t LedPin:8;
	uint32_t:3;
	uint32_t LedState:1;
	uint32_t LedOut:1;
	uint32_t Loop:1;
	uint32_t InvertOut:1;
	uint32_t Enable:1;
	
	uint16_t On_Time;	// On time in ms
	uint16_t Off_Time;	// Off time in ms
	uint32_t StartTime;
}stLedConfig;

void fLedConfigDefault(stLedConfig *);
void fSetLedPeriod(stLedConfig *, uint32_t, uint32_t);
void fStartLedFlash(stLedConfig *, uint32_t);
void fStopLedFlash(stLedConfig *);
stLedConfig* fGetLed(uint32_t);
void fProcessLed(stLedConfig *);
void LedRunTime(void);
void InitLedFlash(void);

#endif /* INCFILE1_H_ */