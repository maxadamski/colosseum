#include "utils.h"

i64 deltatime(timespec t0, timespec t1) {
	return (t1.tv_sec - t0.tv_sec) * 1e9 + (t1.tv_nsec - t0.tv_nsec);
}

timespec gettime() {
	timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	return t;
}

