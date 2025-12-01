#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

unsigned short csum(unsigned short *ptr,int nbytes) {
    long sum;
    unsigned short oddbyte;
    short answer;

    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }
    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
    return(answer);
}

int main() {
    int s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    char packet[1024];
    struct icmphdr *icmp = (struct icmphdr *) packet;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("10.0.0.2");

    memset(packet, 0, 1024);
    icmp->type = 13;
    icmp->code = 0;
    icmp->un.timestamp.originate_timestamp = 0;
    icmp->un.timestamp.receive_timestamp = 0;
    icmp->un.timestamp.transmit_timestamp = 0;
    icmp->checksum = csum((unsigned short *)icmp, sizeof(struct icmphdr));

    sendto(s, packet, sizeof(struct icmphdr), 0, (struct sockaddr*)&addr, sizeof(addr));

    close(s);
    return 0;
}
