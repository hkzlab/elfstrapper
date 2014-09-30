#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include <getopt.h>

#include "common/defines.h"
#include "common/cmn_datatypes.h"

#include "logger/logger.h"
#include "parport/parport.h"
#include "elf/elf.h"
#include "utils/utils.h"


#define PPORT_NAME_BUF_SIZE 50
#define FNAME_BUF_SIZE 256
static char parport_device[PPORT_NAME_BUF_SIZE];
static char filename[FNAME_BUF_SIZE];

static void exit_cleanup(); // Called at exit to provide final cleanup
static void print_usage(char *progname); // Small utility function to print the command help

static void eo_runMode(void);
static void eo_uploadFromFile(uint16_t address);
static void eo_downloadToFile(uint16_t address, uint16_t size);

int main(int argc, char *argv[]) {
	int opt;
	uint8_t h_flag = 0, v_flag = 0, d_flag = 0, R_flag = 0, f_flag = 0, F_flag = 0, a_flag = 0, l_flag = 0; // Option flags
	uint16_t start_address = 0x0000;
	uint16_t data_len = 0xFFFF;

	// Parse the command line
	while ((opt = getopt(argc, argv, "d:D:w:f:F:Rhva:l:")) != -1) {
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
			case 'w': // Set parallel port delay in uSecs
				elf_setDelayTime(atol(optarg));
				break;
			case 'R': // Run mode
				R_flag = 1;
				break;
			case 'f': // Input file
				f_flag = 1;
				strncpy(filename, optarg, FNAME_BUF_SIZE - 1);
				break;
			case 'F': // Output file
				F_flag = 1;
				strncpy(filename, optarg, FNAME_BUF_SIZE - 1);
				break;
			case 'a': // Start address
				a_flag = 1;
				break;
			case 'l':
				l_flag = 1;
				break;
			case 'D': { // Set verbosity level
				int llevel = atoi(optarg);
 				
				if ((llevel > LOGR_VERB) || (llevel < LOGR_NONE)) {
 					fprintf(stderr, "Invalid log level!\n");
 					
					return EXIT_FAILURE;
 				}
 
				logger_setVerbosity(llevel);
				break;
			}
			case '?':
				if (optopt == 'd' || optopt == 'D' || optopt == 'w' || optopt == 'f' || optopt == 'F' || optopt == 'l' || optopt == 'a') {
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

	if (F_flag && f_flag) {
		fprintf(stderr, "Invalid option combination.\n");
		return EXIT_FAILURE;
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

	if (f_flag) 
		eo_uploadFromFile(start_address);
	else if (F_flag)
		eo_downloadToFile(start_address, data_len);


	if (R_flag)
		eo_runMode();

	return EXIT_SUCCESS;
}

static void print_usage(char *progname) {
	fprintf(stdout, "Usage: %s -d PARALLEL_DEVICE [-F/-f filename] [-D level] [-w delay] [-R] [-v] [-h]\n"
			"\t-d PARALLEL_DEVICE\tDefine the parallel port to use.\n"
			"\t-D level\t\tLogging verbosity, ranging from 0 to 3\n"
			"\t-w delay\t\tDelay time between parport commands, in uSeconds.\n"
			"\t-f filename\t\tSource filename.\n"
			"\t-F filename\t\tDestination filename.\n"
			"\t-R\t\tPut the card in RUN mode.\n"
			"\t-h\t\t\tPrint this help\n"
			"\t-v\t\t\tPrint version\n", progname);
}

static void exit_cleanup() {

	// De-initialize the parallel port
	parport_deinit();

	return;
}

/*** *** ***/
static void eo_runMode(void) {
	fprintf(stdout, "Setting the card into RUN mode and going to sleep...\nPress Ctrl-C to quit.\n");
	elf_hl_runCode(0x0000);
	while(1) util_sleep(10);
}

static void eo_uploadFromFile(uint16_t address) {
		uint8_t f_buf[0xFFFF];
		FILE *f_source = fopen(filename, "rb");

		if (f_source == NULL) {
			perror(filename);
			return EXIT_FAILURE;
		}

		long f_size = fread(f_buf, 1, 0xFFFF, f_source);
		fclose(f_source);
	
		fprintf(stdout, "Uploading data from file %s...\n", filename);

		elf_hl_uploadRam(address, f_buf, f_size);
}

static void eo_downloadToFile(uint16_t address, uint16_t size) {
	uint8_t f_buf[0xFFFF];
	FILE *f_dest = fopen(filename, "wb+");

	if (f_dest == NULL) {
		perror(filename);
		return EXIT_FAILURE;
	}
		
	fprintf(stdout, "Downloading data to file %s...\n", filename);

	elf_hl_downloadRam(address, f_buf, size);

	if (fwrite(f_buf, size, 1, f_dest) != size) {
		perror(filename);
		fclose(f_dest);

		return EXIT_FAILURE;
	}

	fclose(filename);
}
