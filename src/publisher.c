#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    char* topic;
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

    if (!strcmp("publisher", data)) {
        fgets(text, 1024, config);
        data = strchr(text, ':');
        data++;
        topic = strdup(data);
    } else {
        fprintf(stderr, "Error: Configuration type mismatch. Expected 'publisher', got '%s'!\n", data);
        exit(1);
    }

    printf("topic= %s\n", topic);

    fclose(config);

    int sock;
    char msg[1024];
    struct sockaddr_in subaddr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&subaddr, 0, sizeof(subaddr));

    subaddr.sin_family = AF_INET;
    subaddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &subaddr.sin_addr);

    while(1) {

        printf("Enviar: ");
        fgets(msg, 1024, stdin);
        //getchar();
        sendto(sock, (char *)msg, strlen(msg), 
            MSG_CONFIRM, (struct sockaddr *) &subaddr, 
            sizeof(subaddr));

        printf("Mensagem enviada\n\n");

    }

    close(sock);
    return 0;
}