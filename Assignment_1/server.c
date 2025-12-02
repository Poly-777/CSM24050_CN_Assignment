#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int serverSocket, clientSocket;
    char msg[100];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 1);

    addrLen = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);

    recv(clientSocket, msg, sizeof(msg), 0);
    printf("Client says: %s\n", msg);

    send(clientSocket, "Hello", strlen("Hello") + 1, 0);

    close(clientSocket);
    close(serverSocket);
    return 0;
}
