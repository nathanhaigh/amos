#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
 int sockfd;
 int len;
 struct sockaddr_in address;
 int result;
 char ch = 'A';
 char send_data[1024], recv_data[1024];
 sockfd = socket(AF_INET, SOCK_STREAM, 0);
 address.sin_family = AF_INET;
 address.sin_addr.s_addr = inet_addr("127.0.0.1");
 address.sin_port = htons(40950);
 len = sizeof(address);
 result = connect(sockfd, (struct sockaddr *)&address, len);

 if (result == -1)
 {
  perror("oops: client1");
  exit(1);
 }
gets(send_data);
send(sockfd,send_data, strlen(send_data),0);
int bytes_recieved;
bytes_recieved = recv(sockfd,recv_data, 1024,0);
recv_data[bytes_recieved] = '\0';
printf(\n Recieved data = %s", recv_data);
close(sockfd);
exit(0);
}
