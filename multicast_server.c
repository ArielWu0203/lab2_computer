/* Send Multicast Datagram code example. */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct in_addr localInterface;
struct sockaddr_in groupSock;
int sd;
char databuf[1024] = "Multicast test message.";
int datalen = sizeof(databuf);
 
int main (int argc, char *argv[ ])
{
/* Create a datagram socket on which to send. */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
	{
	  perror("Opening datagram socket error");
	  exit(1);
	}
	else
	  printf("Opening the datagram socket...OK.\n");
	 
	/* Initialize the group sockaddr structure with a */
	/* group address of 226.1.1.1 and port 4321. */
	memset((char *) &groupSock, 0, sizeof(groupSock));
	groupSock.sin_family = AF_INET;
	groupSock.sin_addr.s_addr = inet_addr("226.1.1.1");
	groupSock.sin_port = htons(4321);
	 
	/* Set local interface for outbound multicast datagrams. */
	/* The IP address specified must be associated with a local, */
	/* multicast capable interface. */
	localInterface.s_addr = inet_addr("127.0.0.1");
	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
	{
	  perror("Setting local interface error");
	  exit(1);
	}
	else
	  printf("Setting the local interface...OK\n");
	/* Send a message to the multicast group specified by the*/
	/* groupSock sockaddr structure. */
	/*int datalen = 1024;*/
    strcpy(databuf,"Start");
	if(sendto(sd, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)
	{
		perror("Sending datagram message error");
	}
	else
	  printf("Sending datagram message...OK\n");

    // Open the file.
    FILE *fp;
    fp = fopen(argv[1],"rb");
    if(fp < 0) {
        perror("ERROR opening the file.");
    }
    bzero(databuf,datalen);
    int read_len,send_len;
    /* total : # of packets */
    int total = 0;
    // Server send the file data message.
    while ((read_len = fread(databuf,sizeof(char),datalen,fp)) > 0) {
        send_len = sendto(sd,databuf,read_len,0,(struct sockaddr*)&groupSock, sizeof(groupSock));
        bzero(databuf,datalen);
        total++;
    }
    // clode file
    fclose(fp);
    bzero(databuf,datalen);
    // Send Stop & # of packets
    strcpy(databuf,"Stop");
    sprintf(databuf+strlen(databuf),"%d",total);
    send_len = sendto(sd,databuf,datalen,0,(struct sockaddr*)&groupSock, sizeof(groupSock));
    printf("Total packets : %s\n",databuf);
    // close socket after 3 secs 
    sleep(3);
    close(sd);
	
    return 0;
}
