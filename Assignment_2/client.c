#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char sendData[100], recvData[300];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);
    serverAddr.sin_addr.s_addr = inet_addr("10.0.0.1");

    connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    char fruit[50];
    int qty;
    printf("Enter fruit name: ");
    scanf("%s", fruit);
    printf("Enter quantity: ");
    scanf("%d", &qty);

    sprintf(sendData, "%s %d", fruit, qty);
    send(clientSocket, sendData, strlen(sendData), 0);

    recv(clientSocket, recvData, sizeof(recvData), 0);
    printf("%s", recvData);

    close(clientSocket);
    return 0;
}
