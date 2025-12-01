#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/ethernet.h>

#define BUFFER_SIZE 65536

void analyze_packet(unsigned char* buffer, int size) {
    struct ethhdr *eth = (struct ethhdr*)buffer;
    
    if (ntohs(eth->h_proto) == ETH_P_IP) {
        struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
        
        printf("Ethernet Header\n");
        printf("   |-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",
               eth->h_source[0], eth->h_source[1], eth->h_source[2],
               eth->h_source[3], eth->h_source[4], eth->h_source[5]);
        printf("   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",
               eth->h_dest[0], eth->h_dest[1], eth->h_dest[2],
               eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
        printf("   |-Protocol : %u\n", ntohs(eth->h_proto));
        
        printf("\nIP Header\n");
        printf("   |-Version : %d\n", (unsigned int)iph->version);
        printf("   |-Internet Header Length : %d DWORDS or %d Bytes\n",
               (unsigned int)iph->ihl, ((unsigned int)(iph->ihl))*4);
        printf("   |-Type Of Service : %d\n", (unsigned int)iph->tos);
        printf("   |-Total Length : %d Bytes\n", ntohs(iph->tot_len));
        printf("   |-Identification : %d\n", ntohs(iph->id));
        printf("   |-Time To Live : %d\n", (unsigned int)iph->ttl);
        printf("   |-Protocol : %d\n", (unsigned int)iph->protocol);
        printf("   |-Header Checksum : %d\n", ntohs(iph->check));
        printf("   |-Source IP : %s\n", inet_ntoa(*(struct in_addr*)&iph->saddr));
        printf("   |-Destination IP : %s\n", inet_ntoa(*(struct in_addr*)&iph->daddr));
        
        if (iph->protocol == IPPROTO_TCP) {
            struct tcphdr *tcph = (struct tcphdr*)(buffer + sizeof(struct ethhdr) + (iph->ihl*4));
            
            printf("\nTCP Header\n");
            printf("   |-Source Port : %u\n", ntohs(tcph->source));
            printf("   |-Destination Port : %u\n", ntohs(tcph->dest));
            printf("   |-Sequence Number : %u\n", ntohl(tcph->seq));
            printf("   |-Acknowledge Number : %u\n", ntohl(tcph->ack_seq));
            printf("   |-Header Length : %d DWORDS or %d BYTES\n",
                   (unsigned int)tcph->doff, (unsigned int)tcph->doff*4);
            printf("   |-Urgent Flag : %d\n", (unsigned int)tcph->urg);
            printf("   |-Acknowledgement Flag : %d\n", (unsigned int)tcph->ack);
            printf("   |-Push Flag : %d\n", (unsigned int)tcph->psh);
            printf("   |-Reset Flag : %d\n", (unsigned int)tcph->rst);
            printf("   |-Synchronise Flag : %d\n", (unsigned int)tcph->syn);
            printf("   |-Finish Flag : %d\n", (unsigned int)tcph->fin);
            printf("   |-Window : %d\n", ntohs(tcph->window));
            printf("   |-Checksum : %d\n", ntohs(tcph->check));
            printf("   |-Urgent Pointer : %d\n", tcph->urg_ptr);
        }
    }
}

int main() {
    int sock_raw;
    struct sockaddr_in saddr;
    unsigned char *buffer = (unsigned char *)malloc(BUFFER_SIZE);
    
    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_raw < 0) {
        perror("Socket Error");
        return 1;
    }
    
    printf("Packet Analyzer Started...\n");
    
    while (1) {
        int data_size = recvfrom(sock_raw, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (data_size < 0) {
            printf("Recvfrom error, failed to get packets\n");
            return 1;
        }
        
        analyze_packet(buffer, data_size);
        printf("\n----------------------------------------\n");
    }
    
    close(sock_raw);
    free(buffer);
    return 0;
}
