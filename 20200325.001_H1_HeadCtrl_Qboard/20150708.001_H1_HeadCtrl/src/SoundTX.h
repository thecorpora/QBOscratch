/*
 * SoundTX.h
 *
 * Created: 14/12/2017 15:14:31
 *  Author: agarcia
 */ 


#ifndef SOUNDTX_H_
#define SOUNDTX_H_

void config_dma_for_TX(void);
int16_t* GetI2S_TX_Buffer(uint32_t);
void SendI2Sdata(int32_t mic_idx);

#endif /* SOUNDTX_H_ */