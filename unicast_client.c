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
    FILE *fp;
    struct sockaddr_in serv_addr;
    char send_buf[1024];	
    int sock_id,send_len,serv_addr_len,i_ret;

    // create socket
    if((sock_id = socket(AF_INET,SOCK_DGRAM,0)) < 0) {
        perror("Create socket failed.\n");
    }

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(7777);
    inet_pton(AF_INET,"localhost",&serv_addr.sin_addr);
    serv_addr_len = sizeof(serv_addr);

    i_ret = connect(sock_id,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr));
    if(i_ret == -1) {
        perror("Connect socket failed.\n");
    }
        
    // send
    strcpy(send_buf,"give me files!");
    if(sendto(sock_id,send_buf,sizeof(send_buf),0,(struct sockaddr*)&serv_addr,serv_addr_len) == -1) {
        perror("send to server error!\n");
    }

    // file open
    if((fp = fopen(argv[1],"wb")) == NULL) {
        perror("file open failed.\n");
    }
    // receive the file data message
    int recv_len;
    bzero(send_buf,sizeof(send_buf));
    while((recv_len = recvfrom(sock_id,send_buf,sizeof(send_buf),0,(struct sockaddr *)&serv_addr,&serv_addr_len)) > 0) {
        int write_length = fwrite(send_buf,sizeof(char),recv_len,fp);
        bzero(send_buf,sizeof(send_buf));
        if(recv_len < 1024) {break;}
    }
	// close file
    fclose(fp);
	// close socket
    close(sock_id);
}
