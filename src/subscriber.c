/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2016/2017
    Semestar:       Zimski (V)

	Autori:			Marko Dragojevic, Stevan Stevic
    
    Ime fajla:      subscriber.c
    Opis:           Subscriber for topic based TCP/IP server
    ********************************************************************
*/

#include "subscriber.h"

#define DEFAULT_BUFFER_LENGTH 512
//#define DEFAULT_PORT   27015

char* ip = NULL;
int port = 0;


int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char *message;
	int messLen;
	int error;
	int readSize;

	messLen = 0;
	readSize = 0;
	opterr = 0;
 
	error = ParseArguments(argc, argv);

	if(argc > 5){
		puts("Too Much Arguments");
		exit(1);
	}

	if(error == 0){
		puts("IP Address You Entered: ");
		puts(ip);
		fflush(stdout);
		printf("Desired Port: %d\n", port);
		fflush(stdout);
	} else if(error == -1) {
		puts("Missing Parameters");
		exit(1);
	} else {
		puts("Invalid Arguments");
		exit(1);
	}

	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1){
		puts("Could Not Creat Socket\n");
	}
	puts("Socket Created");

    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

	
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");
	

	puts("Enter a message:");
	fflush(stdin);


	char *c = (char *)malloc(1);
	message = (char *)malloc(1);
	memset(message, '\0', 1);
	messLen = 0;

	while( read(0, c, 1) > 0) {
		if( *c == '\n') break;
		message = (char *)realloc(message, messLen + 1);
		messLen++;
		strcat(message, c);
	}
		
		//Send some data
		if( send(sock , message , strlen(message), 0) < 0)
		{
			puts("Send failed");
			return 1;
		}

	puts("Client message:");
	puts(message);
	fflush(stdin);
	fflush(stdout);

	//free(message);
	memset(message, '\0', strlen(message));

	while((readSize = recv(sock, message, DEFAULT_BUFFER_LENGTH, 0)) > 0){
		printf("Bytes Recived: %d\n", readSize);
		message[readSize] = '\0';
		printf("Server Feedback: %s\n", message);
	}
	
    close(sock);

    return 0;
}

int ParseArguments(int argc, char** argv){

	int c;
	int iFlag;
	int pFlag;
	int retVal;

	retVal = 0;
	iFlag = 0;
	pFlag = 0;

	//printf("Argc = %d\n", argc);

	while((c = getopt(argc, argv, "i:p:")) != -1) {
		switch(c) {
			case 'i':
				ip = optarg;
				optind--;
				if(strlen(ip) > 7)
					iFlag = 1;
				break;
			case 'p':
				port = atoi(optarg);
				optind--;
				if(port > -1)
					pFlag = 1;
				break;
		}
	}

	if(iFlag == 0){
		//puts("-i Flag Is A Must " );
		retVal = -1;
	}
	if(pFlag == 0){
		//puts("-p Flag Is A Must " );
		retVal = -1;
	}

	return retVal;
}

