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
#include <time.h>



#define MAX_LENGTH_OF_SINGLE_LINE 4096
#define SERVER_PORT 5080
#define BUFFER_SIZE 100000
#define MAX_FILE_COUNT 1000
#define MAX_NAME 1000


void fileSender(FILE *filePointer, int sockfd) {

	// 13.2 Declare and Initalize the buffer for a single file

	char lineBuffer[BUFFER_SIZE] = {0};

	int maxLen = 0;
	// 13.3 Read the data from file in a loop
	while ((maxLen = fread(lineBuffer, sizeof(char), BUFFER_SIZE, filePointer)) > 0) {
		if (maxLen != BUFFER_SIZE && ferror(filePointer)) {
		    perror("Unable to read file");
		    exit(1);
		}

		// 13.4 Send each line from buffer through TCP socket
		if (send(sockfd, lineBuffer, maxLen,0) == -1) {
		    perror("Unable to send file");
		    exit(1);
		}
		memset(lineBuffer, '\0', BUFFER_SIZE);
	}
	fclose(filePointer);
}

// Utility functions for socket
void sockUtil(char *ipAddr, char *fileName, char *pathName) {

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

 
	// 11. Send file name from buffer data through socket so that server can create file with same name.

	printf("%s file Sending is started \n",fileName);

	if (send(sockfd, fileName, MAX_NAME, 0) == -1) {
		perror("Unable to send Filename");
		exit(1);
	}

	// 12. Create File pointers
	

	FILE *filePointer = fopen(pathName, "rb");
	if (filePointer == NULL) {
		perror("Unable to open the file");
		exit(1);
	}

	// 13. Send file through socket.
	fileSender(filePointer, sockfd);
	
	printf("%s file is sent successfully\n",fileName);

	close(sockfd);

}


int main(int argc, char* argv[]) {
	// 1. Check the parameters from command line argument
	if (argc < 2) {
		perror("Directory name is missing");
		exit(1);
	}
	if (argc < 3) {
		perror("IP address is missing");
		exit(1);
	}

	char *dirName = basename(argv[1]);
	if (dirName == NULL) {
		perror("Unable to get dirName");
		exit(1);
	}

	// 4. Read All the file list from directory

	DIR *dirPtr = opendir (dirName);

	// 5. validate directory open
	if (!dirPtr) {
		char errbuf[PATH_MAX] = "";
		sprintf (errbuf, "opendir failed on '%s'", dirName);
		perror (errbuf);
		return EXIT_FAILURE;
	}
	printf("Directory name : %s\n", dirName);

	struct dirent *dir;

	int idx = 0;
	char filePathList[MAX_FILE_COUNT][MAX_NAME] = {0};
	char fileNameList[MAX_FILE_COUNT][MAX_NAME] = {0};
	
	if(dirPtr!=NULL) {
		int flag = 0;

		while((dir=readdir(dirPtr))!=NULL) {
			if((strcmp(dir->d_name,".")==0 || strcmp(dir->d_name,"..")==0 || (*dir->d_name) == '.' )){
			}
			else
			{
				// 6. Define and Initialize the Buffer

				char buff[MAX_NAME] = {0};
				char fileWithPath[MAX_NAME] = {0};

				strcat(fileWithPath, dirName);
				strcat(fileWithPath, "/");

				strcat(fileWithPath, dir->d_name);

				// 7. Copy the file name into Buffer
				strncpy(buff, dir->d_name, strlen(dir->d_name));
				
					
				sockUtil(argv[2], buff,fileWithPath);
				

				printf("%s\n", fileWithPath);
				idx++;
			}
		}
	}
	if(idx == 0) {
		printf("There is no file in the directory : %s\n", dirName);
		return 1;
	}
	
	return 0;
}


