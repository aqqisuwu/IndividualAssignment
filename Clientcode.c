#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 1000
#define PORT 1717
#define STAD struct sockaddr
void func(int sock_desc)
{
	char buff[MAX];
	int l;
	bzero(buff, sizeof(buff));
	l = 0;
	if((buff[l++] = getchar()) != '\n')
	{
		write(sock_desc, buff, sizeof(buff));
		bzero(buff, sizeof(buff));
		read(sock_desc, buff, sizeof(buff));
		printf("\nFrom server : %s\n", buff);
	}
}

int main()
{
	int sock_desc, connfd;
	struct sockaddr_in servaddr, client;

	// This part is for creating socket and verification
	sock_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_desc == -1) {
		printf("Failed\n");
		exit(0);
	}
	else
		printf("Socket succefull\n");
	bzero(&servaddr, sizeof(servaddr));

	// This part to assign IP and PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("192.168.0.171");
	servaddr.sin_port = htons(PORT);

	// This part to connect the client socket to server socket
	if (connect(sock_desc, (STAD*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Failed\n");
		exit(0);
	}
	else
		printf("Server successfully connected \n");

	func(sock_desc);
	close(sock_desc);
}
