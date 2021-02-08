/*  File Name : NWClient.cpp
    Author : Arup Sarker
    Creation date : 12/28/2020
    Description: Main responsibilities of this source are
    1. Create socket and connect with server to a specific port
    2. Send multiple files to server concurrently from a directory
    3. Utility functions to check file integrity by calculating checkSum

    Notes: BSD socket apis are used and compiled with g++ in Ubuntu 18.04. For Other OS, files needs be cross compiled and create executable
*/


#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <libgen.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <assert.h>



#define MAX_LENGTH_OF_SINGLE_LINE 4096
#define SERVER_PORT 5080
#define BUFFER_SIZE 100000
#define MAX_FILE_COUNT 1000
#define MAX_NAME 1000

// Utility functions for socket
void sockUtil(char *ipAddr) {

	// 2. Create TCP Socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("Unable to create Socket");
		exit(1);
	}

	// 3. Setup information about server
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));

	// 4. IP address should be IPv4
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);

	// 5. Check IP adddres and convert it with inet_pton
	if (inet_pton(AF_INET, ipAddr, &serverAddress.sin_addr) < 0) {
		perror("Conversion Error in IP Address");
		exit(1);
	}

	// 6. Client will connect after server bind
	if (connect(sockfd, (const struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
		perror("Unable to Connect");
		exit(1);
	}

 
	char numberstring[] = "Hello, I'm client"; 

	if (send(sockfd, numberstring, strlen(numberstring), 0) == -1) {
		perror("Unable to send Concurrency");
		exit(1);
	}
	printf("Message is sent to Server\n");

	char conString[MAX_NAME] = {0};
	if (recv(sockfd, conString, MAX_NAME,0) == -1) {
		perror("Unable to receive message due to connection error");
		exit(1);
	}
	char *numstr = basename(conString);
	printf("Received From Server <--- : %s\n", numstr);

	close(sockfd);

}


int main(int argc, char* argv[]) {
	// 1. Check the parameters from command line argument
	if (argc < 2) {
		perror("IP address is missing");
		exit(1);
	}
		
	sockUtil(argv[1]);
	return 0;
}


