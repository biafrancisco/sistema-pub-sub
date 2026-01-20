#ifndef _UTILS_
#define _UTILS_

#include "logger.h"
#include <arpa/inet.h>

#define TEXT_SIZE 1024
#define LOG_SIZE (3 * TEXT_SIZE) 

FILE *open_config_file(char *log_path);

char *get_config_role(FILE *config_file);

void map_topic_to_filepath(char path[], char topic[]);

int setup_udp_socket(const char *ip, int port, struct sockaddr_in *addr, char *log_path);

void get_current_status(char path[], char current_status[]);


#endif