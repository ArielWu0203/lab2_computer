/* Send Multicast Datagram code example. */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <liquid/liquid.h>

struct in_addr localInterface;
struct sockaddr_in groupSock;
int sd;
 
int main (int argc, char *argv[ ])
{
    unsigned int n = 585; // original data length
    fec_scheme fs = LIQUID_FEC_HAMMING74;//error-correcting scheme
    /*compute size of encoded message*/
    unsigned int k = fec_get_enc_msg_length(fs,n);
    // create message array
    unsigned char dataorg[n];// original data message
    unsigned char databuf[k];// encoded data message
    // datalen = 1024
    int datalen = sizeof(databuf);

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
    strcpy(databuf,"Start");
	if(sendto(sd, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)
	{
		perror("Sending datagram message error");
	}
	else
	  printf("Sending datagram message...OK\n");

    // Sever sends file.
    FILE *fp;
    fp = fopen(argv[1],"rb");
    if(fp < 0) {
        perror("ERROR opening the file.");
    }
    bzero(databuf,datalen);
    int read_len,send_len;
    /* total : # of packets */
    int total = 0;

    // Create the fec object
    fec q = fec_create(fs,NULL);
    
    while ((read_len = fread(dataorg,sizeof(char),n,fp)) > 0) {
        // Encode message
        fec_encode(q,n,dataorg,databuf);
        send_len = sendto(sd,databuf,fec_get_enc_msg_length(fs,read_len),0,(struct sockaddr*)&groupSock, sizeof(groupSock));
        bzero(databuf,datalen);
        bzero(dataorg,n);
        total++;
    }
    // Destroy the fec object
    fec_destroy(q);
    // Close the file
    fclose(fp);
    bzero(databuf,datalen);
    // Send Stop message & # of packets
    strcpy(databuf,"Stop");
    sprintf(databuf+strlen(databuf),"%d",total);
    send_len = sendto(sd,databuf,datalen,0,(struct sockaddr*)&groupSock, sizeof(groupSock));
    printf("Total packets : %s\n",databuf);
    // close the socket after 3 secs
    sleep(3);
    close(sd);
	return 0;
}
