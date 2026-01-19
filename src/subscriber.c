#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define MAX_TOPICS 3
#define TOPIC_SIZE 128
#define TEXT_SIZE 1024

volatile bool subscriber_active = false; 
volatile bool running = true;

// ?
typedef struct {
    int sock;
} ThreadData;

void *thread_msgs(void *arg) {

    struct sockaddr_in pubaddr;
    memset(&pubaddr, 0, sizeof(pubaddr));

    ThreadData *data = (ThreadData *)arg;

    while (running) {
        
        char buffer[1024];
        
        int len = sizeof(pubaddr); 
        int n;

        n = recvfrom(data->sock, (char *)buffer, 1024, 
                    MSG_WAITALL, (struct sockaddr *) &pubaddr, 
                    (socklen_t*)&len);
        
        buffer[n] = '\0';
        printf("Msg: %s\n", buffer);
        
        if (subscriber_active) {
            // Verifica se ta inscrito e trata
            printf("ok\n");
        } else {
            // Ignora
            printf("continue\n");
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
    char** topics = malloc(sizeof(char*));
    char config_path[1024];
    char text[1024];

    printf("Enter configuration file path: ");
    fgets(config_path, 1024, stdin);
    config_path[strcspn(config_path, "\n")] = 0;
    
    FILE *config = fopen(config_path, "r");
    if (!config) {
        perror("Error opening file");
        exit(1);
    }

    fgets(text, 1024, config);
    char* data = strchr(text, ':');
    data++;
    data[strcspn(data, "\n")] = 0;

    if (!strcmp("subscriber", data)) {
        fgets(text, 1024, config);
        data = strchr(text, ':');
        data++;
        char *aux;
        while(num_topics < MAX_TOPICS) {
            aux = strchr(data, ',');
            if (aux)
                *aux = '\0';
            topics[num_topics] = strdup(data);
            num_topics++;
            if (!aux)
                break;
            data = ++aux;
        } 
        if (aux)
            printf("File contains more than 3 topics. Excess topics skipped.\n");

    } else {
        fprintf(stderr, "Configuration type mismatch. Expected 'subscriber', got '%s'!\n", data);
        exit(1);
    }

    fclose(config);

    for (int i = 0; i < num_topics; i++)
        printf("topic= %s\n", topics[i]);

    int sock;
    struct sockaddr_in subaddr;
    memset(&subaddr, 0, sizeof(subaddr));

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    subaddr.sin_family = AF_INET;
    subaddr.sin_port = htons(atoi(argv[1]));
    inet_pton(AF_INET, "127.0.0.1", &subaddr.sin_addr);

    if ((bind(sock, (struct sockaddr *)&subaddr, sizeof(subaddr))) < 0) {
        perror("Failed to bind socket");
        exit(1);
    }

    pthread_t id_thread;
    ThreadData *args = malloc(sizeof(ThreadData));
    args->sock = sock;

    if (pthread_create(&id_thread, NULL, thread_msgs, (void *)args) != 0) {
        perror("Failed to create thread");
        return 1;
    }

    int option = 0;

    while (running) {
        printf("\n--- SUBSCRIBER MENU ---\n");
        printf("Current status: %s\n", subscriber_active ? "ACTIVE" : "INACTIVE");
        printf("1. Activate\n");
        printf("2. Desactivate\n");
        printf("3. Exit\n");
        printf("Choice: ");
        
        scanf("%d", &option);

        switch (option) {
            case 1:
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