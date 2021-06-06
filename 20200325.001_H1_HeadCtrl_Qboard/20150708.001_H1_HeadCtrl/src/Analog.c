/*
 * Analog.c
 *
 * Created: 21/09/2017 16:05:02
 *  Author: agarcia
 */ 

 #include <asf.h>
 #include <fastmath.h>
 #include <string.h>
 #include "user.h"
 #include "Analog.h"
 #include "SoundTX.h" 

 const int16_t sinewave[] = {
	0,804,1608,2410,3212,4011,4808,5602,6393,7179,7962,8739,9512,10278,11039,11793,12539,13279,14010,
	14732,15446,16151,16846,17530,18204,18868,19519,20159,20787,21403,22005,22594,23170,23731,24279,24811,25329,25832,26319,
	26790,27245,27683,28105,28510,28898,29268,29621,29956,30273,30571,30852,31113,31356,31580,31785,31971,32137,32285,32412,
	32521,32609,32678,32728,32757,32767,32757,32728,32678,32609,32521,32412,32285,32137,31971,31785,31580,31356,31113,30852,
	30571,30273,29956,29621,29268,28898,28510,28105,27683,27245,26790,26319,25832,25329,24811,24279,23731,23170,22594,22005,
	21403,20787,20159,19519,18868,18204,17530,16846,16151,15446,14732,14010,13279,12539,11793,11039,10278,9512,8739,7962,7179,
	6393,5602,4808,4011,3212,2410,1608,804,0,-804,-1608,-2410,-3212,-4011,-4808,-5602,-6393,-7179,-7962,-8739,-9512,-10278,
	-11039,-11793,-12539,-13279,-14010,-14732,-15446,-16151,-16846,-17530,-18204,-18868,-19519,-20159,-20787,-21403,-22005,
	-22594,-23170,-23731,-24279,-24811,-25329,-25832,-26319,-26790,-27245,-27683,-28105,-28510,-28898,-29268,-29621,-29956,
	-30273,-30571,-30852,-31113,-31356,-31580,-31785,-31971,-32137,-32285,-32412,-32521,-32609,-32678,-32728,-32757,-32767,
	-32757,-32728,-32678,-32609,-32521,-32412,-32285,-32137,-31971,-31785,-31580,-31356,-31113,-30852,-30571,-30273,-29956,
	-29621,-29268,-28898,-28510,-28105,-27683,-27245,-26790,-26319,-25832,-25329,-24811,-24279,-23731,-23170,-22594,-22005,
	-21403,-20787,-20159,-19519,-18868,-18204,-17530,-16846,-16151,-15446,-14732,-14010,-13279,-12539,-11793,-11039,-10278,
	-9512,-8739,-7962,-7179,-6393,-5602,-4808,-4011,-3212,-2410,-1608,-804}; 

volatile uint32_t buffer_idx = 0, l_buffer_idx = 0, transfer_is_done = 0, servo_overload = 0, bCompute_offset = 0;;
volatile uint32_t  mic_idx = 0, servo_recon_tm = 0, ovl_recovery_tm = 999, adc_dma_timeout = 0, adc_dma_tm = 0;
float RMS_data[NSCAN_CHN];
volatile int16_t ADC_Offset[NSCAN_CHN];
int16_t Cfg_offset[3] = {-5235, -5235, -5235};	// Mean values must adjust for every OPAM


COMPILER_ALIGNED(16) 
static uint16_t adc_buffer[2][NSCAN_CHN * BUFFER_LENGTH];
struct tc_module tc_instance;
struct adc_module adc_instance;
struct dac_module dac_instance;

struct dma_resource adc_dma_resource;
COMPILER_ALIGNED(16) 
DmacDescriptor adc_dma_descriptor SECTION_DMAC_DESCRIPTOR;

struct events_resource config_tc_event_res;

