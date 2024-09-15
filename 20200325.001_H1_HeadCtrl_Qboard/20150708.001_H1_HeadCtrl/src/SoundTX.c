/*
 * SoundTX.c
 *
 * Created: 14/12/2017 15:17:23
 *  Author: agarcia
 */ 

#include <string.h>
#include "asf.h"
#include "user.h"
#include "Analog.h"
#include "SoundTX.h"


struct dma_resource i2s_tx_dma_resource;
int16_t i2s_tx[2][2 * BUFFER_LENGTH];

uint32_t tx_destDescriptor[2];

uint32_t txBuf_idx = 0;

COMPILER_ALIGNED(16)
DmacDescriptor i2s_tx_dma_descriptor SECTION_DMAC_DESCRIPTOR;

bool  dma_i2s_tx_done = false, ADC_dma_overrun = false;

int16_t* GetI2S_TX_Buffer(uint32_t idx)
{
	 return (int16_t *)&i2s_tx[idx][0];
}

static void i2s_txDMAerror(struct dma_resource* const resource)
{
	dma_resume_job(resource);
}

static void i2s_TX_Done(struct dma_resource* const resource )
{
	resource->descriptor->SRCADDR.reg = (uint32_t)tx_destDescriptor[txBuf_idx++];
	txBuf_idx &= 0x01;
	dma_start_transfer_job(&i2s_tx_dma_resource);
	ADC_dma_overrun = dma_i2s_tx_done = false;
	dma_i2s_tx_done = 1;
}void SendI2Sdata(int32_t idx)
{
	dma_i2s_tx_done = false;
	txBuf_idx = idx;
}

void config_dma_for_TX(void)
{
	struct dma_resource_config config;
	struct dma_descriptor_config descriptor_config;
	
	dma_get_config_defaults(&config);
	config.trigger_action = DMA_TRIGGER_ACTION_BEAT;
	config.peripheral_trigger = I2S_DMAC_ID_TX_1;
	config.priority = DMA_PRIORITY_LEVEL_1;
	
	dma_allocate(&i2s_tx_dma_resource, &config);
	
	tx_destDescriptor[0] =  (uint32_t)&i2s_tx[0][0] + sizeof(i2s_tx) / 2;
	tx_destDescriptor[1] =  (uint32_t)&i2s_tx[1][0] + sizeof(i2s_tx) / 2;

	dma_descriptor_get_config_defaults(&descriptor_config);
	descriptor_config.block_transfer_count = sizeof(i2s_tx)/4;
	descriptor_config.beat_size = DMA_BEAT_SIZE_HWORD;
	descriptor_config.src_increment_enable = true;
	descriptor_config.dst_increment_enable = false;
	descriptor_config.source_address = tx_destDescriptor[0];
	descriptor_config.destination_address = (uint32_t)&I2S->DATA[1];
	descriptor_config.next_descriptor_address = 0;
	dma_descriptor_create(&i2s_tx_dma_descriptor, &descriptor_config);
	dma_add_descriptor(&i2s_tx_dma_resource, &i2s_tx_dma_descriptor);

	dma_register_callback(&i2s_tx_dma_resource, i2s_TX_Done,DMA_CALLBACK_TRANSFER_DONE);
	dma_enable_callback(&i2s_tx_dma_resource, DMA_CALLBACK_TRANSFER_DONE);
	dma_register_callback(&i2s_tx_dma_resource, i2s_txDMAerror,DMA_CALLBACK_TRANSFER_ERROR);
	dma_enable_callback(&i2s_tx_dma_resource, DMA_CALLBACK_TRANSFER_ERROR);
	dma_start_transfer_job(&i2s_tx_dma_resource);
}
