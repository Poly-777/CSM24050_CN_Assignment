#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
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
    int s = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    char packet[4096];
    struct iphdr *ip = (struct iphdr *) packet;
    struct tcphdr *tcp = (struct tcphdr *) (packet + sizeof(struct iphdr));
    char *data = packet + sizeof(struct iphdr) + sizeof(struct tcphdr);
    strcpy(data, "ROLLNUMBER123");

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr("10.0.0.2");

    memset(packet, 0, 4096);

    ip->ihl = 5;
    ip->version = 4;
    ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr) + strlen(data));
    ip->ttl = 64;
    ip->protocol = IPPROTO_TCP;
    ip->saddr = inet_addr("10.0.0.1");
    ip->daddr = addr.sin_addr.s_addr;
    ip->check = csum((unsigned short *)ip, sizeof(struct iphdr));

    tcp->source = htons(1234);
    tcp->dest = htons(80);
    tcp->seq = 0;
    tcp->ack_seq = 0;
    tcp->doff = 5;
    tcp->syn = 1;
    tcp->check = 0;

    sendto(s, packet, sizeof(struct iphdr) + sizeof(struct tcphdr) + strlen(data), 0, (struct sockaddr*)&addr, sizeof(addr));

    close(s);
    return(0);
}
