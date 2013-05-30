#include "common/common.h"

const char* decompressionProgram="lzop";
const char *filenameExtension=".lzo";

int decompress(const char* filename) {

        // Constructing string to execute
	char *arguments="--decompress";
        char *command= (char*) malloc(
		sizeof(decompressionProgram)+1
		+sizeof(filename)
		+sizeof(filenameExtension)+1
		+sizeof(arguments)+1
	);
        if (command==NULL) {
                printf("Unable to allocate mem\n");
                exit(1);
        }
        sprintf(command,"%s %s%s %s",decompressionProgram,filename,filenameExtension,arguments);
//      printf("command to execute: %s\n",command);

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
//      printf("command to execute: %s\n",command);

        //Executing command:
        return ( system(command) );
}

int main(void)
{
	int listenfd, connfd;
	char buff[BUFFERSIZE + 1];
	char filename[BUFFERSIZE + 1];
	struct sockaddr_in servaddr, cliaddr;
	int cliaddrlen;
	int filefd;    /* file descriptor */
	int count;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

//////////////////////////////////////////////////////////
	while(1) {
		printf("listening........\n");

		cliaddrlen = sizeof(cliaddr);
		connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddrlen);
		printf("readinging filename........\n");
		if (readline(connfd, buff, BUFFERSIZE) < 0) {
			fprintf(stderr, "readline error\n");
			exit(1);
		}
		buff[strlen(buff) - 1] = 0;    /* change '\n' to NUL */
		memcpy(filename, buff, BUFFERSIZE + 1);
		printf("will save to file: %s\n", buff);

		filefd = open(buff, O_WRONLY | O_CREAT, S_IRUSR);
		// Added mode 'S_IRUSR' - required when compiling with optimiziation flags
		if (filefd < 0) {
			fprintf(stderr, "can't open the file: %s\n", buff);
			exit(1);
		}

		while( count=read(connfd, buff, BUFFERSIZE) ) {
		// TODO: Print download speed
			printf("Downloaded : %.f\n",(float)count*BUFFERSIZE/1024);
			if (count < 0) {
				fprintf(stderr, "connfd read error\n");
				exit(1);
			}
			if (writen(filefd, buff, count) < 0) {
				fprintf(stderr, "writing to filefd error\n");
				exit(1);
			}
		}
		Close(filefd);
		Close(connfd);
		printf("file %s received!\n", filename);

		if ( decompress(filename) ) {
			printf("Unable to decompress %s%s\n",filename,".lzo");
			return 1;
		}

		if ( remove(strcat(filename, ".lzo")) ) {
                	printf("Unable to remove tempoary file %s%s\n",filename,filenameExtension);
                	return 1;
        	}

	}
	Close(listenfd);
//////////////////////////////////////////////////////////

	return 0;
}
