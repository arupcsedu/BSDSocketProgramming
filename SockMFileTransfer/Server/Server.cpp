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


void fileWriter(char *fileName, int sockfd, FILE *fp) {
	// 10.1 Declare data length
	int n;
	// 10.2 Declare and Initialize buffer size

	printf("\n%s file Receiving is started in Connection ID: %d\n", fileName, sockfd);

	// 10.3 Receive data from socket and save into buffer
	char buff[BUFFER_SIZE] = {0};
	while(1) {		
		n = recv(sockfd, buff, BUFFER_SIZE,0);
		if (n == -1) {
			perror("Error in Receiving File");
			exit(1);
		}
		if(n <= 0) {
			break;
		}

		// 10.4 Read File Data from buffer and Write into file
		if (fwrite(buff, sizeof(char), n, fp) != n) {
			perror("Error in Writing File");
			exit(1);
		}

		// 10.5 Allocate buffer
		memset(buff, '\0', BUFFER_SIZE);
	}
	fclose(fp);
}


int main(int argc, char *argv[]) {

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
	
	while(1) {
		// 6. Accept connection for concurrency number
		socklen_t adlen = sizeof(nwClientAddr);
		int cfd = accept(sockfd, (struct sockaddr *) &nwClientAddr, &adlen);
		if (cfd == -1) {
			perror("Error in Accepting Connection for ");
			exit(1);
		}
		 // 7. Receive incoming file from client
		char filename[MAX_NAME] = {0};

		if (recv(cfd, filename, MAX_NAME,0) == -1) {
			perror("Unable to receive file due to connection or file error");
			exit(1);
		}

		int fileLength = strlen(filename);

		if(fileLength > 0) {
			// 8. After receiving from client, create file to save data

			FILE *fp = fopen(filename, "wb");
			if (fp == NULL) {
				perror("Unable to create File pointer");
				exit(1);
			}

			// 9. Start receiving file Data and print in console
			char addr[INET_ADDRSTRLEN];

			// 10. Write the data into file.

			fileWriter(filename, cfd, fp);

			// 11. Print success message into console

			printf("%s is received\n",filename);

		}

		close(cfd);
	}
	
	close(sockfd);
	return 0;
}


