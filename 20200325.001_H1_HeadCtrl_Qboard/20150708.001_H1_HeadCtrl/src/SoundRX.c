/*
 * SoundRX.c
 *
 * Created: 15/12/2017 11:54:50
 *  Author: agarcia
 */ 

 #include <string.h>
 #include "asf.h"
 #include "user.h"
 #include "Analog.h"
 #include "SoundRX.h"
 #include "Mouth.h"

  int16_t i2s_RXBuffer[2][BUFFER_LENGTH];
  uint32_t DAC_dma_done = true;
  uint16_t DAC_buffer[2][BUFFER_LENGTH/2];
  struct tc_module tc_dac_instance;
  struct tc_module tc_timing_instance;

  struct dma_resource i2s_rx_dma_resource, DAC_dma_resource;
  uint32_t rx_DacDescriptor[2];
  uint32_t rx_I2SDescriptor[2];
  uint32_t rxBuf_idx = 0, l_rxBuf_idx = 0;
  uint32_t dma_tm = 0;
  COMPILER_ALIGNED(16)
  DmacDescriptor i2s_rx_dma_descriptor,DAC_dma_descriptor SECTION_DMAC_DESCRIPTOR;

 static void ConvertToDAC(uint32_t src, uint32_t dst, uint32_t length) {	 int16_t *pSrc = (int16_t *)src;	 uint16_t *pDst = (uint16_t *)dst;	 int16_t m_threshold = 3276, m_trigger = 0;	 while(length-- > 0)	 {		 if(*pSrc > m_threshold || *pDst < -m_threshold)		 m_trigger++;		 *pDst = (uint16_t)(*pSrc + 32768);		 pDst++;		 pSrc +=2;	 }	 if(m_trigger > 9)		fTrig_Animation(); }
 static void i2s_RX_Done(struct dma_resource* const resource )
 {
	// I2S clock time meassurement 
	
	dma_tm = TC5->COUNT16.COUNT.reg / (BUFFER_LENGTH / 32);
	TC5->COUNT16.COUNT.reg = 0;

	l_rxBuf_idx = rxBuf_idx & 0x01;

	resource->descriptor->DSTADDR.reg = rx_I2SDescriptor[++rxBuf_idx & 0x01];

	dma_start_transfer_job(&i2s_rx_dma_resource);

	ConvertToDAC((uint32_t)(void *)&i2s_RXBuffer[l_rxBuf_idx][0], (uint32_t)(void *)&DAC_buffer[l_rxBuf_idx][0],sizeof(DAC_buffer) / 4);

	// IF DAC DMA transfer is complete set source pointer to present buffer and start
	 
	if(dma_tm > 2849 && dma_tm < 3150)
		TC4->COUNT16.CC[0].reg = dma_tm;

	if(DAC_dma_done)
	{
		DAC_dma_resource.descriptor->SRCADDR.reg = rx_DacDescriptor[l_rxBuf_idx];
		dma_start_transfer_job(&DAC_dma_resource);
		// TC4->COUNT16.CC[0].reg++;
		DAC_dma_done = 0;
	}
	else
	{
		l_rxBuf_idx |= 0x80;
		// TC4->COUNT16.CC[0].reg--;
	}

 }
void i2s_callback_RX(struct i2s_module *const module)
{
	  
	if(I2S->INTFLAG.bit.RXOR0)
	{
		//  Code to check coherence between dma_resource & status of hardware registers
		// Sometimes dma complete interrupt was not fired

		I2S->INTFLAG.bit.RXOR0 = 1;

		if(DAC_dma_done && i2s_rx_dma_resource.job_status != STATUS_OK)
		{
			DMAC->CHINTENSET.reg = i2s_rx_dma_resource.channel_id;
			if(!DMAC->CHSTATUS.bit.PEND)
			DMAC->SWTRIGCTRL.reg = 1 << i2s_rx_dma_resource.channel_id;
		}
	}
}
 void config_dma_for_RX(void)
 {
	struct dma_resource_config config;
	struct dma_descriptor_config descriptor_config;

	dma_get_config_defaults(&config);
	config.trigger_action = DMA_TRIGGER_ACTION_BEAT;
	config.peripheral_trigger = I2S_DMAC_ID_RX_0;
	config.priority = DMA_PRIORITY_LEVEL_0;
	dma_allocate(&i2s_rx_dma_resource, &config);

	rx_I2SDescriptor[0] = (uint32_t)&i2s_RXBuffer[0][0] + sizeof(i2s_RXBuffer)/2;
	rx_I2SDescriptor[1] = (uint32_t)&i2s_RXBuffer[1][0] + sizeof(i2s_RXBuffer)/2;

	dma_descriptor_get_config_defaults(&descriptor_config);
	descriptor_config.block_transfer_count = sizeof(i2s_RXBuffer)/4;
	descriptor_config.beat_size = DMA_BEAT_SIZE_HWORD;
	descriptor_config.src_increment_enable = false;
	descriptor_config.dst_increment_enable = true;
	descriptor_config.source_address = (uint32_t)&I2S->DATA[0];
	descriptor_config.destination_address = rx_I2SDescriptor[0];
	descriptor_config.next_descriptor_address = 0;

	dma_descriptor_create(&i2s_rx_dma_descriptor, &descriptor_config);
	dma_add_descriptor(&i2s_rx_dma_resource, &i2s_rx_dma_descriptor);

	dma_register_callback(&i2s_rx_dma_resource, i2s_RX_Done, DMA_CALLBACK_TRANSFER_DONE);
	dma_enable_callback(&i2s_rx_dma_resource, DMA_CALLBACK_TRANSFER_DONE);	
	/* Enable I2S DMAC channel upon DMAC DAC channel had been configured */
	dma_start_transfer_job(&i2s_rx_dma_resource); }


