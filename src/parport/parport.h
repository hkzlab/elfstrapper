#ifndef _PARPORT_HEADER_
#define _PARPORT_HEADER_

#include "common/cmn_datatypes.h"

typedef enum {
	PP_DATA = 0,
	PP_STATUS = 1,
	PP_CONTROL = 2
} PP_REGISTER;

void parport_deinit();
int8_t parport_init(const char *pdev);

uint8_t parport_read(PP_REGISTER reg);
void parport_write(PP_REGISTER reg, uint8_t data);

#endif /* _PARPORT_HEADER_ */
