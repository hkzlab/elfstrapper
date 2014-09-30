#ifndef _LOGGER_DEFINE_
#define _LOGGER_DEFINE_

typedef enum {
	LOGR_NONE = 0,
	LOGR_BASE = 1,
	LOGR_EXTN = 2,
	LOGR_VERB = 3
} logging_verbosity;

// Set logging verbosity: that is, from which level downwards the messages are printed
void logger_setVerbosity(logging_verbosity level);
// Print a message via the logging facility (either using syslog or printf, depending on
// the daemonization status of the process). "level" indicates the verbosity of this print.
void logger_print(logging_verbosity level, const char* format, ...);

#endif /* _LOGGER_DEFINE_ */
