/*
 * user.c
 *
 * Created: 26/09/2017 14:50:00
 *  Author: Phobos
 */ 
 #include <string.h>
 #include "asf.h"
 #include "user.h"
 #include "Analog.h"
 #include "XL320.h"
 #include "Mouth.h"
 #include "serialProtocol.h"
 #include "SoundRX.h"
 
 #define CAP1203_ADDR	0x28

 volatile uint32_t TickCount = 0;
 volatile uint32_t I2S_Periode = 0;
 volatile uint32_t bforwarding = 0, nBytes = 0;

 uint8_t usb_rx_data[USB_FW_BUFFER], usb_tx_data[USB_FW_BUFFER];

 struct usart_module serial_instance;
 struct usart_module servo_com_instance;

 struct i2c_master_module i2c_master_instance;
 struct i2s_module i2s_instance;

 struct rtc_module rtc_instance;

 struct tc_module tc_timing_instance;

 stVersion __attribute__ ((section (".configuration")))FlashVersion  = {
	 {	.bits = {.Line = 1, .Revision = 2, .Build = 1}},
	 .BoardName = "20150708.001_H5",
	 .TimeStamp = __TIMESTAMP__ ,
 .Checksum = 0x5555};


 
 int16_t i2s_RXBuffer[2][BUFFER_LENGTH];
 uint32_t bRx_data = 0;
 uint16_t DAC_buffer[BUFFER_LENGTH/2];

 static bool my_flag_autorize_cdc_transfert = false;

 bool my_callback_cdc_enable(void)
 {
	 my_flag_autorize_cdc_transfert = true;
	 return true;
 }
 void my_callback_cdc_disable(void)
 {
	 my_flag_autorize_cdc_transfert = false;
 }


 // RTC timer configuration

static void rtc_ovf_callback(void)
{
	TickCount++;
	fMouth_Main();	// Refresh mouth leds at 1KHz / 4 rows = 250Hz refresh rate
}

void SystemRTC_Init(void)
{
	struct rtc_count_config config_rtc_count;
	rtc_count_get_config_defaults(&config_rtc_count);
	config_rtc_count.prescaler           = RTC_COUNT_PRESCALER_DIV_1;
	config_rtc_count.mode                = RTC_COUNT_MODE_16BIT;
	config_rtc_count.continuously_update = true;
	rtc_count_init(&rtc_instance, RTC, &config_rtc_count);
	rtc_count_enable(&rtc_instance);

	rtc_count_register_callback(&rtc_instance, rtc_ovf_callback, RTC_COUNT_CALLBACK_OVERFLOW);
	rtc_count_enable_callback(&rtc_instance, RTC_COUNT_CALLBACK_OVERFLOW);

	rtc_count_set_period(&rtc_instance, 32);
}

uint32_t GetTickCount(void)
{
	return TickCount;
}

/*******************************************************************
	Method      :  TimeDiff 

	Description :
         This function gets the difference between two time stamp, time
         overflow is considered.
**********************************************************************/

uint32_t TimeDiff(uint32_t time_start, uint32_t time_end)
{
	if (time_end >= time_start) 
		return time_end - time_start;
	else 
		return UINT32_MAX - time_start + time_end + 1;
}

