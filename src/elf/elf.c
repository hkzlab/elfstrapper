#include "elf/elf.h"
#include "parport/parport.h"
#include "utils/utils.h"

#include "logger/logger.h"

#define DEF_SLEEPTIME 1000
static unsigned long elf_delaytime = DEF_SLEEPTIME;

void elf_setDelayTime(unsigned long delay) {
	logger_print(LOGR_EXTN, "elf::elf_setDelayTime(%lu)\n", delay);
	
	elf_delaytime = delay;
}

void elf_setControlSwitches(uint8_t sw_bitmask) {
	logger_print(LOGR_EXTN, "elf::elf_setControlSwitches(0x%.2X)\n", sw_bitmask);
	
	uint8_t ctrl_bitmask = 0;

 	// Correctly convert the bitmask	
	ctrl_bitmask |= ((sw_bitmask & 0x8) ? 0x00 : 0x01); // IN connected to /STROBE, must be reversed
	ctrl_bitmask |= ((sw_bitmask & 0x4) ? 0x00 : 0x02); // WAIT connected to /AUTOFEED, must be reversed
	ctrl_bitmask |= ((sw_bitmask & 0x2) ? 0x04 : 0x00); // CLR connected to INIT, must NOT be reversed
	ctrl_bitmask |= ((sw_bitmask & 0x1) ? 0x08 : 0x00); // R/W connected to /SELECT, must be reversed (UP generates a GND signal, which means Write Enable)

	parport_write(PP_CONTROL, ctrl_bitmask);
	util_sleep(elf_delaytime);
}

void elf_setDataSwitches(uint8_t sw_bitmask) {
	logger_print(LOGR_EXTN, "elf::elf_setDataSwitches(0x%.2X)\n", sw_bitmask);
	
	parport_write(PP_DATA, sw_bitmask);
	util_sleep(elf_delaytime);
}

uint8_t elf_readDataAndAdvance(void) {
	logger_print(LOGR_EXTN, "elf::elf_readDataAndAdvance()\n");
	
	uint8_t nibble_a, nibble_b;

	elf_setControlSwitches(0x08); // Every control switch off
	nibble_a = parport_read(PP_STATUS); // Read the first nibble
	elf_setControlSwitches(0x00); // "Push" the IN button
	nibble_b = parport_read(PP_STATUS); // Read the second nibble
	elf_setControlSwitches(0x08); // "Release" the IN button. This advances the counter

	nibble_a = (nibble_a & 0x7F) | ((~nibble_a) & 0x80);
	nibble_b = (nibble_b & 0x7F) | ((~nibble_b) & 0x80);

	return ((nibble_b >> 4) & 0x0F) | (nibble_a & 0xF0);
}

void elf_hl_uploadRam(uint16_t address, uint8_t *buffer, uint16_t len) {
	logger_print(LOGR_EXTN, "elf::elf_hl_uploadRam(0x%.4X, 0x%.8X, 0x%.4X)\n", address, buffer, len);
	
	uint32_t cur_addr = 0, addr_difference;

	elf_setControlSwitches(0x04); // Wait up
	elf_setControlSwitches(0x00); // All Down

	// Skip to the address 
	for (cur_addr = 0; cur_addr < address; cur_addr++) {
		elf_setControlSwitches(0x00);
		elf_setControlSwitches(0x08);
	}

	addr_difference = cur_addr;
	elf_setControlSwitches(0x01); // Write up
	for (; cur_addr < (address + len); cur_addr++) {
		elf_setDataSwitches(buffer[cur_addr - addr_difference]);
		elf_setControlSwitches(0x01);
		elf_setControlSwitches(0x09);
	}	
}

void elf_hl_downloadRam(uint16_t address, uint8_t *buffer, uint16_t len) {
	logger_print(LOGR_EXTN, "elf::elf_hl_downloadRam(0x%.4X, 0x%.8X, 0x%.4X)\n", address, buffer, len);
	
	uint8_t data;

	elf_setControlSwitches(0x04); // Wait up
	elf_setControlSwitches(0x00); // All Down
	for (uint32_t cur_addr = 0; cur_addr < (address + len); cur_addr++) {
		data = elf_readDataAndAdvance();
	
		if (cur_addr >= address) {
			buffer[cur_addr - address] = data;
		}
	}
}

void elf_hl_runCode(uint16_t address) {
	logger_print(LOGR_EXTN, "elf::elf_hl_runCode(0x%.4X)\n", address);
	
	elf_setControlSwitches(0x04); // Wait up
	elf_setControlSwitches(0x00); // All Down

	// Skip to the address 
	for (uint32_t cur_addr = 0; cur_addr < address; cur_addr++) {
		elf_setControlSwitches(0x00);
		elf_setControlSwitches(0x08);
	}

	// And then run
	elf_setControlSwitches(0x06);
}


