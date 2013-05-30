#include "common/common.h"

const char* compressionProgram="lzop";
const char *filenameExtension=".lzo";
char *arguments="-1 --force"; // 1 is fastest, 9 is best compression. '--force- is for overwriting

int compressFile(const char* filename) {

	// Constructing string to execute
	char *command= (char*) malloc( sizeof(compressionProgram)+1+sizeof(filename)+1+sizeof(arguments)+1 );
	if (command==NULL) {
		printf("Unable to allocate mem\n");
		exit(1);
	}
	sprintf(command,"%s %s %s",compressionProgram,filename,arguments);
//	printf("command to execute: %s\n",command);

	//Executing command:
	return ( system(command) );
}

int remove(const char* filename) {
        // Constructing string to execute
        char *baseCommand="rm -f";
        char *command= (char*) malloc( sizeof(baseCommand)+1+sizeof(filename)+1 );
        if (command==NULL) {
                printf("Unable to allocate mem\n");
                exit(1);
        }
        sprintf(command,"%s %s",baseCommand,filename);
//	printf("command to execute: %s\n",command);

	//Executing command:
        return ( system(command) );
}


int main(int argc, char *argv[]) {

	if (argc != 3) {
		fprintf(stderr, "Usage: ./fileclient <file> <serverIP>\n");
		exit(1);
	}

	int sockfd;
	char buff[BUFFERSIZE + 1];
	char filenameheader[BUFFERSIZE + 1];
	struct sockaddr_in servaddr;
	int filefd;    /* file descriptor */
	int count;

	const int filenameExtensionLength=sizeof(filenameExtension);
	char *filenameSource= (char*) malloc(sizeof(argv[1])); // basename
	char *filenameDest= (char*) malloc(sizeof(argv[1])+filenameExtensionLength); // basename+extension
	if (filenameSource==NULL || filenameDest==NULL) {
		printf("Unable to allocate mem\n");
		exit(1);
	}
	sprintf(filenameSource,"%s",argv[1]);
	sprintf(filenameDest,"%s%s",argv[1],filenameExtension);
//	printf("Source: %s\nDest: %s\n",filenameSource,filenameDest);
	
	if ( compressFile(filenameSource) ) {
		printf("Unable to compress %s\n",filenameSource);
		exit(1);
	} else printf("Succesfully created %s\n",filenameDest);
	
//	exit(0); // For debugging - exit before we go online

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	Inet_pton(AF_INET, argv[2], &servaddr.sin_addr);
	servaddr.sin_port = htons(PORT);

	//////////////////////////////////////////////////////////
	printf("connecting........\n");
	Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	// zhouduanqi
	//filenameheader[strlen(argv[1])+8] = '\n';
	//filenameheader[strlen(argv[1])+8] = 0;
	//char * a_test = "test.tar.gz";
	//memcpy(filenameheader, a_test, strlen(a_test));

	//
	printf("transferring file name: %s ........\n", filenameDest);
//	memcpy(filenameheader, argv[1], strlen(argv[1]));
//	filenameheader[strlen(argv[1])] = '\n';
//	filenameheader[strlen(argv[1])+1] = 0;
	writen(sockfd, filenameSource, strlen(filenameheader));

	//zhouduanqi
	//char *command = (char *)malloc(100*sizeof(char));
	//sprintf(command, "tar -cjf test.tar.gz %s", filenameheader);
	//system(command);
	//end

	printf("will transfer file: %s\n", filenameDest);

	filefd = open(filenameDest, O_RDONLY);

	if (filefd < 0) {
		fprintf(stderr, "can't open the file: %s\n", filenameDest);
		exit(1);
	}

	while (count = read(filefd, buff, BUFFERSIZE)) {
	// TODO: print upload speed (and maybe progress)
		if (count < 0) {
			fprintf(stderr, "filefd read error\n");
			exit(1);
		}
		if (writen(sockfd, buff, count) < 0) {
			fprintf(stderr, "writing to sockfd error\n");
			exit(1);
		}
	}
	Close(filefd);
	Close(sockfd);
	free(filenameSource);
	free(filenameDest);
	printf("file %s is succesfully transferred!\n", filenameSource);

	// Cleaning up:
	if ( remove(filenameDest) ) {
		printf("Unable to remove tempoary file %s\n",filenameDest);
		return 1;
	}

	return 0;
}
