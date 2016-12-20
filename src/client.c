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

#include "client.h"

#define DEFAULT_BUFFER_LENGTH 512

char* ip = NULL;
int port = 0;

int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char *message;
	int messLen;
	int error;
	int* sockPointer;
	int i;

	pthread_t listeningHandle;
	i = 0;
	messLen = 0;
	opterr = 0;
	sockPointer = &sock;
 
	error = ParseArguments(argc, argv);

	if(argc > 5){
		puts("Too Much Arguments");
		exit(1);
	}

	//Checking for error in IP input parameters
	if (error == 0){
	// All params are ok, because it never enters default in
	}else if(error == -1) {
		puts("Missing Parameters");
		exit(1);
	} else {
		puts("Invalid Arguments");
		exit(1);
	}
	fflush(stdout);

	//Creating socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1){
		puts("Could Not Creat Socket\n");
	}

    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

	
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Connection failed, Error");
        return 1;
    }

    printf("You have successfully connected to: %s : %d \n", ip, port);
	//Creating Thread for listening routine
	pthread_create(&listeningHandle, NULL, ListeningRoutine, (void*)sockPointer);

	puts("If you want to communicate with the server, type in your message and press enter");
	fflush(stdout);

	//Message sending routine
	while(1){

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
			
		//Convert user's message to upper case
		for(i = 0; i < strlen(message); i++)
			message[i] = toupper(message[i]);

		if(!strcmp(message,"QUIT"))
			break;

		fflush(stdin);
		fflush(stdout);
		
		//Reseting message buffer to null terminators, to remove garbage
		memset(message, '\0', strlen(message));

	}
	
    close(sock);

    return 0;
}

void* ListeningRoutine(void* param){
		
	int sock;
	char message[DEFAULT_BUFFER_LENGTH];
	int readSize;
   	sock = *(int*)param;
	
	//Clearing buffer to avoid server's garbage
	memset(message, '\0', DEFAULT_BUFFER_LENGTH);
		
   	while((readSize = recv(sock, message, DEFAULT_BUFFER_LENGTH, 0)) > 0){
		message[readSize] = '\0';
		printf("Server message: %s\n", message);
	}
	return NULL;	
}

int ParseArguments(int argc, char** argv){

	int c;
	int iFlag;
	int pFlag;
	int retVal;

	retVal = 0;
	iFlag = 0;
	pFlag = 0;


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
		retVal = -1;
	}
	if(pFlag == 0){
		retVal = -1;
	}

	return retVal;
}