/*******************************************************************
	Method      :  Compute_ADC_RMS 

	Description :
         This function computes  RMS value for NSCAN_CHN from ADC sample buffer.

	Inputs:
		*data	:		pointer to sampled data
		nSamples:		number of samples by channel	
	Output:
		RMS_out:		float array with RMS value of each channel	
**********************************************************************/

uint32_t Compute_ADC_RMS(int16_t *data, uint16_t nSamples, float *RMS_out)
{
	uint64_t adder[NSCAN_CHN];
	int16_t *pData = data;
	uint32_t x = 0, nItems = nSamples;

	
	if(data == NULL || RMS_out == NULL)
		return STATUS_NO_CHANGE;

	for(x = 0; x <NSCAN_CHN ; x++) 
		adder[x] = 0;
		
	while(nItems--)
	{
		for(x = 0; x<NSCAN_CHN; x++)
		{
			adder[x] +=(uint64_t)((*pData) * (*pData));
			pData++;
		}
	}
	
	for(x = 0; x<NSCAN_CHN; x++)
	{
		adder[x] /= nSamples;
		RMS_out[x] = (7 * RMS_out[x] + sqrtf((float)adder[x]) / 32767.0)/8;	// Divide 32767 results in 1V max RMS
		
	}
	return STATUS_OK;
}

uint32_t Compute_ADC_Offset(void)
{
	int32_t adder[NSCAN_CHN];
	int16_t *pData = fGetADCtransfertBUffer();
	uint32_t x = 0, nItems = BUFFER_LENGTH;
	if(pData == NULL)
		return STATUS_BUSY;
	for(x = 0; x <NSCAN_CHN ; x++)
		adder[x] = 0;
	
	while(nItems--)
		for(x = 0; x<NSCAN_CHN; x++)
			adder[x] +=(int32_t)*(pData++);
	
	for(x = 0; x<NSCAN_CHN; x++)
		ADC_Offset[x] =(int16_t)((15 * ADC_Offset[x] + (adder[x] / BUFFER_LENGTH)) / 16);	// Result is in ADC counts

	return STATUS_OK;
}

int16_t *fGetADCtransfertBUffer(void)
{
	if(transfer_is_done)
	{
		transfer_is_done = 0;
		return (int16_t *)&adc_buffer[l_buffer_idx][0];
	}	
	return NULL;
}

uint16_t *GetADCBuffer(void)
{
	return (uint16_t *)&adc_buffer;
}

struct adc_module *GetADC_instance(void)
{
	return &adc_instance;
}

struct dac_module *GetDAC_instance(void)
{
	return &dac_instance;
}

float *GetRMSdata(void)
{
	return RMS_data;
}

uint32_t SetMicIndex(uint32_t index)
{
	if(index < 3)
		mic_idx = index;
	return mic_idx;
}


/*******************************************************************
	Method      :  configure_ADC_trigger 

	Description :
		Setup TC counter for precise start ADC trigger 
	Inputs:
		none.
	Output:
		none.
**********************************************************************/

void configure_ADC_trigger(void)
{
	struct tc_config config_tc;
	struct tc_events config_events;
	float periode = (1.25 * NSCAN_CHN * BUFFER_LENGTH) / 48000;	// Timeout set to 25% of total 3 channel filling dma periode 

	tc_get_config_defaults(&config_tc);
	config_tc.counter_size = TC_COUNTER_SIZE_16BIT;
	config_tc.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;
	config_tc.clock_source = GCLK_GENERATOR_0;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;
	config_tc.counter_16_bit.compare_capture_channel[0] = 999;	 // 3 Channel * 16000 Hz = 48000Hz -> 48e6 / 48e3  = 1000
	// config_events.generate_event_on_compare_channel[0] = true;
	config_events.generate_event_on_overflow = true;
	tc_init(&tc_instance, TC3 , &config_tc);
	tc_enable_events(&tc_instance, &config_events);
	tc_enable(&tc_instance);
	adc_dma_timeout =(uint32_t)(((float)config_tc.counter_16_bit.compare_capture_channel[0]) * periode); 
	adc_dma_tm = GetTickCount();
}

