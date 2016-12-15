#ifndef SUBSCRIBER_H_INCLUDED
#define SUBSCRIBER_H_INCLUDED

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <ctype.h>
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

#endif
