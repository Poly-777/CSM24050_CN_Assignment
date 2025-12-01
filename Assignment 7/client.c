#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int c;
    char expr[256], ans[256];
    struct sockaddr_in addr;
    socklen_t len;

    c = socket(AF_INET, SOCK_DGRAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("10.0.0.1");

    printf("Enter expression: ");
    fgets(expr, sizeof(expr), stdin);

    sendto(c, expr, strlen(expr), 0, (struct sockaddr*)&addr, sizeof(addr));

    len = sizeof(addr);
    int n = recvfrom(c, ans, sizeof(ans), 0, (struct sockaddr*)&addr, &len);
    ans[n] = '\0';

    printf("Result: %s\n", ans);

    close(c);
    return 0;
}
