#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5000

int main() {

    int sock;
    char buffer[1024];
    struct sockaddr_in subaddr, pubaddr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&subaddr, 0, sizeof(subaddr));
    memset(&pubaddr, 0, sizeof(pubaddr));

    subaddr.sin_family = AF_INET;
    subaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &subaddr.sin_addr);

    bind(sock, (struct sockaddr *)&subaddr, sizeof(subaddr));
    
    printf("Subscriber esperando dados na porta %d...\n", PORT);

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