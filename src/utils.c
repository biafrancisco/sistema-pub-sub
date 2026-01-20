#include "utils.h"

FILE *open_config_file() {

    char config_path[TEXT_SIZE];

    printf("Enter configuration file path: ");
    fgets(config_path, TEXT_SIZE, stdin);
    config_path[strcspn(config_path, "\n")] = 0;
    
    FILE *config_file = fopen(config_path, "r");
    if (!config_file) {
        perror("Error opening file");
        exit(1);
    }

    return config_file;
}

char *get_config_role(FILE *config_file) {

    char text[TEXT_SIZE];
    fgets(text, TEXT_SIZE, config_file);

    char* role = strchr(text, ':');
    role++;
    role[strcspn(role, "\n")] = 0;

    return role;
}

int setup_udp_socket(const char *ip, int port, struct sockaddr_in *addr) {

    int sock;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failed to create socket");
        exit(-1); 
    }

    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(-1);
    }

    return sock;
}

void map_topic_to_filepath(char path[], char topic[]) {
    
    sprintf(path, "../tmp/status%s.txt", topic);

    for (int i = 14; path[i] != '\0'; i++) {
        if (path[i] == '/')
            path[i] = '-';
    }
}

void get_current_status(char path[], char current_status[]) {
    
    FILE *status_file = fopen(path, "r");
    
    if (status_file) {
        fgets(current_status, TEXT_SIZE, status_file);
        fclose(status_file);
    } else {
        strcpy(current_status, "Unknown");
    }
}