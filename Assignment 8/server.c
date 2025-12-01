#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>

int clients[100];
int count = 0;
pthread_mutex_t lock;

void *handle(void *p) {
    int fd = *(int*)p;
    char msg[512];
    FILE *f;

    while (1) {
        int n = recv(fd, msg, sizeof(msg), 0);
        if (n <= 0) break;
        msg[n] = 0;

        pthread_mutex_lock(&lock);
        f = fopen("log.txt", "a");
        time_t t = time(NULL);
        fprintf(f, "[%s] %s", ctime(&t), msg);
        fclose(f);

        for (int i = 0; i < count; i++) {
            send(clients[i], msg, strlen(msg), 0);
        }
        pthread_mutex_unlock(&lock);
    }

    close(fd);
    return NULL;
}

int main() {
    int s, newfd;
    struct sockaddr_in addr, cli;
    socklen_t cl;
    pthread_t th;

    pthread_mutex_init(&lock, NULL);

    s = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr*)&addr, sizeof(addr));
    listen(s, 10);

    while (1) {
        cl = sizeof(cli);
        newfd = accept(s, (struct sockaddr*)&cli, &cl);

        clients[count++] = newfd;
        pthread_create(&th, NULL, handle, (void*)&newfd);
    }

    close(s);
    return 0;
}
