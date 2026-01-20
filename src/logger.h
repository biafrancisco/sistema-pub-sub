#ifndef _LOGGER_
#define _LOGGER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void log_message(char *path, const char *level, const char *message);

#endif