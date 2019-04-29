#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <arpa/inet.h>
# include <fcntl.h>

int main(int argc,char *argv[]) {
    struct sockaddr_in serv_addr;
    struct sockaddr_in clie_addr;
    char recv_buf[1024];
    int sock_id,recv_len,clie_addr_len;
    FILE *fp;

    // create socket
    if((sock_id = socket(AF_INET,SOCK_DGRAM,0)) < 0) {
        perror("Create socket failed.\n");
    }
        
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(7777);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind 
    if(bind(sock_id,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        perror("bind socket failed.\n");
    }

	// Get the size of clie_addr    
	clie_addr_len = sizeof(clie_addr);
    bzero(recv_buf,1024);
    // recieve client's request
    if(recv_len = recvfrom(sock_id,recv_buf,1024,0,(struct sockaddr *)&clie_addr,&clie_addr_len)) {
        if(recv_len < 0) {
            printf("Recieve data from client failed.\n");
        }
        printf("client : %s\n",recv_buf);
        bzero(recv_buf,1024);

        // file open
        if((fp = fopen(argv[1],"rb")) == NULL) {
            perror("file open failed.\n");
        }
        // send file
        int read_len,send_len;

        while((read_len = fread(recv_buf,sizeof(char),1024,fp)) > 0) {
            send_len = sendto(sock_id,recv_buf,read_len,0,(struct sockaddr *)&clie_addr,clie_addr_len);
            bzero(recv_buf,1024);
        }
        // close file
        fclose(fp);
    }
	// clode socket
    close(sock_id);
}
