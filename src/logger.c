#include "logger.h"

void log_message(char *path, const char *level, const char *message) {
    
    FILE *file = fopen(path, "a"); 
    if (file != NULL) {
        fprintf(file, "[%s] %s\n", level, message);
        fclose(file); 
    } else {
        perror("Error opening log file");
    }

    if (strcmp(level, "ERROR") == 0) {
        fprintf(stderr, "[%s] %s\n", level, message);
    } else {
        printf("[%s] %s\n", level, message);
    }
}