void Delay_ms(uint32_t value)
{
	uint32_t start_tm = TickCount;
	while(TimeDiff(start_tm, TickCount) < value);
}

 /* Configure I2c module */

 struct i2c_master_module  *get_i2c_instance(void)
 {
	 return &i2c_master_instance;
 }

 void configure_i2c_master(void)
 {
	 /* Initialize config structure and software module. */
	 struct i2c_master_config config_i2c_master;
	 i2c_master_get_config_defaults(&config_i2c_master);
	 /* Change buffer timeout to something longer. */
	 config_i2c_master.baud_rate = I2C_MASTER_BAUD_RATE_400KHZ;
	 config_i2c_master.buffer_timeout = 65535;
	 config_i2c_master.pinmux_pad0 = PINMUX_PA22C_SERCOM3_PAD0;
	 config_i2c_master.pinmux_pad1 = PINMUX_PA23C_SERCOM3_PAD1;
	 config_i2c_master.generator_source = GCLK_GENERATOR_3;
	 /* Initialize and enable device with config. */
	 i2c_master_init(&i2c_master_instance, SERCOM3, &config_i2c_master);
	 i2c_master_enable(&i2c_master_instance);
 } // *****************************************************************************
 // fRdSRAM()	 Reads data from SRAM by Angel Garcia
 // v1.00	05/09/12   ... created ...
 // This routine reads data from SRAM
 //	Input:				Data type:		Values:
 //	------				----------		-------
 //	*pData				uint8_t            pointer to read data buffer
 //	InitAddr				uint16_t 	E2Prom addres to start writing data
 //	nbyte	 			uint16_t	Number of bytes to write
 //	Output:
 //	-------
 //	0 if fault
 //	number of writen bytes
 // *****************************************************************************

 uint32_t I2C_ReadData(uint8_t *pData, uint16_t InitAddr, uint16_t nbytes)
 {
	 struct i2c_master_packet wr_packet;
	 struct i2c_master_packet rd_packet;
	 
	 if (!nbytes || (InitAddr + nbytes -1) > 0xff)return 0;
	 
	 //Select slave and  Write Data address
	 
	wr_packet.address = CAP1203_ADDR;;
	wr_packet.data_length = 1;
	wr_packet.data = (uint8_t*)&InitAddr;
	wr_packet.ten_bit_address = false,
	wr_packet.high_speed      = false,
	wr_packet.hs_master_code  = 0;
	 
	 if(i2c_master_write_packet_wait(&i2c_master_instance, &wr_packet) != STATUS_OK)return 0;
	 
	 // Read 32 bytes from data address
	 
	rd_packet.address = CAP1203_ADDR;;
	rd_packet.data_length = nbytes;
	rd_packet.data = pData;
	rd_packet.ten_bit_address = false,
	rd_packet.high_speed      = false,
	rd_packet.hs_master_code  = 0;

	 if(i2c_master_read_packet_wait(&i2c_master_instance, &rd_packet) != STATUS_OK)return 0;
	 else return nbytes;
 }
 // *****************************************************************************
 // fWrSRAM()	 Writes data to E2PROM by Angel Garcia
 // v1.00	05/09/12   ... created ...
 // This routine writes data to SRAM
 //	Input:				Data type:		Values:
 //	------				----------		-------
 //	*pData				uint8_t            pointer to data buffer
 //	InitAddr			uint16_t 	SRAM addres to start writing data
 //	nbyte	 			uint16_t	Number of bytes to write
 //	Output:
 //	-------
 //	0 if fault
 //	number of writen bytes
 // *****************************************************************************

 uint32_t I2C_WriteData(uint8_t *pData, uint16_t InitAddr, uint16_t nbytes)
 {
	 struct i2c_master_packet wr_RTC_packet;
	 int32_t error;
	 uint8_t buffer[256];
	 if (!nbytes || (uint32_t)(InitAddr + nbytes + 1) > sizeof(buffer))
		return 0;

	 memcpy_ram2ram(&buffer[1], pData, nbytes);
	 
	 buffer[0] = InitAddr;
	 
	 wr_RTC_packet.address = CAP1203_ADDR;;
	 wr_RTC_packet.data_length = nbytes + 1;
	 wr_RTC_packet.data = buffer;
	 error = i2c_master_write_packet_wait(&i2c_master_instance, &wr_RTC_packet);
	 
	 if(error != STATUS_OK)return 0;
	 else return nbytes;
 }
 /* Configure serial */

 struct usart_module *GetUsartInstance(void)
 {
	 return &serial_instance;
 }

 void configure_usart_Serial(void)
 {
	 enum status_code status = STATUS_OK;
	 struct usart_config config_usart;
	 usart_get_config_defaults(&config_usart);

	 config_usart.baudrate    = 115200;
	 config_usart.mux_setting = USART_RX_3_TX_2_XCK_3;
	 config_usart.pinmux_pad0 = PINMUX_DEFAULT;
	 config_usart.pinmux_pad1 = PINMUX_DEFAULT;
	 config_usart.pinmux_pad2 = PINMUX_PB22D_SERCOM5_PAD2;
	 config_usart.pinmux_pad3 = PINMUX_PB23D_SERCOM5_PAD3;
	 config_usart.sample_rate = USART_SAMPLE_RATE_16X_FRACTIONAL;
	 config_usart.sample_adjustment = USART_SAMPLE_ADJUSTMENT_9_10_11;
	 config_usart.generator_source = GCLK_GENERATOR_3;
	 status = usart_init(&serial_instance,	SERCOM5, &config_usart);
	 while (status != STATUS_OK) {
	 };
	 usart_enable(&serial_instance);
	 fSerial_InitSerialProtocol((uint32_t *)&serial_instance);
 }

