#include "common/defines.h"
#include "logger.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include <syslog.h>

#define PRINT_HEADER "[ELFStrapper] "
#define STR_BUFLEN 256


static logging_verbosity cur_log_level = LOGR_BASE;
static char *log_names[4] = {"[NONE] ", "[BASE] ", "[EXTN] ", "[VERB] "};

static char strbuf[STR_BUFLEN];

void logger_setVerbosity(logging_verbosity level) {
	cur_log_level = level;
}


void logger_print(logging_verbosity level, const char* format, ...) {
	if (level > cur_log_level) return; // Do nothing...

	int head_len = strlen(PRINT_HEADER);
	int log_len = strlen(log_names[level]);
	memcpy(strbuf, PRINT_HEADER, head_len);
	memcpy(strbuf + strlen(PRINT_HEADER), log_names[level], log_len);

	va_list args;
	va_start(args, format);
		vsnprintf(strbuf + head_len + log_len, STR_BUFLEN - head_len - log_len - 1, format, args);
	va_end(args);

	fprintf(stdout, "%s", strbuf);
	fflush(stdout);
}
