/*
	********************************************************************
	Faculty of Technical Sciences
	University of Novi Sad

	Department of Computing and Control Engineering

	Study Programme:    Computing and Control Engineering, Computer Based
   Systems (RT-RK)
	Course:             Fundamentals of Computer Networks 1
	Year:               3, Semester: Winter
	School Year:        2016/2017

	Authors:			Marko Dragojevic, Stevan Stevic

	File Name:          server.h
	Description:        Topic Based TCP/IP server
	********************************************************************
*/

#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <glib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

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

GHashTable* topicsHashMap;

/*
 * Function for parsing command line arguments.
 * -p - Port for connection with publishers
 * -s - Port for connection with subscribers.

* Parameters:
 *  int argc - Number of command line arguments.
 *  char** argv - Command line arguments.
 *
 * Return Value:
 *  1 - Number of arguments is not valid.
 *  2 - Flags are not correctly set.
 *  3 - Invalid flag is used.
 */

int ParseArguments(int argc, char** argv);

/*
 * Thread routine that will handle incoming subscribers.
 *
 * Parmeters:
 *  void* param - Port to create socket for subscribers.
 */

void* SubscribersRoutine(void* param);

/*
 * Thread routine which handles single subscriber.
 * Parmeters:
 *  void* param - Pointer to subscriber socket.
 */

void* SignleSubscriberRoutine(void* param);

/*
 * Thread routine that will handle incoming publishers.
 *
 * Parmeters:
 *  void* param - Port to create socket for publishers.
 */

void* PublishersRoutine(void* param);

/*
 * Thread routine which handles single publisher.
 *
 * Parmeters:
 *  void* param - Pointer to publisher socket.
 */

void* SignlePublisherRoutine(void* param);

/*
 * Function for parsing any input that comes from subscriber or publisher.
 *
 * Parmeters:
 *  char* message - Input from clients.
 *  Input* input - structure to store parsed info.
 *  int flag - 0 if subscriber sends input, 1 if it's publisher.
 *
 * Return Value:
 *  It dosen't returns error, because all possible errors are handled inside of
 * a
 *  function.
 */
void ParseInput(char* message, Input* input, int flag);

/*
 * This function will be called for iterating over list of subscribers,
 * to send them data from publisher.
 *
 * Parmeters:
 *  gpointer data - Not set by user, it represents list values.
 *  gpointer inp - This is an Input structure, that will be sent.
 */

void SendDataToSubs(gpointer data, gpointer inp);

/*
 * This function will be called for iterating over hash map,
 * to find occurrences of subscriber in all topics(lists).
 * Parmeters:
 *  gpointer key, gpointer value - Not set by user, name is self-explanitory.
 *  gpointer socket - Subscriber's socket to remove.
 */

void RemovingSubFromAllTopics(gpointer key, gpointer value, gpointer socket);

/*
 * This function notifies all subscribers from list that publisher is gone,
 * and they will no longer recive anything from him. List is freed (not in this
 * function
 * Parmeters:
 *  gpointer data - Not set by user, it represents list values.
 *  gpointer top - Topic that needs to be removed
 */

void NotifySubsForLeaving(gpointer data, gpointer top);

#endif
