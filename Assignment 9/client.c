#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>

int main() {
    int s;
    struct sockaddr_in addr;
    char buf[1024];

    s = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    addr.sin_addr.s_addr = inet_addr("10.0.0.1");

    connect(s, (struct sockaddr*)&addr, sizeof(addr));

    int mode = 1;
    send(s, &mode, sizeof(mode), 0);
    FILE *f = fopen("downloaded_file.txt", "wb");
    clock_t start = clock();
    int n;
    while ((n = recv(s, buf, sizeof(buf), 0)) > 0) {
        fwrite(buf, 1, n, f);
        if (n < sizeof(buf)) break;
    }
    clock_t end = clock();
    printf("File downloaded in %lf seconds\n", (double)(end - start)/CLOCKS_PER_SEC);
    fclose(f);

    mode = 2;
    send(s, &mode, sizeof(mode), 0);
    f = fopen("client_file.txt", "rb");
    start = clock();
    while (1) {
        n = fread(buf, 1, sizeof(buf), f);
        if (n <= 0) break;
        send(s, buf, n, 0);
    }
    end = clock();
    printf("File uploaded in %lf seconds\n", (double)(end - start)/CLOCKS_PER_SEC);
    fclose(f);

    close(s);
    return 0;
}
