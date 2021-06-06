/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <fastmath.h>

#include "user.h"
#include "Analog.h"
#include "XL320.h"
#include "CAP1203.h"
#include "Mouth.h"
#include "serialProtocol.h"
#include "SoundTX.h"
#include "SoundRX.h"
#include "LedFlash.h"

int main (void)
{
	volatile uint32_t init_delay_tm = 0;

	system_init();
	
	cpu_irq_enable();
	
	sleepmgr_init();
	udc_start();

	SystemRTC_Init();

	configure_usart_Servo();
	configure_usart_Serial();
	configure_i2c_master();
		configure_adc_trigger_event();
	configure_adc(GetI2S_TX_Buffer(0));
	configure_dac();
	configure_dac_channel();

	config_dma_for_DAC();
	configure_DAC_trigger_dma();	configure_ADC_dma();	configure_ADC_trigger();

	config_dma_for_RX();
	config_dma_for_TX();

	configure_i2s();
	CAP1203_Initialize();
	fInit_Mouth();

	ioport_set_pin_level(SERVO_EN, 1);

	configure_ac();
	
	ioport_set_pin_level(SPK_EN, true);
	
	init_delay_tm = GetTickCount();

	while(TimeDiff(init_delay_tm, GetTickCount()) < 99);
		
	XL320_Init(GetServoInstance());

	InitLedFlash();

	while(1)
	{
		AnalogRuntime();
		fSerial_processSerial();	// Command processor periodic task
		LedRunTime();
		GetTouch_runtime();
		USBFordwarding_runtime();
		sleepmgr_enter_sleep();
	};	
	
}
