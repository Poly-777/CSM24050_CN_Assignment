#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

struct Fruit {
    char name[50];
    int quantity;
};

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen;
    char recvData[100], sendData[200];

    struct Fruit fruits[3] = {
        {"apple", 10},
        {"mango", 8},
        {"banana", 15}
    };

    char customers[100][100];
    int customerCount = 0;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);

    while(1) {
        addrLen = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);

        int exists = 0;
        char idStr[100];
        sprintf(idStr, "%s:%d", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        for(int i=0; i<customerCount; i++) {
            if(strcmp(customers[i], idStr) == 0)
                exists = 1;
        }
        if(!exists) {
            strcpy(customers[customerCount], idStr);
            customerCount++;
        }

        recv(clientSocket, recvData, sizeof(recvData), 0);

        char fruitName[50];
        int reqQty;
        sscanf(recvData, "%s %d", fruitName, &reqQty);

        int found = 0;
        for(int i=0; i<3; i++) {
            if(strcmp(fruits[i].name, fruitName) == 0) {
                found = 1;
                if(fruits[i].quantity >= reqQty) {
                    fruits[i].quantity -= reqQty;
                    sprintf(sendData,
                        "Purchase successful.\nRemaining %s: %d\nTotal unique customers: %d\n",
                        fruits[i].name, fruits[i].quantity, customerCount
                    );
                } else {
                    sprintf(sendData,
                        "Regret: Only %d %s left. Cannot process request.\nTotal unique customers: %d\n",
                        fruits[i].quantity, fruits[i].name, customerCount
                    );
                }
            }
        }

        if(!found) {
            sprintf(sendData, "Fruit not found.\nTotal unique customers: %d\n", customerCount);
        }

        send(clientSocket, sendData, strlen(sendData), 0);
        close(clientSocket);
    }

    close(serverSocket);
    return 0;
}
