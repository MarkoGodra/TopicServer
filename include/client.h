/*
 * ********************************************************************
	Faculty of Technical Sciences
	University of Novi Sad

	Department of Computing and Control Engineering

	Study Programme:    Computing and Control Engineering, Computer Based
 Systems (RT-RK)
	Course:             Fundamentals of Computer Networks 1
	Year:               3, Semester: Winter
	School Year:        2016/2017

	Authors:			Marko Dragojevic, Stevan Stevic

	File Name:          client.c
	Description:        Client that represents publisher nad subscriber for
 topic based TCP/IP server
	********************************************************************
*/

#ifndef SUBSCRIBER_H_INCLUDED
#define SUBSCRIBER_H_INCLUDED

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEFAULT_BUFFER_LENGTH 512

extern char* ip;
extern int port;

/*
 * Function for parsing command line arguments.
 * -i is the IP address.
 * -p is the PORT.
 *
 * Parameters:
 * int argc - number of arguments from terminal.
 * char** argv - command line arguments.
 *
 * Return Value:
 * return -1 if flags are not set (i,p).
 * return -2 if illegal flag is used.
 */

int ParseArguments(int argc, char** argv);

/*
 * Thread function for listening.
 *
 * Parameters:
 *  void* param - Passing socket descriptor.
 */

void* ListeningRoutine(void* param);

#endif