/* ADC DMA channel configuration */

void configure_ADC_dma_resource(struct dma_resource *resource)
{
    struct dma_resource_config dma_res_config;
    dma_get_config_defaults(&dma_res_config);
    dma_res_config.peripheral_trigger = ADC_DMAC_ID_RESRDY;
    dma_res_config.trigger_action = DMA_TRIGGER_ACTION_BEAT;
	dma_res_config.priority = DMA_PRIORITY_LEVEL_0;

    dma_allocate(resource, &dma_res_config);
}

void setup_ADC_DMA_transfer_descriptor(DmacDescriptor *descriptor)
{
    struct dma_descriptor_config descriptor_config;
    dma_descriptor_get_config_defaults(&descriptor_config);
    descriptor_config.beat_size = DMA_BEAT_SIZE_HWORD;
    descriptor_config.dst_increment_enable = true;
    descriptor_config.src_increment_enable = false;
    descriptor_config.block_transfer_count = sizeof(adc_buffer)/4;
    descriptor_config.source_address = (uint32_t)(&adc_instance.hw->RESULT.reg);
    descriptor_config.destination_address = (uint32_t)&adc_buffer[0][0] + sizeof(adc_buffer)/2;
	descriptor_config.next_descriptor_address = 0;

    dma_descriptor_create(descriptor, &descriptor_config);
}

static void ADC_ProcessToSend(uint32_t src, uint32_t dst, uint32_t length)
{
	int16_t *pDst = (void *)dst;
	int16_t *pSrc = (void*)src;
	int16_t value;

	while(length-- > 0)
	{
		value =  *(pSrc + mic_idx) - Cfg_offset[mic_idx];
		*(pDst++) = value;
		*(pDst++) = value;
		pSrc += NSCAN_CHN;
	}
}

void dma_ADC_tranfer_done(struct dma_resource* const resource )
{
	l_buffer_idx = buffer_idx & 0x01;
	resource->descriptor->DSTADDR.reg = (uint32_t)&adc_buffer[buffer_idx++][0] + sizeof(adc_buffer)/2;
	buffer_idx &= 0x1;
	if(ADC->INTFLAG.bit.OVERRUN)
	{
		if (!(ADC->STATUS.reg & ADC_STATUS_SYNCBUSY)) 
		{
			ADC->INPUTCTRL.bit.INPUTOFFSET = 0;

			ADC->INTFLAG.reg |= ADC_INTFLAG_OVERRUN;
			ADC->SWTRIG.bit.FLUSH = 1;
		}
	}
	if(dma_start_transfer_job(&adc_dma_resource) == STATUS_BUSY)	{		dma_abort_job(&adc_dma_resource);		dma_start_transfer_job(&adc_dma_resource);		}
	adc_dma_tm = GetTickCount();
	ADC_ProcessToSend((uint32_t)&adc_buffer[l_buffer_idx][0], (uint32_t)(void *)GetI2S_TX_Buffer(l_buffer_idx), BUFFER_LENGTH);	SendI2Sdata(l_buffer_idx);
	transfer_is_done = true;
}
 
void configure_ADC_dma(void)
{
	configure_ADC_dma_resource(&adc_dma_resource);
	setup_ADC_DMA_transfer_descriptor(&adc_dma_descriptor);
	dma_add_descriptor(&adc_dma_resource, &adc_dma_descriptor);
    dma_register_callback(&adc_dma_resource, dma_ADC_tranfer_done, DMA_CALLBACK_TRANSFER_DONE);
    dma_enable_callback(&adc_dma_resource, DMA_CALLBACK_TRANSFER_DONE);
	dma_start_transfer_job(&adc_dma_resource);	adc_dma_timeout = GetTickCount();}

/****************************************/
/*		ADC configuration		*/
/****************************************/

