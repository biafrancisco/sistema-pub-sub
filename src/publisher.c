#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <arpa/inet.h>

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

    printf("Enter configuration file path: ");
    fgets(config_path, TEXT_SIZE, stdin);
    config_path[strcspn(config_path, "\n")] = 0;
    
    FILE *config_file = fopen(config_path, "r");
    if (!config_file) {
        perror("Error opening file");
        exit(1);
    }

    fgets(text, TEXT_SIZE, config_file);
    char* data = strchr(text, ':');
    data++;
    data[strcspn(data, "\n")] = 0;

    if (!strcmp("publisher", data)) {
        fgets(text, TEXT_SIZE, config_file);
        data = strchr(text, ':');
        data++;
        strncpy(topic, data, TOPIC_SIZE);
    } else {
        fprintf(stderr, "Error: Configuration type mismatch. Expected 'publisher', got '%s'!\n", data);
        exit(1);
    }

    fclose(config_file);


    int sock;
    char msg[MSG_SIZE];
    struct sockaddr_in subaddr;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    memset(&subaddr, 0, sizeof(subaddr));
    subaddr.sin_family = AF_INET;
    subaddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &subaddr.sin_addr);


    char status_path[TEXT_SIZE];
    sprintf(status_path, "../tmp/status%s.txt", topic);

    for (int i = 14; status_path[i] != '\0'; i++) {
        if (status_path[i] == '/')
            status_path[i] = '-';
    }

    char current_status[TEXT_SIZE] = "";

    FILE *status_file = fopen(status_path, "r");
    if (status_file) {
        fgets(current_status, TEXT_SIZE, status_file);
        fclose(status_file);
    }

    //printf("%s\n", status_path);
    bool running = true;
    int option;

    while (running) {
        printf("\n--- PUBLISHER MENU ---\n");
        printf("Topic '%s' status: '%s'\n", topic, current_status ? current_status : "Unknown");
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

                status_file = fopen(status_path, "w");
                fprintf(status_file, "%s", text);
                fclose(status_file);

                printf("Topic %s changed from %s to %s\n", topic, current_status, text);
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