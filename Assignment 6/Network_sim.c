#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int s;
    char buf[2048];
    struct sockaddr_in addr;
    socklen_t len;

    s = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);

    addr.sin_family = AF_INET;
    addr.sin_port = 0;
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr*)&addr, sizeof(addr));

    while (1) {
        len = sizeof(addr);
        int n = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)&addr, &len);
        if (n > 0) {
            printf("Got TCP packet, size %d bytes\n", n);

            unsigned short src = (buf[0] << 8) | buf[1];
            unsigned short dst = (buf[2] << 8) | buf[3];

            printf("Src Port: %u\n", src);
            printf("Dst Port: %u\n", dst);

            int header = (buf[12] >> 4) * 4;
            printf("Data size: %d\n", n - header);
        }
    }

    close(s);
    return 0;
}
