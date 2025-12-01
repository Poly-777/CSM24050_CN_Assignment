#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int fd;

void *recv_thread(void *p) {
    char buf[512];
    while (1) {
        int n = recv(fd, buf, sizeof(buf), 0);
        if (n > 0) {
            buf[n] = 0;
            printf("%s", buf);
        }
    }
    return NULL;
}

int main() {
    struct sockaddr_in addr;
    pthread_t t;
    char msg[512];

    fd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    addr.sin_addr.s_addr = inet_addr("10.0.0.1");

    connect(fd, (struct sockaddr*)&addr, sizeof(addr));

    pthread_create(&t, NULL, recv_thread, NULL);

    while (1) {
        fgets(msg, sizeof(msg), stdin);
        send(fd, msg, strlen(msg), 0);
    }

    close(fd);
    return 0;
}
