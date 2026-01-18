#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_TOPICS 3
#define TOPIC_SIZE 128

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int num_topics = 0;
    char** topics = malloc(sizeof(char*));
    char path[1024];
    char text[1024];

    printf("Enter configuration file path: ");
    fgets(path, 1024, stdin);
    path[strcspn(path, "\n")] = 0;
    
    FILE *config = fopen(path, "r");
    if (!config) {
        fprintf(stderr, "Error opening file: No such file or directory!\n");
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
        while(num_topics < 3) {
            char *aux = strchr(data, ',');
            if (aux)
                *aux = '\0';
            topics[num_topics] = strdup(data);
            num_topics++;
            if (!aux)
                break;
            data = ++aux;
        }   
    } else {
        fprintf(stderr, "Error: Configuration type mismatch. Expected 'subscriber', got '%s'!\n", data);
        exit(1);
    }

    fclose(config);

    for (int i = 0; i < num_topics; i++)
        printf("topic= %s\n", topics[i]);

    int sock;
    char buffer[1024];
    struct sockaddr_in subaddr, pubaddr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&subaddr, 0, sizeof(subaddr));
    memset(&pubaddr, 0, sizeof(pubaddr));

    subaddr.sin_family = AF_INET;
    subaddr.sin_port = htons(atoi(argv[1]));
    inet_pton(AF_INET, "127.0.0.1", &subaddr.sin_addr);

    bind(sock, (struct sockaddr *)&subaddr, sizeof(subaddr));
    
    printf("Subscriber esperando dados na porta %d...\n", atoi(argv[1]));

    while(1) {

        int len = sizeof(pubaddr); 
        int n;

        n = recvfrom(sock, (char *)buffer, 1024, 
                    MSG_WAITALL, (struct sockaddr *) &pubaddr, 
                    (socklen_t*)&len);
        
        buffer[n] = '\0';
        printf("Mensagem recebida: %s\n", buffer);

    }

    close(sock);
    return 0;
}