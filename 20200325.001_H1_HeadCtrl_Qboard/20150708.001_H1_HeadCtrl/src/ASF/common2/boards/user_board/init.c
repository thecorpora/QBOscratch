/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

#if defined(__GNUC__)
void board_init(void) WEAK __attribute__((alias("system_board_init")));
#elif defined(__ICCARM__)
void board_init(void);
#  pragma weak board_init=system_board_init
#endif

void system_board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	 	    struct system_pinmux_config config_pinmux;
	 	    
	 	    // PIN configuration
	 	    
	 	    system_pinmux_get_config_defaults(&config_pinmux);
	 	    config_pinmux.mux_position = SYSTEM_PINMUX_GPIO;
	 	    config_pinmux.direction = SYSTEM_PINMUX_PIN_DIR_INPUT;
	 	    config_pinmux.input_pull = SYSTEM_PINMUX_PIN_PULL_NONE;
	 	    
	 	    system_pinmux_pin_set_config(USB_DET, &config_pinmux);
	 	    system_pinmux_pin_set_config(USB_ID, &config_pinmux);

			config_pinmux.input_pull = SYSTEM_PINMUX_PIN_PULL_UP;
			system_pinmux_pin_set_config(UART0_RXTX, &config_pinmux);
	 	    
	 	    config_pinmux.mux_position = 0x01;		//  function B for analog input
	 	    config_pinmux.input_pull = SYSTEM_PINMUX_PIN_PULL_NONE;
	 	    system_pinmux_pin_set_config(AMP_IN1, &config_pinmux);
	 	    system_pinmux_pin_set_config(AMP_REF, &config_pinmux);

	 	    system_pinmux_pin_set_config(MIC_1, &config_pinmux);
	 	    system_pinmux_pin_set_config(MIC_2, &config_pinmux);
	 	    system_pinmux_pin_set_config(MIC_3, &config_pinmux);
	 	    system_pinmux_pin_set_config(I_SERVO, &config_pinmux);
	 	    
	 	    config_pinmux.mux_position = SYSTEM_PINMUX_GPIO;
	 	    config_pinmux.direction    = SYSTEM_PINMUX_PIN_DIR_OUTPUT;

	 	    system_pinmux_pin_set_config(SPK_EN, &config_pinmux);

	 	    system_pinmux_pin_set_config(SERVO_EN, &config_pinmux);

	 	    system_pinmux_pin_set_config(G_COLUMN_0, &config_pinmux);
	 	    system_pinmux_pin_set_config(G_COLUMN_1, &config_pinmux);
	 	    system_pinmux_pin_set_config(G_COLUMN_2, &config_pinmux);
	 	    system_pinmux_pin_set_config(G_COLUMN_3, &config_pinmux);
	 	    system_pinmux_pin_set_config(G_COLUMN_4, &config_pinmux);
	 	    
	 	    system_pinmux_pin_set_config(G_ROW_0, &config_pinmux);
	 	    system_pinmux_pin_set_config(G_ROW_1, &config_pinmux);
	 	    system_pinmux_pin_set_config(G_ROW_2, &config_pinmux);
	 	    system_pinmux_pin_set_config(G_ROW_3, &config_pinmux);

			system_pinmux_pin_set_config(G_NOSE_B, &config_pinmux);
			system_pinmux_pin_set_config(G_NOSE_G, &config_pinmux);
			system_pinmux_pin_set_config(G_NOSE_R, &config_pinmux);


	 	    system_pinmux_pin_set_config(LED_RX, &config_pinmux);
	 	    //system_pinmux_pin_set_config(LED_TX, &config_pinmux);
 	    
	 	    ioport_set_pin_level(LED_RX, true);
	 	    // ioport_set_pin_level(LED_TX, true);
}