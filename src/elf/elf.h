#ifndef _ELF_HEADER_
#define _ELF_HEADER_

#include "common/defines.h"
#include "common/cmn_datatypes.h"


/* 0 -> Switch flipped down, 1 -> Switch flipped up
 * First 4 bits are ignored
 * - IN switch
 * - WAIT
 * - CLR
 * - R/W
 */
void elf_setControlSwitches(uint8_t sw_bitmask);

/* 0 -> Switch flipped down, 1 -> Switch flipped up
 * All bits are used.
 * - MSb is D7, LSb is D0
 */
void elf_setDataSwitches(uint8_t sw_bitmask);

uint8_t elf_readDataAndAdvance(void);

/* Delay time between elf control instructions. In uSecs.
 */
void elf_setDelayTime(unsigned long delay);


void elf_hl_uploadRam(uint16_t address, uint8_t *buffer, uint16_t len);
void elf_hl_downloadRam(uint16_t address, uint8_t *buffer, uint16_t len);
void elf_hl_runCode(uint16_t address);

#endif /* _ELF_HEADER_ */
