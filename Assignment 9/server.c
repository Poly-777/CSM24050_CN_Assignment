#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>

int main() {
    int s, c;
    struct sockaddr_in addr, cli;
    socklen_t cl;
    char buf[1024];

    s = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr*)&addr, sizeof(addr));
    listen(s, 5);

    while (1) {
        cl = sizeof(cli);
        c = accept(s, (struct sockaddr*)&cli, &cl);

        int mode;
        recv(c, &mode, sizeof(mode), 0);

        if (mode == 1) {
            FILE *f = fopen("server_file.txt", "rb");
            clock_t start = clock();
            while (1) {
                int n = fread(buf, 1, sizeof(buf), f);
                if (n <= 0) break;
                send(c, buf, n, 0);
            }
            clock_t end = clock();
            printf("File sent in %lf seconds\n", (double)(end - start)/CLOCKS_PER_SEC);
            fclose(f);
        } else if (mode == 2) {
            FILE *f = fopen("uploaded_file.txt", "wb");
            clock_t start = clock();
            int n;
            while ((n = recv(c, buf, sizeof(buf), 0)) > 0) {
                fwrite(buf, 1, n, f);
                if (n < sizeof(buf)) break;
            }
            clock_t end = clock();
            printf("File received in %lf seconds\n", (double)(end - start)/CLOCKS_PER_SEC);
            fclose(f);
        }

        close(c);
    }

    close(s);
    return 0;
}