/**************************************************************************/
/*		DMA & DAC configuration							*/
/**************************************************************************/
  void configure_DAC_trigger_dma(void)
{
	struct tc_config config_tc;

	tc_get_config_defaults(&config_tc);
	config_tc.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;
	config_tc.counter_16_bit.compare_capture_channel[0] = 2999;	// 16000 Hz -> 48e6 /16000 = 3000, real 2875 for interrupt delays
	tc_init(&tc_dac_instance, TC4 , &config_tc);
	tc_enable(&tc_dac_instance);
}

/*		Timing counter TC5 is used to synchronize i2s RX data clock with DAC clock
		TC5 clock = sysclk / 16,  48MHz /16 = 3MHz  */

static void InitTimingCounter(void)
{
	struct tc_config config_tc;

	tc_get_config_defaults(&config_tc);
	config_tc.wave_generation = TC_WAVE_GENERATION_NORMAL_FREQ;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV16;
	tc_init(&tc_timing_instance, TC5, &config_tc);
	tc_enable(&tc_timing_instance);
} 

static void DAC_transfer_done(struct dma_resource* const resource )
 {

	if(l_rxBuf_idx & 0x80) // Test if buffer is updated with new data
	{
		resource->descriptor->SRCADDR.reg = rx_DacDescriptor[l_rxBuf_idx & 0x01];
		dma_start_transfer_job(&DAC_dma_resource);
	}
	else 
		DAC_dma_done = true;

	 l_rxBuf_idx &= 0x01;
 }
 
 void config_dma_for_DAC(void)
 {
	struct dma_resource_config config;
	struct dma_descriptor_config descriptor_config;
	dma_get_config_defaults(&config);
	config.trigger_action = DMA_TRIGGER_ACTION_BEAT;
	config.peripheral_trigger = TC4_DMAC_ID_OVF;
	config.priority = DMA_PRIORITY_LEVEL_1;
	dma_allocate(&DAC_dma_resource, &config);
	 
	rx_DacDescriptor[0] = (uint32_t)&DAC_buffer[0][0] + sizeof(DAC_buffer)/2;
	rx_DacDescriptor[1] = (uint32_t)&DAC_buffer[1][0] + sizeof(DAC_buffer)/2;

	dma_descriptor_get_config_defaults(&descriptor_config);
	descriptor_config.block_transfer_count = sizeof(DAC_buffer)/4;
	descriptor_config.beat_size = DMA_BEAT_SIZE_HWORD;
	descriptor_config.src_increment_enable = true;
	descriptor_config.dst_increment_enable = false;
	descriptor_config.source_address = rx_DacDescriptor[0];
	descriptor_config.destination_address = (uint32_t)&DAC->DATA.reg;
	descriptor_config.next_descriptor_address = 0;

	dma_descriptor_create(&DAC_dma_descriptor, &descriptor_config);
	dma_add_descriptor(&DAC_dma_resource, &DAC_dma_descriptor);

	dma_register_callback(&DAC_dma_resource, DAC_transfer_done, DMA_CALLBACK_TRANSFER_DONE);
	dma_enable_callback(&DAC_dma_resource, DMA_CALLBACK_TRANSFER_DONE);
	dma_start_transfer_job(&DAC_dma_resource);	InitTimingCounter();
 }