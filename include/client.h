#ifndef SUBSCRIBER_H_INCLUDED
#define SUBSCRIBER_H_INCLUDED

#include <arpa/inet.h>
#include <ctype.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unistd.h>

extern char* ip;
extern int port;

/*
 * Function for parsing command line arguments.
 * -i is the IP address.
 * -p is the PORT.
 * int argc - number of arguments from terminal.
 * char** argv - command line arguments.
 * return -1 if flags are not set (i,p).
 * return -2 if illegal flag is used.
 */

int ParseArguments(int argc, char** argv);

/*
 * Thread function for listening.
 * void* param - Passing socket descriptor.
 */

void* ListeningRoutine(void* param);

#endif
