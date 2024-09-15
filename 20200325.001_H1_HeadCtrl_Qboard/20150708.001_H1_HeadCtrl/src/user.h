/*
 * user.h
 *
 * Created: 26/09/2017 14:49:41
 *  Author: Phobos
 */ 


#ifndef USER_H_
#define USER_H_

#define USB_FW_BUFFER	64	// USB portforwarding buffer size definition

typedef struct
{
	union
	{
		struct
		{
			uint16_t Build:6;
			uint16_t Revision:4;
			uint16_t Line :6;
		}bits;
		uint16_t reg;
	}Version;
	uint8_t BoardName[16];
	uint8_t TimeStamp[24];
	uint16_t Checksum;
}stVersion;

void SystemRTC_Init(void);
uint32_t GetTickCount(void);
uint32_t TimeDiff(uint32_t, uint32_t);
void Delay_ms(uint32_t);
void configure_usart_Serial(void);
void configure_usart_Servo(void);
void configure_i2c_master(void);
uint32_t I2C_ReadData(uint8_t *, uint16_t , uint16_t );
uint32_t I2C_WriteData(uint8_t *, uint16_t, uint16_t);
struct usart_module *GetUsartInstance(void);
struct i2s_module *get_i2s_instance(void);
struct i2c_master_module  *get_i2c_instance(void);
uint32_t *GetServoRxByteLength(void);
struct usart_module *GetServoInstance(void);
void EnableUSBPortForwarding(uint32_t value);
void USBFordwarding_runtime(void);

#endif /* USER_H_ */