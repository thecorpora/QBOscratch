/*
 * Analog.h
 *
 * Created: 21/09/2017 16:03:04
 *  Author: agarcia
 */ 


#ifndef ANALOG_H_
#define ANALOG_H_

#define VDD				3.3		// Vdd value
#define VADC            3.3/1.48		// Value of Vadc reference

#define KADC8		(float)(VADC / 255.0)      // K of ADC in V
#define KADC10		(float)(VADC / 1023.0)      // K of ADC in V
#define KADC11		(float)(VADC / 2047.0)
#define KADC12		(float)(VADC / 4095.0)
#define KADC15		(float)(VADC / 32767.0)
#define KADC16		(float)(VADC / 65535.0)

#define KDAC		(float)(VADC / 1024.0)

#define NSCAN_CHN		3
#define BUFFER_LENGTH	256

/* ADC reference selection */
    
#define ADC_VREF_1000mV 0   // Gain 40dB
#define ADC_VREF_2230mV 1   // Gain 33dB 
#define ADC_VREF_1650mV 2   // Gain 35.6dB

/* DAC reference selection */

#define DAC_VREF_1V 0       // 15.5dB output signal 
#define DAC_VREF_3V3 1      // 26dB output signal


uint32_t Compute_ADC_RMS(int16_t *, uint16_t , float *);
uint32_t Compute_ADC_Offset(void);
int16_t *fGetADCtransfertBUffer(void);
uint16_t *GetADCBuffer(void);
float *GetRMSdata(void);
uint32_t SetMicIndex(uint32_t index);

void configure_ADC_trigger(void);
void tc_callback_to_toggle_led(struct tc_module *const );
void configure_ADC_dma_resource(struct dma_resource *);
void setup_ADC_DMA_transfer_descriptor(DmacDescriptor *);
void dma_ADC_tranfer_done(struct dma_resource* const);
void adc_complete_callback(const struct adc_module *const );
void configure_dma_event_channel(struct events_resource *);
void callback_function_ac(struct ac_module *const);
struct adc_module *GetADC_instance(void);
struct dac_module *GetDAC_instance(void);
void configure_dac(void);
void configure_dac_channel(void);
void configure_adc(int16_t *);
void configure_adc_callbacks(void);
void configure_adc_trigger_event(void);
void configure_ADC_trigger_event_channel(struct events_resource *);
void configure_ADC_dma(void);
void configure_i2s(void);
void configure_ac(void);
void SetDACReference(uint32_t );
void SetAdcReference(uint32_t );
void SetRMSorMEAN(uint32_t);
void Check_ADCDMA(void);
void AnalogRuntime(void);

#endif /* ANALOG_H_ */