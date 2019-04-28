/* Receiver/client multicast Datagram example. */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <liquid/liquid.h>

struct sockaddr_in localSock;
struct ip_mreq group;
int sd;
 
int main(int argc, char *argv[])
{

    int datalen;
    unsigned int n = 585; //original data length
    fec_scheme fs = LIQUID_FEC_HAMMING74; //error-correcting scheme
    // compute size of encoded message
    unsigned int k = fec_get_enc_msg_length(fs,n);
    // create message arrays
    unsigned char datadec[n]; // decoded data message
    unsigned char databuf[k]; // received data message

    /* Create a datagram socket on which to receive. */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
	{
		perror("Opening datagram socket error");
		exit(1);
	}
	else
	printf("Opening datagram socket....OK.\n");
		 
	/* Enable SO_REUSEADDR to allow multiple instances of this */
	/* application to receive copies of the multicast datagrams. */
	{
		int reuse = 1;
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
	{
		perror("Setting SO_REUSEADDR error");
		close(sd);
		exit(1);
	}
	else
		printf("Setting SO_REUSEADDR...OK.\n");
	}
	 
	/* Bind to the proper port number with the IP address */
	/* specified as INADDR_ANY. */
	memset((char *) &localSock, 0, sizeof(localSock));
	localSock.sin_family = AF_INET;
	localSock.sin_port = htons(4321);
	localSock.sin_addr.s_addr = INADDR_ANY;
	if(bind(sd, (struct sockaddr*)&localSock, sizeof(localSock)))
	{
		perror("Binding datagram socket error");
		close(sd);
		exit(1);
	}
	else
		printf("Binding datagram socket...OK.\n");
	 
	/* Join the multicast group 226.1.1.1 on the local 127.0.0.1 */
	/* interface. Note that this IP_ADD_MEMBERSHIP option must be */
	/* called for each local interface over which the multicast */
	/* datagrams are to be received. */
	group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");
	group.imr_interface.s_addr = inet_addr("127.0.0.1");
	if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
	{
		perror("Adding multicast group error");
		close(sd);
		exit(1);
	}
	else
		printf("Adding multicast group...OK.\n");
	 
	/* Read from the socket. */
	datalen = sizeof(databuf);
	if(read(sd, databuf, datalen) < 0)
	{
		perror("Reading datagram message error");
		close(sd);
		exit(1);
	}
	else
	{
		printf("Reading datagram message...OK.\n");
		printf("The message from multicast server is: \"%s\"\n", databuf);
    }
    int localSock_len = sizeof(localSock_len);
    if (!strcmp(databuf,"Start")) {
        FILE *fp;
        int recv_len;
        if ((fp = fopen(argv[1],"wb")) == NULL) {
            perror("Eroor opening the file.");
        }
        bzero(databuf,datalen);
        /*total : Accept # of packets*/
        int total = 0;

        // Create the fec object
        fec q = fec_create(fs,NULL);

        while ((recv_len = recvfrom(sd,databuf,sizeof(databuf),0,(struct sockaddr*)&localSock,&localSock_len)) > 0) {
            // Decode message
            fec_decode(q,n,databuf,datadec);
            int tmp = fwrite(datadec,sizeof(char),n,fp);
            total++;
            bzero(databuf,datalen);
            bzero(datadec,n);
            printf("recv_len : %d\n",recv_len);

            if(recv_len < datalen) {
                break;
            }
        }

        // Destroy the fec object
        fec_destroy(q);

        recv_len = recvfrom(sd,databuf,sizeof(databuf),0,(struct sockaddr*)&localSock,&localSock_len);
        if(!strncmp(databuf,"Stop",4)) {
            // n : total packets
            int n = atoi(databuf+4);
            printf("Packet loss rate : %f %%\n",(float)(n-total)/n);

        }
        printf("%d\n",total);
        fclose(fp);
        close(sd);
    }
	return 0;
}
