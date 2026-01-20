#ifndef _UTILS_
#define _UTILS_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define TEXT_SIZE 1024

FILE *open_config_file();

char *get_config_role(FILE *config_file);

void map_topic_to_filepath(char path[], char topic[]);

int setup_udp_socket(const char *ip, int port, struct sockaddr_in *addr);

void get_current_status(char path[], char current_status[]);


#endif