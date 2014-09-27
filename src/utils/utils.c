#include "utils/utils.h"

#include <time.h>

int util_sleep(long usec) {
	struct timespec sleep_time;
 
	sleep_time.tv_sec = usec / 1000000;
	sleep_time.tv_nsec = (usec % 1000000) * 1000;
 
	return nanosleep(&sleep_time, NULL);
}
