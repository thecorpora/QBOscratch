/*
 * SoundRX.h
 *
 * Created: 15/12/2017 11:54:31
 *  Author: agarcia
 */ 


#ifndef SOUNDRX_H_
#define SOUNDRX_H_

void config_dma_for_RX(void);
void i2s_callback_RX(struct i2s_module *const );
void configure_DAC_trigger_dma(void);
void config_dma_for_DAC(void);
#endif /* SOUNDRX_H_ */