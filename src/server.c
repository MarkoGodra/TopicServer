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
	
    Ime fajla:      server.c
    Opis:           Topic Based TCP/IP server
    ********************************************************************
*/

#include "server.h"

int main(int argc, char** argv) {

    ParseArguments(argc, argv);

    pthread_t sockets[2];

    *pPortPub = 30000;
    *pPortSub = 27015;

    //Creating socket for publishers
    pthread_create(&sockets[0], NULL, PublishersRoutine, (void*)pPortPub);

    //Creating socket for subscribers
    pthread_create(&sockets[1], NULL, SubscribersRoutine, (void*)pPortSub);

    pthread_join(sockets[0], NULL);
    pthread_join(sockets[1], NULL);

    return 0;
}

int ParseArguments(int argc, char** argv) {

    return 0;
}

void* SubscribersRoutine(void* param) {
    int socketDesc;
    int subscriberSock;
    struct sockaddr_in server;
    struct sockaddr_in subscriber;
    int size;
    int port;

    port = *(int*)param;

    //Create socket
    socketDesc = socket(AF_INET, SOCK_STREAM, 0);
    if(socketDesc == -1) {
        fprintf(stderr, "Can not create socket: %s\n", strerror(errno));
        exit(1);
    }
    puts("Socket created!");
    fflush(stdout);

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    //Bind
    if( bind(socketDesc, (struct sockaddr *)&server , sizeof(server)) < 0) {
        fprintf(stderr, "Bind failed: %s\n", strerror(errno));
        exit(1);
    }
    puts("Bind done!");
    puts("Waiting for connections...");
    fflush(stdout);

    //Listening for incoming subscriber's connections
    listen(socketDesc , 3);

    //Accpeting subscriber connections
    size = sizeof(struct sockaddr_in);
    while( (subscriberSock = accept(socketDesc, (struct sockaddr *)&subscriber, (socklen_t*)&size)) ) {
        if (subscriberSock < 0) {
            fprintf(stderr, "Accept failed: %s\n", strerror(errno));
            exit(1);
        }

        pthread_t subscriberT;

        int* newSock = (int*)malloc(sizeof(int));
        *newSock = subscriberSock;

        pthread_create(&subscriberT, NULL, SignleSubscriberRoutine, (void*)newSock);
        pthread_detach(subscriberT);

        puts("Connection accepted");
    }

    close(socketDesc);

    return NULL;
}

void* SignleSubscriberRoutine(void* param) {
    int subscriberSock;
    int readSize;
    char subscriberMessage[DEFAULT_BUFLEN];
    char* serverMessage;
    Input subInput;
    int error;

    subscriberSock = *(int*)param;

    while( (readSize = recv(subscriberSock , subscriberMessage , DEFAULT_BUFLEN , 0)) > 0 )
    {
        printf("Bytes received: %d\n", readSize);
        *(subscriberMessage + readSize) = '\0';
        printf("Messagee: %s\n", subscriberMessage);
        fflush(stdout);

        error = ParseCommand(subscriberMessage, &subInput, 0);

        if( strcmp(subInput.command, "QUIT") == 0) {
            //Remove from all lists!!!
            //Close connection
            serverMessage = "Thank you for using our service, see ya soon!!!";
            if( (send(subscriberSock, serverMessage, strlen(serverMessage), 0)) < 0) {
                fprintf(stderr, "Send of '%s' message failed: %s\n", serverMessage, strerror(errno));
            }

            close(subscriberSock);
            break;
        }
        else if( strcmp(subInput.command, "SUBSCRIBE") == 0) {
            char msg[DEFAULT_BUFLEN] = "You will now recive informations about ";
            strcat(msg, subInput.topic);
            strcat(msg, "!");
            serverMessage = msg;

            //Add to list if list is there, or error msg if not
        }
        else if(strcmp(subInput.command, "UNSUBSCRIBE") == 0) {
            //Remove from list or error msg if topic isn't existing or he is not subed
        }
        else {
            serverMessage = "Command you entered is not recognized! Check your spelling, or see documentation!";
        }

        if( (send(subscriberSock, serverMessage, strlen(serverMessage), 0)) < 0) {
            fprintf(stderr, "Send of '%s' message failed: %s\n", serverMessage, strerror(errno));
        }
    }

    return NULL;
}