void configure_adc(int16_t *tx_buffer)
{
	struct adc_config config_adc;
	//struct adc_events config_events;
	
	struct stDID
	{
		uint32_t DEVSEL :8;
		uint32_t REVISION :4;
		uint32_t DIE :4;
		uint32_t SERIES :6;
		uint32_t :1;
		uint32_t FAMILY :5;
		uint32_t PROCESSOR : 4;
	};
	
	// const struct stDID *pDID = (uint32_t *)0x41002018U;
	const struct stDID *pDID = (struct stDID *)0x41002018U;
	
	adc_get_config_defaults(&config_adc);

	config_adc.clock_source = GCLK_GENERATOR_3;
	config_adc.reference = ADC_REFCTRL_REFSEL_INT1V;
	config_adc.differential_mode = true;
	config_adc.gain_factor     = ADC_INPUTCTRL_GAIN_1X;
	config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV4;
	config_adc.sample_length = 2;
	config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN2;
	config_adc.negative_input = ADC_NEGATIVE_INPUT_PIN1;
	config_adc.resolution = ADC_RESOLUTION_12BIT;
	config_adc.accumulate_samples = ADC_ACCUMULATE_SAMPLES_4;
	config_adc.divide_result = ADC_DIVIDE_RESULT_4;
	config_adc.pin_scan.inputs_to_scan = 3;
	config_adc.pin_scan.offset_start_scan = 0;
	config_adc.left_adjust = true;
	config_adc.event_action = ADC_EVENT_ACTION_START_CONV;
	adc_init(&adc_instance, ADC, &config_adc);
	
	ADC->AVGCTRL.bit.ADJRES = 0x02;
	ADC->AVGCTRL.bit.SAMPLENUM = 0x02;

	/*  ASF library errata don´t read DID properly */
	
	if(pDID->REVISION > 3)
	adc_instance.hw->AVGCTRL.bit.ADJRES = 0;

	adc_enable(&adc_instance);

	memset((void *)ADC_Offset,0,sizeof(ADC_Offset));
	memset(RMS_data,0,sizeof(RMS_data));
}

/* Events from evsys configuration */

void configure_adc_trigger_event()
{
	struct events_config config;
	events_get_config_defaults(&config);
	config.generator      = EVSYS_ID_GEN_TC3_MCX_0;
	config.edge_detect    = EVENTS_EDGE_DETECT_NONE;
	config.path           = EVENTS_PATH_ASYNCHRONOUS;
	config.clock_source   = GCLK_GENERATOR_0;
	events_allocate(&config_tc_event_res, &config);
	events_attach_user(&config_tc_event_res, EVSYS_ID_USER_ADC_START);
}

/*****************************/
/*	DAC configuration	*/
/*****************************/

void configure_dac(void)
{
	struct dac_config config_dac;
	dac_get_config_defaults(&config_dac);
	config_dac.clock_source = GCLK_GENERATOR_3;
	config_dac.reference = DAC_REFERENCE_AVCC;
	config_dac.output = DAC_OUTPUT_EXTERNAL;
	config_dac.databuf_protection_bypass = true;
	config_dac.left_adjust = true;
	dac_init(&dac_instance, DAC, &config_dac);
	servo_recon_tm = GetTickCount();
}

void configure_dac_channel(void)
{
	dac_chan_enable_output_buffer(&dac_instance, DAC_CHANNEL_0);
	dac_enable(&dac_instance);
	dac_chan_write(&dac_instance, DAC_CHANNEL_0, 0x8000);
}

/**********************************
*						*	
* Configure analog comparator   *
*						*
**********************************/

static struct ac_module ac_instance;

void callback_function_ac(struct ac_module *const module_inst)
{
	ioport_set_pin_level(SERVO_EN, 0);
	servo_recon_tm = GetTickCount();
	servo_overload = 1;
}

