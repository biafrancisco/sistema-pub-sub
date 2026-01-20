#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include "utils.h"
#include "logger.h"

#define TOPIC_SIZE 128
#define TEXT_SIZE 1024
#define MSG_SIZE (TOPIC_SIZE + TEXT_SIZE + 2)

int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    char topic[TOPIC_SIZE];
    char config_path[TEXT_SIZE];
    char text[TEXT_SIZE];

    FILE *config_file = open_config_file();

    char *role = get_config_role(config_file);

    if (!strcmp("publisher", role)) {
        fgets(text, TEXT_SIZE, config_file);
        char *data = strchr(text, ':');
        data++;
        strncpy(topic, data, TOPIC_SIZE);
    } else {
        fprintf(stderr, "Error: Configuration type mismatch. Expected 'publisher', got '%s'!\n", role);
        exit(1);
    }

    fclose(config_file);

    char msg[MSG_SIZE];
    struct sockaddr_in subaddr;
    int sock = setup_udp_socket(argv[1], atoi(argv[2]), &subaddr);

    char status_path[TEXT_SIZE];
    map_topic_to_filepath(status_path, topic);

    char current_status[TEXT_SIZE];
    get_current_status(status_path, current_status);

    bool running = true;
    int option;

    while (running) {
        printf("\n--- PUBLISHER MENU ---\n");
        printf("Topic %s status: %s\n", topic, current_status);
        printf("1. Configure device\n");
        printf("2. Exit\n");
        printf("Choice: ");
        
        scanf("%d", &option);
        getchar();

        switch (option) {
    
            case 1:
                printf("New device status: ");
                fgets(text, TEXT_SIZE, stdin);
                text[strcspn(text, "\n")] = 0;

                FILE *status_file = fopen(status_path, "w");
                fprintf(status_file, "%s", text);
                fclose(status_file);

                printf("\nTopic %s changed from %s to %s\n", topic, current_status, text);
                strcpy(current_status, text);

                memset(msg, 0, sizeof(msg));
                snprintf(msg, sizeof(msg), "%s:%s", topic, text);
                sendto(sock, (char *)msg, sizeof(msg), 
                        MSG_CONFIRM, (struct sockaddr *) &subaddr, 
                        sizeof(subaddr));

                break;

            case 2:
                printf("Exiting...\n");
                running = false; 
                break;

            default:
                printf("Invalid option.\n");
                break;
        }
    }

    close(sock);
    return 0;
}