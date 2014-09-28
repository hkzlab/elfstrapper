#include "parport.h"

#include "logger/logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <linux/parport.h>
#include <linux/ppdev.h>

static int pp_fd = -1;

void parport_deinit() {
	logger_print(LOGR_VERB, "parport::parport_deinit()\n");

	if (pp_fd != -1) {
		ioctl(pp_fd, PPRELEASE);
		close(pp_fd);

		pp_fd = -1;
	}
}

int8_t parport_init(const char *pdev) {
	logger_print(LOGR_VERB, "parport::parport_init(%s)\n", pdev);

	// Open parallel port device file
	pp_fd = open(pdev, O_RDWR);
	if (pp_fd == -1) {
		perror("open");

		return -1;
	}

	// Claim exclusive access to it
	if (ioctl(pp_fd, PPCLAIM)) {
		perror("PPCLAIM");
		close(pp_fd);

		return -1;
	}

	// Set compatibility mode for the parallel port
	int mode = IEEE1284_MODE_COMPAT;
	if (ioctl(pp_fd, PPSETMODE, &mode)) {
		perror("PPSETMODE");
      	close(pp_fd);
		
		return -1;
   }

	// Set data direction to output
	int ddir = 0;
	if (ioctl(pp_fd, PPDATADIR, &ddir)) {
		perror("PPDATADIR");
		close(pp_fd);

		return -1;
	}

	return 0;
}

uint8_t parport_read(PP_REGISTER reg) {
	logger_print(LOGR_VERB, "parport::parport_read(%u)\n", reg);
	
	uint8_t data = 0;

	switch (reg) {
		case PP_CONTROL:
			ioctl(pp_fd, PPRCONTROL, &data);
			return data;
		case PP_STATUS:
			ioctl(pp_fd, PPRSTATUS, &data);
			return data;
		case PP_DATA:
		default:
			return 0;
	}
}

void parport_write(PP_REGISTER reg, uint8_t data) {
	logger_print(LOGR_VERB, "parport::parport_write(%u, 0x%.2X)\n", reg, data);

	switch(reg) {
		case PP_DATA:
			ioctl(pp_fd, PPWDATA, &data);
			break;
		case PP_STATUS: // This can't be written
			break;
		case PP_CONTROL:
			ioctl(pp_fd, PPWCONTROL, &data);
			break;
		default:
			break;
	}
}
