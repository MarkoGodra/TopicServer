#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <glib.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_TOPIC_LEN 128

typedef struct input_st {
    char command[15];
    char topic[DEFAULT_TOPIC_LEN];
    char text[DEFAULT_BUFLEN];
} Input;

int portPublishers;
int portSubscribers;

extern int* pPortPub;
extern int* pPortSub;

int* pPortPub = &portPublishers;
int* pPortSub = &portSubscribers;

/*
 * Function for parsing command line arguments.
 * -p - Port for connection with publishers
 * -s - Poet for connection with subscribers.
 * int argc - Number of command line arguments.
 * char** argv - Command line arguments.
 * Returns -1 if flags are not set(p, s).
 * Return -2 if illegal flag is used.
*/
int ParseArguments(int argc, char** argv);

/*
 * Thread routine that will handle incoming subscribers.
 * void* param - Port to create socket for subscribers.
*/
void* SubscribersRoutine(void* param);

/*Thread routine which handles single subscriber.
 * void* param - Pointer to subscriber socket.
*/
void* SignleSubscriberRoutine(void* param);

/*
 * Thread routine that will handle incoming publishers.
 * void* param - Port to create socket for publishers.
*/
void* PublishersRoutine(void* param);

/*Thread routine which handles single publisher.
 * void* param - Pointer to publisher socket.
*/
void* SignlePublisherRoutine(void* param);

int ParseCommand(char* message, Input* input, int flag);

#endif
