#include "parport.h"

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
	fprintf(stdout, "parport::parport_deinit()\n");

	if (pp_fd != -1) {
		ioctl(pp_fd, PPRELEASE);
		close(pp_fd);

		pp_fd = -1;
	}
}

int8_t parport_init(const char *pdev) {
	fprintf(stdout, "parport::parport_init(%s)\n", pdev);
	
	pp_fd = open(pdev, O_RDWR);
	if (pp_fd == -1) {
		perror("open");

		return -1;
	}

	if (ioctl(pp_fd, PPCLAIM)) {
		perror("PPCLAIM");
		close(pp_fd);

		return -1;
	}

	int mode = IEEE1284_MODE_COMPAT;
	if (ioctl(pp_fd, PPSETMODE, &mode)) {
		perror("PPSETMODE");
      	close(pp_fd);
		
		return -1;
   }

	int ddir = 0;
	if (ioctl(pp_fd, PPDATADIR, &ddir)) {
		perror("PPDATADIR");
		close(pp_fd);

		return -1;
	}

	return 0;
}

uint8_t parport_read(PP_REGISTER reg) {
	fprintf(stdout, "parport::parport_read(%u)\n", reg);
	
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
	fprintf(stdout, "parport::parport_write(%u, 0x%.2X)\n", reg, data);

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
