/*
 * Flash.c
 *
 * Created: 18/02/2016 10:53:16
 *  Author: agarcia
 */ 

#include <asf.h>
#include "user.h"
#include "Flash.h"


void vE2PROM_Init(void)
{
	struct nvm_config config_nvm;
	nvm_get_config_defaults(&config_nvm);
	config_nvm.manual_page_write = false;
	nvm_set_config(&config_nvm);
}

uint32_t vE2PROM_ErasePage(uint32_t addr)
{
	uint32_t error;
	do
	{
		error = nvm_erase_row(EEPROM_ADDR + addr);
	} while (error == STATUS_BUSY);	
	return error;
}

uint32_t vE2PROM_Write(uint8_t *buffer, uint16_t addr ,uint16_t length)
{
	uint32_t error;
	
	uint16_t nBytes;

	do
	{
		if(length > NVMCTRL_PAGE_SIZE)nBytes = NVMCTRL_PAGE_SIZE;
		else nBytes = length; 
		do
		{
			error = nvm_write_buffer( EEPROM_ADDR + addr, buffer, nBytes);
		} while (error == STATUS_BUSY);
		
		buffer += nBytes;
		addr += nBytes;
		length -= nBytes;
	} while (length > 0);

	return error;
}

uint32_t vE2PROM_Read(uint8_t *buffer, uint16_t addr ,uint16_t length)
{
	uint32_t error;
	uint16_t nBytes;
	
	do
	{
		if(length > NVMCTRL_PAGE_SIZE)nBytes = NVMCTRL_PAGE_SIZE;
		else nBytes = length; 

		do
		{
			error = nvm_read_buffer(EEPROM_ADDR + addr, buffer, nBytes);
		} while (error == STATUS_BUSY);

		buffer += nBytes;
		addr += nBytes;
		length -= nBytes;
	} while (length > 0);
	
	return error;
}

 


