#include "elf/elf.h"
#include "parport/parport.h"
#include "utils/utils.h"

#define DEF_SLEEPTIME 1000
static long elf_delaytime = DEF_SLEEPTIME;

void elf_setDelayTime(long delay) {
	elf_delaytime = delay;
}

void elf_setControlSwitches(uint8_t sw_bitmask) {
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
	parport_write(PP_DATA, sw_bitmask);
	util_sleep(elf_delaytime);
}
