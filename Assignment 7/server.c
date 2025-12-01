#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>
#include <unistd.h>

int main() {
    int s;
    char buf[256];
    struct sockaddr_in addr, cli;
    socklen_t len;
    double x, y, res;

    s = socket(AF_INET, SOCK_DGRAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr*)&addr, sizeof(addr));

    while (1) {
        len = sizeof(cli);
        int n = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)&cli, &len);
        buf[n] = '\0';

        if (strstr(buf, "sin(")) {
            sscanf(buf, "sin(%lf)", &x);
            res = sin(x);
        } else if (strstr(buf, "cos(")) {
            sscanf(buf, "cos(%lf)", &x);
            res = cos(x);
        } else if (strstr(buf, "tan(")) {
            sscanf(buf, "tan(%lf)", &x);
            res = tan(x);
        } else if (strstr(buf, "+")) {
            sscanf(buf, "%lf+%lf", &x, &y);
            res = x + y;
        } else if (strstr(buf, "-")) {
            sscanf(buf, "%lf-%lf", &x, &y);
            res = x - y;
        } else if (strstr(buf, "*")) {
            sscanf(buf, "%lf*%lf", &x, &y);
            res = x * y;
        } else if (strstr(buf, "/")) {
            sscanf(buf, "%lf/%lf", &x, &y);
            res = y == 0 ? 0 : x / y;
        } else {
            res = 0;
        }

        char out[128];
        sprintf(out, "%lf", res);
        sendto(s, out, strlen(out), 0, (struct sockaddr*)&cli, len);
    }

    close(s);
    return 0;
}