void configure_ac(void)
{
    struct ac_config config_ac;
    ac_get_config_defaults(&config_ac);
    ac_init(&ac_instance, AC, &config_ac);

	/* configure_ac_channel */

    /* Create a new configuration structure for the Analog Comparator channel  settings and fill with the default module channel settings. */

    struct ac_chan_config config_ac_chan;
    ac_chan_get_config_defaults(&config_ac_chan);

    /* Set the Analog Comparator channel configuration settings */

    config_ac_chan.sample_mode         = AC_CHAN_MODE_CONTINUOUS;
    config_ac_chan.positive_input      = AC_CHAN_POS_MUX_PIN1;
    config_ac_chan.negative_input      = AC_CHAN_NEG_MUX_SCALED_VCC;
    config_ac_chan.vcc_scale_factor    = 16;	// Set to Vdd * (n + 1)/64 = 3.3*(4+1)/64 = 0.2578mV (with Rsense = 0.1mOhm  -> 2.5478 A)
    config_ac_chan.interrupt_selection = AC_CHAN_INTERRUPT_SELECTION_RISING;
	config_ac_chan.filter = AC_CHAN_FILTER_MAJORITY_3;
	config_ac_chan.output_mode = AC_CHAN_OUTPUT_INTERNAL;

    /* Set up a pin as an AC channel input */

    struct system_pinmux_config ac0_pin_conf;
    system_pinmux_get_config_defaults(&ac0_pin_conf);
    ac0_pin_conf.direction    = SYSTEM_PINMUX_PIN_DIR_INPUT;
    ac0_pin_conf.mux_position = I_SERVO_MUX;
    system_pinmux_pin_set_config(PIN_I_SERVO, &ac0_pin_conf);

    /* Initialize and enable the Analog Comparator channel with the user settings */

    ac_chan_set_config(&ac_instance, AC_CHAN_CHANNEL_0, &config_ac_chan);
    ac_chan_enable(&ac_instance, AC_CHAN_CHANNEL_0);
	
	/* Configure analog comparator callback */

    ac_register_callback(&ac_instance, callback_function_ac, AC_CALLBACK_COMPARATOR_0);
    ac_enable_callback(&ac_instance, AC_CALLBACK_COMPARATOR_0);
	ac_enable(&ac_instance);
	
}

void SetDACReference(uint32_t ref)
{
	DAC->CTRLB.bit.REFSEL = (ref & 0x1);
}

void SetAdcReference(uint32_t ref)
{
	if(ref < 3)
		ADC->REFCTRL.reg = ref; 
}

void SetRMSorMEAN(uint32_t value)
{
	bCompute_offset = value;
}
void Check_ADCDMA(void)
{
	//  Code to check coherence between dma_resource & status of hardware registers
	// Sometimes dma complete interrupt was not fired
		 
	DMAC->CHINTENSET.reg = adc_dma_resource.channel_id;
	if(!DMAC->CHSTATUS.bit.PEND)
	DMAC->SWTRIGCTRL.reg = 1 << adc_dma_resource.channel_id;
}

void AnalogRuntime(void)
{

	if(TimeDiff(adc_dma_tm, GetTickCount()) > adc_dma_timeout )
	{
		Check_ADCDMA();
		adc_dma_tm = GetTickCount();
	}

	if(bCompute_offset)
		Compute_ADC_Offset();
	else
		Compute_ADC_RMS(fGetADCtransfertBUffer(), BUFFER_LENGTH, GetRMSdata());
	
	// Restart servo  after overload 

	if(servo_overload && TimeDiff(servo_recon_tm, GetTickCount()) > ovl_recovery_tm)
	{
		ioport_set_pin_level(SERVO_EN, 1);
		if(servo_overload < 3)
		{
			servo_overload++;
		}
		else if(servo_overload < 10)
		{
			ovl_recovery_tm = 9999;	// Increase time between reconnections to 10 sec
			servo_overload = 0;
		}
		servo_recon_tm = GetTickCount();
	}

		
}