uint32_t *GetServoRxByteLength(void)
{
	return &bRx_data;
}
/*
static void usart_servo_read_callback(struct usart_module *const usart_module)
{
	bRx_data++;
}*/

struct usart_module *GetServoInstance(void)
{
	return &servo_com_instance;
}
 
 void configure_usart_Servo(void)
 {
	 enum status_code status = STATUS_OK;
	 struct usart_config config_usart;
	 struct system_pinmux_config config_pinmux;


	 usart_get_config_defaults(&config_usart);

	 config_usart.baudrate    = 1000000;
	 config_usart.mux_setting = USART_RX_2_TX_2_XCK_3;
	 config_usart.pinmux_pad0 = PINMUX_PB10D_SERCOM4_PAD2;
	 config_usart.pinmux_pad1 = PINMUX_DEFAULT;
	 config_usart.pinmux_pad2 = PINMUX_DEFAULT;
	 config_usart.pinmux_pad3 = PINMUX_DEFAULT;
	 config_usart.collision_detection_enable = true;
	 config_usart.sample_rate = USART_SAMPLE_RATE_8X_FRACTIONAL;
	 config_usart.generator_source = GCLK_GENERATOR_3;	
	 status = usart_init(&servo_com_instance,	SERCOM4, &config_usart);
	 if(status == STATUS_OK)
		usart_enable(&servo_com_instance);

	// Enable because usart_init call changes pimux for these pins

	config_pinmux.mux_position = SYSTEM_PINMUX_GPIO;
	config_pinmux.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
	config_pinmux.input_pull = SYSTEM_PINMUX_PIN_PULL_NONE;
	config_pinmux.powersave = false;

	system_pinmux_pin_set_config(G_COLUMN_1, &config_pinmux);
	system_pinmux_pin_set_config(G_COLUMN_2, &config_pinmux);
	system_pinmux_pin_set_config(G_COLUMN_3, &config_pinmux);

   /* usart_register_callback(&servo_com_instance, usart_servo_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&servo_com_instance, USART_CALLBACK_BUFFER_RECEIVED);*/
	
 }

 /* Configure I2S module */

 struct i2s_module *get_i2s_instance(void)
 {
	 return &i2s_instance;
 }

 void configure_i2s(void)
 {
	 struct i2s_clock_unit_config config_clock_unit;

	 i2s_init(&i2s_instance, I2S);

	 i2s_clock_unit_get_config_defaults(&config_clock_unit);

	 config_clock_unit.clock.gclk_src = GCLK_GENERATOR_3;
	 config_clock_unit.clock.mck_src = I2S_MASTER_CLOCK_SOURCE_GCLK;
	 config_clock_unit.clock.mck_out_enable = false;
	 config_clock_unit.clock.sck_src = I2S_SERIAL_CLOCK_SOURCE_SCKPIN;
	 config_clock_unit.clock.sck_div = 1;
	 config_clock_unit.frame.number_slots = 2;
	 config_clock_unit.frame.slot_size = I2S_SLOT_SIZE_16_BIT;
	 config_clock_unit.frame.data_delay = I2S_DATA_DELAY_I2S;
	 config_clock_unit.frame.frame_sync.source = I2S_FRAME_SYNC_SOURCE_FSPIN;
	 config_clock_unit.frame.frame_sync.width = I2S_FRAME_SYNC_WIDTH_SLOT;
	 config_clock_unit.mck_pin.enable = false;
	 config_clock_unit.sck_pin.enable = true;
	 config_clock_unit.sck_pin.gpio = I2S_SCK;
	 config_clock_unit.sck_pin.mux = I2S_SCK_MUX;
	 config_clock_unit.fs_pin.enable = true;
	 config_clock_unit.fs_pin.gpio = I2S_LRCK;
	 config_clock_unit.fs_pin.mux = I2S_LRCK_MUX;
	 i2s_clock_unit_set_config(&i2s_instance, I2S_CLOCK_UNIT_0, &config_clock_unit);

	 struct i2s_serializer_config config_serializer;
	 i2s_serializer_get_config_defaults(&config_serializer);

	 config_serializer.clock_unit = I2S_CLOCK_UNIT_0;
	 config_serializer.data_size = I2S_DATA_SIZE_16BIT;

	 config_serializer.mode = I2S_SERIALIZER_TRANSMIT;
	 config_serializer.data_pin.enable = true;
	 config_serializer.dma_usage = I2S_DMA_USE_SINGLE_CHANNEL_FOR_ALL;
	 config_serializer.mono_mode = true;
	 config_serializer.data_adjust_left_in_slot = true;
	 config_serializer.data_pin.gpio = I2S_DOUT;
	 config_serializer.data_pin.mux = I2S_DOUT_MUX;
	 config_serializer.data_padding = I2S_DATA_PADDING_0;
	 i2s_serializer_set_config(&i2s_instance, I2S_SERIALIZER_1, &config_serializer);

	 config_serializer.mode = I2S_SERIALIZER_RECEIVE;
	 config_serializer.data_pin.enable = true;
	 config_serializer.dma_usage = I2S_DMA_USE_SINGLE_CHANNEL_FOR_ALL;
	 config_serializer.mono_mode = true;
	 config_serializer.data_adjust_left_in_slot = true;
	 config_serializer.data_pin.gpio = I2S_DIN;
	 config_serializer.data_pin.mux = I2S_DIN_MUX;
	 config_serializer.data_padding = I2S_DATA_PADDING_0;
	 i2s_serializer_set_config(&i2s_instance, I2S_SERIALIZER_0, &config_serializer);

	 i2s_enable(&i2s_instance);
	 i2s_clock_unit_enable(&i2s_instance, I2S_CLOCK_UNIT_0);
	 i2s_serializer_enable(&i2s_instance, I2S_SERIALIZER_0);
	 i2s_serializer_enable(&i2s_instance, I2S_SERIALIZER_1);

 	i2s_serializer_register_callback(&i2s_instance, I2S_SERIALIZER_0, i2s_callback_RX, I2S_SERIALIZER_CALLBACK_OVER_UNDER_RUN);
 	i2s_serializer_enable_callback(&i2s_instance, I2S_SERIALIZER_0, I2S_SERIALIZER_CALLBACK_OVER_UNDER_RUN);
 }

 void EnableUSBPortForwarding(uint32_t value)
 {
	bforwarding = value;
 }

 void USBFordwarding_runtime(void)
 {
 	if(bforwarding)
 	{
	 	if(usart_get_job_status(GetServoInstance(), USART_TRANSCEIVER_RX) == STATUS_BUSY)
	 	{
		 	nBytes = USB_FW_BUFFER - GetServoInstance()->remaining_rx_buffer_length;
		 	if(nBytes)
		 	{
			 	usart_abort_job(GetServoInstance(),USART_TRANSCEIVER_RX);
			 	udi_cdc_write_buf(&usb_tx_data, nBytes);
			 	usart_read_buffer_job(GetServoInstance(),(uint8_t *)&usb_tx_data, USB_FW_BUFFER);

		 	}
	 	}

	 	nBytes = udi_cdc_get_nb_received_data();
	 	if(nBytes)
	 	{
		 	usart_abort_job(GetServoInstance(),USART_TRANSCEIVER_RX);
		 	udi_cdc_read_buf(&usb_rx_data, nBytes);

		 	SwitchComDir(1);
		 	sendData((uint8_t *)&usb_rx_data, nBytes);

		 	SwitchComDir(0);
		 	usart_read_buffer_job(GetServoInstance(),(uint8_t *)&usb_tx_data, USB_FW_BUFFER);
	 	}
 	}
}

