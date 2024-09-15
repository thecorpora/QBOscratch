/**
 * \file
 *
 * \brief User board configuration template
 *
 * Copyright (C) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

	#define AMP_IN1			PIN_PA02

	#define SPK_EN			PIN_PA27

	#define AMP_REF			PIN_PA03
	
	#define MIC_1			PIN_PB08
	#define MIC_2			PIN_PB09
	#define MIC_3			PIN_PA03

	#define I_SERVO			PIN_PA05
	#define PIN_I_SERVO		PIN_PA05B_AC_AIN1
	#define I_SERVO_MUX		MUX_PA05B_AC_AIN1;

	#define SERVO_EN		PIN_PA06

	#define I2S_DOUT		PIN_PA08
	#define I2S_DIN			PIN_PA07
	#define I2S_SCK			PIN_PA10
	#define I2S_LRCK		PIN_PA11

	#define I2S_DIN_MUX		MUX_PA07G_I2S_SD0
	#define I2S_DOUT_MUX	MUX_PA08G_I2S_SD1
	#define I2S_SCK_MUX		MUX_PA10G_I2S_SCK0
	#define I2S_LRCK_MUX	MUX_PA11G_I2S_FS0

	#define G_COLUMN_0		PIN_PA12
	#define G_COLUMN_1		PIN_PA13
	#define G_COLUMN_2		PIN_PA14
	#define G_COLUMN_3		PIN_PA15
	#define G_COLUMN_4		PIN_PA16

	#define G_ROW_0			PIN_PA17
	#define G_ROW_1			PIN_PA18
	#define G_ROW_2			PIN_PA19
	#define G_ROW_3			PIN_PA20

	#define G_NOSE_G		PIN_PA21
	#define G_NOSE_B		PIN_PB02
	#define G_NOSE_R		PIN_PB11

	#define USB_DET			PIN_PA09
//	#define USB_DM			PIN_PA24
//	#define USB_DM			PIN_PA25
	#define USB_ID			PIN_PA28

	#define UART0_RXTX		PIN_PB10

	#define UART1_TX		PIN_PB22
	#define UART1_RX		PIN_PB23

	// #define LED_TX			PIN_PA27
	#define LED_RX			PIN_PB03

#endif // CONF_BOARD_H
