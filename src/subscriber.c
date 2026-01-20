#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "utils.h"
#include "logger.h"

#define MAX_TOPICS 3
#define TOPIC_SIZE 128
#define TEXT_SIZE 1024

volatile bool subscriber_active = false; 
volatile bool running = true;
char **current_status;

typedef struct {
    int sock;
    int num_topics;
    char **topics;
} ThreadData;

void print_menu();

char **get_topics(int *num_topics, FILE *config_file);

void *thread_msgs(void *arg) {

    struct sockaddr_in pubaddr;
    memset(&pubaddr, 0, sizeof(pubaddr));

    ThreadData *thread_data = (ThreadData *)arg;

    while (running) {
        
        char buffer[1024];

        int len = sizeof(pubaddr); 
        int n = recvfrom(thread_data->sock, (char *)buffer, 1024, 
                MSG_WAITALL, (struct sockaddr *) &pubaddr, (socklen_t*)&len);
        buffer[n] = '\0';

        
        if (subscriber_active) {
            char * data = strchr(buffer, ':');
            *data = '\0';
            data++;

            int id = 0;
            while(id < thread_data->num_topics) {
                if (!strcmp(buffer, thread_data->topics[id]))
                    break;
                id++;
            }

            if (id < thread_data->num_topics) {
                printf("\n\nTopic %s changed from %s to %s\n", thread_data->topics[id], current_status[id], data);
                strcpy(current_status[id], data);   
                print_menu();
            }
        }
    }  
    return NULL;
}

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int num_topics = 0;
    char config_path[1024];
    char text[1024];

    FILE *config_file = open_config_file();

    char *role = get_config_role(config_file);

    if (strcmp("subscriber", role)) {
        fprintf(stderr, "Configuration type mismatch. Expected 'subscriber', got '%s'!\n", role);
        exit(1);
    }

    char **topics = get_topics(&num_topics, config_file);

    fclose(config_file);

    char status_path[num_topics][TEXT_SIZE];
    current_status = (char**) malloc(num_topics * sizeof(char*));

    for (int i = 0; i < num_topics; i++) {
        map_topic_to_filepath(status_path[i], topics[i]);
        current_status[i] = (char*) malloc(TEXT_SIZE * sizeof(char));
        printf("topic= %s\n", topics[i]);
        //printf("status= %s\n", status_path[i]);
    }

    struct sockaddr_in subaddr;
    int sock = setup_udp_socket("127.0.0.1", atoi(argv[1]), &subaddr);

    if ((bind(sock, (struct sockaddr *)&subaddr, sizeof(subaddr))) < 0) {
        perror("Failed to bind socket");
        exit(1);
    }

    pthread_t id_thread;
    ThreadData args;
    args.sock = sock;
    args.num_topics = num_topics;
    args.topics = topics;

    if (pthread_create(&id_thread, NULL, thread_msgs, (void *)&args) != 0) {
        perror("Failed to create thread");
        exit(1);
    }

    int option = 0;

    while (running) {
        
        print_menu();

        scanf("%d", &option);

        switch (option) {
            case 1:
                for (int i = 0; i < num_topics; i++) 
                    get_current_status(status_path[i], current_status[i]);

                subscriber_active = true;
                printf(">> Subscriber ACTIVATED.\n");

                break;
            case 2:
                subscriber_active = false;
                printf(">> Subscriber DESACTIVATED.\n");
                break;
            case 3:
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

void print_menu() {
    printf("\n--- SUBSCRIBER MENU ---\n");
    printf("Current status: %s\n", subscriber_active ? "ACTIVE" : "INACTIVE");
    printf("1. Activate\n");
    printf("2. Desactivate\n");
    printf("3. Exit\n");
    printf("Choice: ");
    fflush(stdout);
}

char **get_topics(int *num_topics, FILE *config_file) {
    
    char **topics = (char **)malloc(sizeof(char*));
    
    char text[TEXT_SIZE];
    fgets(text, 1024, config_file);

    char *data = strchr(text, ':');
    data++;

    char *aux;
    while(*num_topics < MAX_TOPICS) {
        aux = strchr(data, ',');
        if (aux)
            *aux = '\0';
        topics[*num_topics] = strdup(data);
        (*num_topics)++;
        if (!aux)
            break;
        topics = (char**) realloc(topics, (*num_topics + 1) * sizeof(char*));
        data = ++aux;
    } 
    if (aux)
        printf("File contains more than 3 topics. Excess topics skipped.\n");

    return topics;
}