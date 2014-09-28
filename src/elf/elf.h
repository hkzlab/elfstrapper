#ifndef _ELF_HEADER_
#define _ELF_HEADER_

#include "common/defines.h"
#include "common/cmn_datatypes.h"

#include "parport/parport.h"

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

#endif /* _ELF_HEADER_ */
