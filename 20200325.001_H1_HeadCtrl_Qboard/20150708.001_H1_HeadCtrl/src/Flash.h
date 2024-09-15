/*
 * Flash.h
 *
 * Created: 18/02/2016 12:49:24
 *  Author: agarcia
 */ 


#ifndef FLASH_H_
#define FLASH_H_

#define EEPROM_ROWS			1
#define EEPROM_SIZE			EEPROM_ROWS * NVMCTRL_ROW_SIZE
#define EEPROM_ADDR			NVMCTRL_FLASH_SIZE - EEPROM_SIZE	// EEPROM_SIZE , 1 ROW is minimum EEROM erase space

// const uint8_t vEEPROM[NVMCTRL_ROW_SIZE] __attribute__((at(0x00003f00)));

void vE2PROM_Init(void);
uint32_t vE2PROM_ErasePage(uint32_t);
uint32_t vE2PROM_Write(uint8_t *, uint16_t, uint16_t);
uint32_t vE2PROM_Read(uint8_t *, uint16_t, uint16_t);

#endif /* FLASH_H_ */