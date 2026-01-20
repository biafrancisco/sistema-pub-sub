#include "logger.h"

void log_message(char *path, const char *level, const char *message) {
    
    // Append to log file
    FILE *file = fopen(path, "a"); 
    if (file != NULL) {
        fprintf(file, "%s\n", message);
        fclose(file); 
    } else {
        perror("Error opening log file");
    }

    // Print to stdout or stderr
    if(strcmp(level, "LOG")) {
        if (!strcmp(level, "ERROR")) {
            perror(message);
        } else {
            printf("%s\n", message);
        }
    }
}