void* PublishersRoutine(void* param) {
    int socketDesc;
    int publisherSock;
    struct sockaddr_in server;
    struct sockaddr_in publisher;
    int size;
    int port;

    port = *(int*)param;

    //Create socket for publishers
    socketDesc = socket(AF_INET, SOCK_STREAM, 0);
    if(socketDesc == -1) {
        fprintf(stderr, "Can not create socket for publishers: %s\n", strerror(errno));
        exit(1);
    }
    puts("Socket for publishers created!");
    fflush(stdout);

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    //Bind
    if( bind(socketDesc, (struct sockaddr *)&server , sizeof(server)) < 0) {
        fprintf(stderr, "Binding publishers socket failed: %s\n", strerror(errno));
        exit(1);
    }
    puts("Binding socket for publishers done!");
    puts("Waiting for connections...");
    fflush(stdout);

    //Listening for incoming publishers connections
    listen(socketDesc , 3);

    //Accpeting publishers  connections
    size = sizeof(struct sockaddr_in);
    while( (publisherSock = accept(socketDesc, (struct sockaddr *)&publisher, (socklen_t*)&size)) ) {
        if (publisherSock < 0) {
            fprintf(stderr, "Accept failed: %s\n", strerror(errno));
            exit(1);
        }

        pthread_t publisherT;

        int* newSock = (int*)malloc(sizeof(int));
        *newSock = publisherSock;

        pthread_create(&publisherT, NULL, SignlePublisherRoutine, (void*)newSock);
        pthread_detach(publisherT);
        puts("Connection accepted");
    }

    close(socketDesc);

    return NULL;
}

void* SignlePublisherRoutine(void* param) {
    int publisherSock;
    int readSize;
    char publisherMessage[DEFAULT_BUFLEN];
    char* serverMessage;
    Input pubInput;
    int error;

    publisherSock = *(int*)param;

    while( (readSize = recv(publisherSock , publisherMessage , DEFAULT_BUFLEN , 0)) > 0) {
        printf("Bytes received: %d\n", readSize);
        *(publisherMessage + readSize) = '\0';
        printf("Messagee: %s\n", publisherMessage);
        fflush(stdout);

        error = ParseCommand(publisherMessage, &pubInput, 0);

        printf("%s\n", pubInput.text);

        if( strcmp(pubInput.command, "QUIT") == 0) {
            //REMOVE LIST WITH HIS SUBS

            serverMessage = "Thank you for providing information for us, see ya soon!!!";
            if( (send(publisherSock, serverMessage, strlen(serverMessage), 0)) < 0) {
                fprintf(stderr, "Send of '%s' message failed: %s\n", serverMessage, strerror(errno));
            }

            close(publisherSock);
            break;
        }
        else if( strcmp(pubInput.command, "PUBLISH") == 0) {
            char msg[DEFAULT_BUFLEN] = "You will publish information for topic: ";
            strcat(msg, pubInput.topic);
            strcat(msg, "!");
            serverMessage = msg;

            //SEND TO ALL IF THERE, CREATE NEW IF NOT
        }
        else {
            serverMessage = "Command you entered is not recognized! Check your spelling, or see documentation!";
        }

        if( (send(publisherSock, serverMessage, strlen(serverMessage), 0)) < 0) {
            fprintf(stderr, "Send of '%s' message failed: %s\n", serverMessage, strerror(errno));
        }
    }

    return NULL;
}

int ParseCommand(char* message, Input* input, int flag) {
    int i;
    char* temp;

    //Parsing command
    temp = message;
    i = 0;

    while( *temp != ' ' && *temp != '\0') {
        input->command[i++] = *temp++;
    }

    input->command[i] = '\0';

    //If command is quit stop parsing to avoid SEGFAULT or similar
    if( strcmp(input->command, "QUIT") == 0) {
        return 0;
    }

    //Parsing topic
    temp++;     //Skiping space
    i = 0;
    while( *temp != ' ' && *temp != '\0') {
        input->topic[i++] = *temp++;
    }

    input->topic[i] = '\0';

    if(flag == 0) {       //It is subsriber and further parsing is not neccessary
        input->text[0] = '\0';
        return 0;
    }

    //Parsing text from publishers
    temp++;     //Skiping space
    i = 0;

    while( *temp != ' ' && *temp != '\0') {
        input->text[i++] = *temp++;
    }

    input->text[i] = '\0';

    return 0;
}



























