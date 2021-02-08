/*  File Name : Server.cpp
    Author : Arup Sarker
    Creation date : 07/02/2015
    Description: Main responsibilities of this source are
    1. Create socket and bind with client to a specific port
    2. Receive message from client

    Notes: BSD socket apis are used and compiled with g++ in Ubuntu 18.04. For Other OS, files needs be cross compiled and create executable
*/

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>

#define MAX_LENGTH_OF_SINGLE_LINE 4096

#define LISTEN_PORT 8050

#define SERVER_PORT 5080

#define BUFFER_SIZE 100000
#define MAX_NAME 1000




int main(int argc, char *argv[]) {
	char *hello = "Hi, I'm server"; 
	// 1. Create TCP Socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("Unable to create Socket");
		exit(1);
	}


	// 2. Setup information about client and server
	struct sockaddr_in nwClientAddr, nwServerAddr;
	memset(&nwServerAddr, 0, sizeof(nwServerAddr));

	// 3. IP address should be IPv4
	nwServerAddr.sin_family = AF_INET;
	nwServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	nwServerAddr.sin_port = htons(SERVER_PORT);

	// 4. Bind to Server Address
	if (bind(sockfd, (const struct sockaddr *) &nwServerAddr, sizeof(nwServerAddr)) == -1) {
		perror("Unable to Bind");
		exit(1);
	}

	// 5. Listen Socket for incoming File
	if (listen(sockfd, LISTEN_PORT) == -1) {
		perror("Error in Socket Listening");
		exit(1);
	}
	

	// 6. Accept connection for concurrency number
	socklen_t adlen = sizeof(nwClientAddr);
	int cfd = accept(sockfd, (struct sockaddr *) &nwClientAddr, &adlen);
	if (cfd == -1) {
		perror("Error in Accepting Connection for ");
		exit(1);
	}
	int val = 0;

	char conString[MAX_NAME] = {0};
	if (recv(cfd, conString, MAX_NAME,0) == -1) {
		perror("Unable to receive message due to connection error");
		exit(1);
	}
	char *numstr = basename(conString);
	printf("Received From Client <--- : %s\n", numstr);

	
	send(cfd , hello , strlen(hello) , 0 ); 
    	printf("%s message is sent to client\n",hello);

	close(cfd);
	
	close(sockfd);
	return 0;
}


