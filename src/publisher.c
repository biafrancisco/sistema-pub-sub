#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5000

int main() {

    int sock;
    //char* msg = malloc(1024 * sizeof(char));
    char msg[1024];
    struct sockaddr_in subaddr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&subaddr, 0, sizeof(subaddr));

    subaddr.sin_family = AF_INET;
    subaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &subaddr.sin_addr);

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