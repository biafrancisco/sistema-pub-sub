#include <unistd.h>
#include <stdbool.h>
#include "utils.h"
#include "logger.h"

#define TOPIC_SIZE 128
#define TEXT_SIZE 1024
#define MSG_SIZE (TOPIC_SIZE + TEXT_SIZE + 2)
#define LOG_SIZE (3 * TEXT_SIZE) 

#define LOG_PATH "./tmp/log/pub_log.txt"

int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    char topic[TOPIC_SIZE];
    char text[TEXT_SIZE];
    char log_msg[LOG_SIZE];

    // Initialize application configuration
    FILE *config_file = open_config_file(LOG_PATH);

    char *role = get_config_role(config_file);

    // Validate the role
    if (!strcmp("publisher", role)) {
        // Get topic
        fgets(text, TEXT_SIZE, config_file);
        char *data = strchr(text, ':');
        data++;
        strncpy(topic, data, TOPIC_SIZE);
    } else {
        sprintf(log_msg, "Configuration type mismatch. Expected 'publisher', got '%s'!", role);
        log_message(LOG_PATH, "INFO", log_msg);
        exit(1);
    }

    fclose(config_file);

    // Configure socket
    struct sockaddr_in subaddr;
    int sock = setup_udp_socket(argv[1], atoi(argv[2]), &subaddr, LOG_PATH);
    char msg[MSG_SIZE];

    // Map status file path
    char status_path[TEXT_SIZE];
    map_topic_to_filepath(status_path, topic);

    // Get current aplication status
    char current_status[TEXT_SIZE];
    get_current_status(status_path, current_status);

    bool running = true;
    int option;

    log_message(LOG_PATH, "LOG", "\nPublisher initialized successfully!");

    while (running) {
        // Show menu options
        printf("\n--- PUBLISHER MENU ---\n");
        printf("Topic %s status: %s\n", topic, current_status);
        printf("1. Configure device\n");
        printf("2. Exit\n");
        printf("Choice: ");
        
        scanf("%d", &option);
        getchar();

        switch (option) {
    
            case 1: // Configure device
                // Get user input
                printf("New device status: ");
                fgets(text, TEXT_SIZE, stdin);
                text[strcspn(text, "\n")] = 0;

                // Update status file
                FILE *status_file = fopen(status_path, "w");
                fprintf(status_file, "%s", text);
                fclose(status_file);

                // Print log message
                sprintf(log_msg, "\nTopic %s changed from %s to %s", topic, current_status, text);
                log_message(LOG_PATH, "INFO", log_msg);
                strcpy(current_status, text);

                // Send UDP message to potential subscriber
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
    
    log_message(LOG_PATH, "LOG", "\nPublisher shutdown complete\n\n");

    return 0;
}