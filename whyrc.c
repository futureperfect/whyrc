#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_PENDING         5
#define MESSAGE_BUFFER_SIZE 512

void handle_client(int socket);

int main(int argc, char* argv[])
{
    unsigned short port;
    int sock;
    int client_sock;

    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [port]\n", argv[0]);
        exit(1);
    }

    port = atoi(argv[1]);

    if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        perror("socket() failed");
        exit(1);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port);

    if(bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("bind() failed");
        exit(1);
    }

    if(listen(sock, MAX_PENDING) < 0)
    {
        perror("listen() failed");
        exit(1);
    }

    printf("Connecting on port %d\n", port);

    for(;;)
    {
        socklen_t clientLen = sizeof(clientAddress);
        if((client_sock = accept(sock, (struct sockaddr *) &clientAddress, &clientLen)) < 0)
        {
            perror("accept() failed");
            exit(1);
        }
        handle_client(client_sock);
    }

    return 0;
}

void handle_client(int client_sock)
{
    char buffer[MESSAGE_BUFFER_SIZE];
    int receivedMessageSize;

    if((receivedMessageSize = recv(client_sock, buffer, MESSAGE_BUFFER_SIZE, 0)) < 0)
    {
        perror("recv() failed");
        exit(1);
    }

    while(receivedMessageSize > 0)
    {
        if(send(client_sock, buffer, receivedMessageSize, 0) != receivedMessageSize)
        {
            perror("send() failed");
            exit(1);
        }

        if((receivedMessageSize = recv(client_sock, buffer, MESSAGE_BUFFER_SIZE, 0)) < 0)
        {
            perror("recv()  failed");
            exit(1);
        }
    }

    close(client_sock);
}

