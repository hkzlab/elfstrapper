#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include <getopt.h>

#include "common/defines.h"
#include "common/cmn_datatypes.h"

#include "logger/logger.h"
#include "parport/parport.h"
#include "utils/utils.h"


#define PPORT_NAME_BUF_SIZE 50
#define FNAME_BUF_SIZE 256
static char parport_device[PPORT_NAME_BUF_SIZE];

static void exit_cleanup(); // Called at exit to provide final cleanup
static void print_usage(char *progname); // Small utility function to print the command help

int main(int argc, char *argv[]) {
	int opt;
	uint8_t h_flag = 0, v_flag = 0, d_flag = 0; // Option flags

	// Parse the command line
	while ((opt = getopt(argc, argv, "d:D:hv")) != -1) {
		switch(opt) {
			case 'd': // Parallel port to use
				d_flag = 1;
				strncpy(parport_device, optarg, PPORT_NAME_BUF_SIZE - 1);
				break;
			case 'v': // Version flag
				v_flag = 1;
				break;
			case 'h': // Help requested
				h_flag = 1;
				break;
			case 'D': { // Set verbosity level
				int llevel = atoi(optarg);
 				
				if ((llevel > LOGR_VERB) || (llevel < LOGR_NONE)) {
 					fprintf(stderr, "Invalid log level!\n");
 					
					return EXIT_FAILURE;
 				}
 
				logger_setVerbosity(llevel);;
				break;
			}
			case '?':
				if (optopt == 'd') {
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				} else if (isprint(optopt)) {
					fprintf(stderr, "Unknown option '-%c'.\n", optopt);
				} else {
					fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
				}
			default:
				return EXIT_FAILURE;
		}
	}

	// Manage the option flags
	if (v_flag) {
		fprintf(stdout, "%s Version %s\n", argv[0], VER_STR);
		return EXIT_SUCCESS;
	}

	if (h_flag) {
		print_usage(argv[0]);
		return EXIT_SUCCESS;
	}

	if (!d_flag) {
		fprintf(stderr, "Missing mandatory parameters!\n");
		print_usage(argv[0]);

		return EXIT_FAILURE;
	}

	// Register the exit handler
	atexit(exit_cleanup);

	if (parport_init(parport_device) < 0) { // Initialize the parallel port module
		fprintf(stderr, "Failure to initialize the parallel port device %s .\n", parport_device);
		return EXIT_FAILURE;
	}

	// TEST TEST TEST
/*
	parport_write(PP_CONTROL, 0x0F);
	util_sleep(DEF_SLEEPTIME);
	for (int cnt = 0; cnt < 0xFFFF; cnt++) {
		parport_write(PP_DATA, cnt);
		util_sleep(DEF_SLEEPTIME);
		parport_write(PP_CONTROL, 0x0A);
		parport_write(PP_DATA, cnt);
		util_sleep(DEF_SLEEPTIME);
		parport_write(PP_CONTROL, 0x0B);
	}

	parport_write(PP_CONTROL, 0x01);
	util_sleep(DEF_SLEEPTIME);
	parport_write(PP_CONTROL, 0x03);
	util_sleep(DEF_SLEEPTIME);
	for (int cnt = 0; cnt < 0xFFFF; cnt++) {
		util_sleep(DEF_SLEEPTIME);
		parport_write(PP_CONTROL, 0x02 + 1);
		util_sleep(DEF_SLEEPTIME);
		fprintf(stdout, "CONTENT %.2X\n", parport_read(PP_STATUS));
		parport_write(PP_CONTROL, 0x02 + 0);
		util_sleep(DEF_SLEEPTIME);
		fprintf(stdout, "CONTENT %.2X\n", parport_read(PP_STATUS));
	}
*/	
	return EXIT_SUCCESS;
}

static void print_usage(char *progname) {
	fprintf(stdout, "Usage: %s -d PARALLEL_DEVICE [-D level] [-v] [-h]\n"
			"\t-d PARALLEL_DEVICE\tDefine the parallel port to use.\n"
			"\t-D level\t\tLogging verbosity, ranging from 0 to 3\n"
			"\t-h\t\t\tPrint this help\n"
			"\t-v\t\t\tPrint version\n", progname);
}

static void exit_cleanup() {

	// De-initialize the parallel port
	parport_deinit();

	return;
}

