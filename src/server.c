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

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015

int portPublishers;
int portSubscribers;

int main(int argc, char** argv) {
    int* pPortSub;
    int* pPortPub;

    ParseArguments(argc, argv);

    pthread_t sockets[2];

    portPublishers = 30000;
    portSubscribers = 27015;

    pPortPub = &portPublishers;
    pPortSub = &portSubscribers;

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
        puts("Connection accepted");
    }

    close(socketDesc);

    return NULL;
}

void* SignleSubscriberRoutine(void* param) {
    int subscriberSock;
    int readSize;
    char subscriberMessage[DEFAULT_BUFLEN];
    subscriberSock = *(int*)param;


    while( (readSize = recv(subscriberSock , subscriberMessage , DEFAULT_BUFLEN , 0)) > 0 )
    {
        printf("Bytes received: %d\n", readSize);
        *(subscriberMessage + readSize) = '\0';
        printf("Messagee: %s\n", subscriberMessage);
        fflush(stdout);

        if( (send(subscriberSock, subscriberMessage, readSize, 0)) < 0) {
            fprintf(stderr, "Send failed: %s\n", strerror(errno));
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
        puts("Connection accepted");
    }

    close(socketDesc);

    return NULL;
}

void* SignlePublisherRoutine(void* param) {
    int publisherSock;
    int readSize;
    char publisherMessage[DEFAULT_BUFLEN];

    publisherSock = *(int*)param;

    while( (readSize = recv(publisherSock , publisherMessage , DEFAULT_BUFLEN , 0)) > 0) {

    }

    return NULL;
}






























