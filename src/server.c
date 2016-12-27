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

	File Name:          server.c
	Description:        Topic Based TCP/IP server
	********************************************************************
*/

#include "server.h"

int main(int argc, char** argv)
{
	int i;
	int error;
	pthread_t socketsT[2];
	char command[5];

	topicsHashMap = g_hash_table_new(g_str_hash, g_str_equal);

	// Command line arguments parsing
	error = ParseArguments(argc, argv);
	if (error != 0) {
		puts("Invalid parametars, check the documentation!");
		printf("Error code: %d\n", error);
		exit(EXIT_FAILURE);
	}

	printf("Port for publishers: %d\n", *pPortPub);
	printf("Port for subscribers: %d\n", *pPortSub);
	fflush(stdout);

	// Creating socket for publishers
	pthread_create(&socketsT[0], NULL, PublishersRoutine, (void*)pPortPub);

	// Creating socket for subscribers
	pthread_create(&socketsT[1], NULL, SubscribersRoutine, (void*)pPortSub);

	pthread_detach(socketsT[0]);
	pthread_detach(socketsT[1]);

	puts("To shutdown server type: quit/QUIT");
	fflush(stdout);

	do {
		scanf("%s", command);
		fflush(stdout);

		for (i = 0; i < strlen(command); i++) {
			command[i] = toupper(command[i]);
		}
	} while (strcmp(command, "QUIT") != 0);

	return 0;
}

int ParseArguments(int argc, char** argv)
{
	int c;
	int retVal;

	retVal = 0;

	if (argc != 5) {
		retVal = 1;
	} else {
		while ((c = getopt(argc, argv, "s:p:")) != -1) {
			switch (c) {
			case 's':
				if ((*pPortSub = atoi(optarg)) != 0) {
					optind--;
				} else {
					retVal = 2;
				}
				break;
			case 'p':
				if ((*pPortPub = atoi(optarg)) != 0) {
					optind--;
				} else {
					retVal = 2;
				}
				break;
			default:
				retVal = 3; // Invalid parametars
				break;
			}
		}

		if (*pPortSub < 1 || *pPortPub < 1) {
			retVal = 2;
		}
	}

	return retVal;
}

void* SubscribersRoutine(void* param)
{
	int socketDesc;
	int subscriberSock;
	struct sockaddr_in server;
	struct sockaddr_in subscriber;
	int size;
	int port;

	port = *(int*)param;
	size = sizeof(struct sockaddr_in);

	// Create socket for subscribers
	socketDesc = socket(AF_INET, SOCK_STREAM, 0);
	if (socketDesc == -1) {
		fprintf(stderr, "Creating socket for subscribers failed: %s\n",
				strerror(errno));
		exit(EXIT_FAILURE);
	}

	// Prepare the server sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	// Bind
	if (bind(socketDesc, (struct sockaddr*)&server, sizeof(server)) < 0) {
		fprintf(stderr, "Bind for subscribers socket failed: %s\n",
				strerror(errno));
		exit(EXIT_FAILURE);
	}

	// Listening for incoming subscriber's connections
	listen(socketDesc, 3);

	puts("Socket for subscribers created!");
	fflush(stdout);

	// Accpeting subscriber connections
	while ((subscriberSock = accept(socketDesc, (struct sockaddr*)&subscriber,
									(socklen_t*)&size))) {
		if (subscriberSock < 0) {
			fprintf(stderr, "Connection from %s : %d subscriber failed: %s\n",
					inet_ntoa(subscriber.sin_addr),
					(int)ntohs(subscriber.sin_port), strerror(errno));
			exit(EXIT_FAILURE);
		}

		pthread_t subscriberT;

		int* newSock;
		newSock = (int*)malloc(sizeof(int));
		*newSock = subscriberSock;

		pthread_create(&subscriberT, NULL, SignleSubscriberRoutine,
					   (void*)newSock);
		pthread_detach(subscriberT);

		printf("New connection from: %s : %d as a SUBSCRIBER, ID = %d\n",
			   inet_ntoa(subscriber.sin_addr), (int)ntohs(subscriber.sin_port),
			   subscriberSock);
		fflush(stdout);
	}

	close(socketDesc);

	return NULL;
}

void* SignleSubscriberRoutine(void* param)
{
	int subscriberSock;
	int* pSub;
	int readSize;
	char subscriberMessage[DEFAULT_BUFLEN];
	char* serverMessage;
	Input subInput;

	subscriberSock = *(int*)param;
	pSub = &subscriberSock;

	while ((readSize = recv(subscriberSock, subscriberMessage, DEFAULT_BUFLEN,
							0)) > 0) {
		*(subscriberMessage + readSize) = '\0';

		ParseInput(subscriberMessage, &subInput, 0);

		if (strcmp(subInput.command, "QUIT") == 0) {
			// Remove this subscriber from all topics
			g_hash_table_foreach(topicsHashMap,
								 (GHFunc)RemovingSubFromAllTopics,
								 (gpointer)pSub);

			serverMessage = "You are successfully disconnected. Come again!!!";
			if ((send(subscriberSock, serverMessage, strlen(serverMessage),
					  0)) < 0) {
				fprintf(stderr, "Sending %s message to ID = %d failed: %s\n",
						serverMessage, subscriberSock, strerror(errno));
			}

			printf("Subscriber with ID = %d has disconnected\n",
				   subscriberSock);
			fflush(stdout);

			// Close connection
			close(subscriberSock);

			break;
		} else if (strcmp(subInput.command, "SUBSCRIBE") == 0) {
			GSList* listT = NULL;

			// Get list pointer(value) for topic(key) from hash map
			if (g_hash_table_lookup_extended(topicsHashMap, subInput.topic,
											 NULL, (gpointer*)&listT) == 0) {
				// Key not in a map
				serverMessage = "Specifed topic is not active, or it does'nt "
								"exist. Check your spelling, or try later!";
			} else {
				// If topic exists add new subscriber to list
				if (g_slist_find(listT, pSub) != NULL) {
					// Subscriber is already in list
					serverMessage = "You are already subscribed to this topic!";
				} else {
					listT = g_slist_append(listT, pSub);

					g_hash_table_insert(topicsHashMap, subInput.topic, listT);

					char msg[DEFAULT_BUFLEN] =
						"You will now recive informations about ";
					strcat(msg, subInput.topic);
					strcat(msg, "!");
					serverMessage = msg;
				}
			}
		} else if (strcmp(subInput.command, "UNSUBSCRIBE") == 0) {
			// Remove from list or send error messag if topic isn't existing or
			// he is not subscribed
			GSList* listT = NULL;
			if (g_hash_table_lookup_extended(topicsHashMap, subInput.topic,
											 NULL, (gpointer*)&listT) == 0) {
				// Key not in a map
				serverMessage = "Specifed topic is not active, or it dosen't "
								"exist. Check your spelling, or try later!";
			} else {
				listT = g_slist_remove(listT, pSub);

				g_hash_table_insert(topicsHashMap, subInput.topic, listT);

				serverMessage = "You are sussccesfully unsubscribed!";
			}
		} else {
			serverMessage = "Command you entered is not recognized! Check your "
							"spelling, or see documentation!";
		}

		if ((send(subscriberSock, serverMessage, strlen(serverMessage), 0)) <
			0) {
			fprintf(stderr,
					"Sending %s message to subscriber ID = %d failed: %s\n",
					serverMessage, subscriberSock, strerror(errno));
		}
	}

	return NULL;
}

void* PublishersRoutine(void* param)
{
	int socketDesc;
	int publisherSock;
	struct sockaddr_in server;
	struct sockaddr_in publisher;
	int size;
	int port;

	port = *(int*)param;

	// Create socket for publishers
	socketDesc = socket(AF_INET, SOCK_STREAM, 0);
	if (socketDesc == -1) {
		fprintf(stderr, "Creating socket for publishers failed: %s\n",
				strerror(errno));
		exit(EXIT_FAILURE);
	}

	// Prepare the server sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	// Bind
	if (bind(socketDesc, (struct sockaddr*)&server, sizeof(server)) < 0) {
		fprintf(stderr, "Bind for publishers socket failed: %s\n",
				strerror(errno));
		exit(EXIT_FAILURE);
	}

	// Listening for incoming publishers connections
	listen(socketDesc, 3);

	puts("Socket for publishers created!");

	// Accpeting publishers  connections
	size = sizeof(struct sockaddr_in);
	while ((publisherSock = accept(socketDesc, (struct sockaddr*)&publisher,
								   (socklen_t*)&size))) {
		if (publisherSock < 0) {
			fprintf(stderr, "Connection from %s : %d publisher failed: %s\n",
					inet_ntoa(publisher.sin_addr),
					(int)ntohs(publisher.sin_port), strerror(errno));
			exit(EXIT_FAILURE);
		}

		pthread_t publisherT;

		int* newSock;
		newSock = (int*)malloc(sizeof(int));
		*newSock = publisherSock;

		pthread_create(&publisherT, NULL, SignlePublisherRoutine,
					   (void*)newSock);
		pthread_detach(publisherT);

		printf("New connection from: %s : %d as a PUBLISHER, ID = %d\n",
			   inet_ntoa(publisher.sin_addr), (int)ntohs(publisher.sin_port),
			   publisherSock);
		fflush(stdout);
	}

	close(socketDesc);

	return NULL;
}

void* SignlePublisherRoutine(void* param)
{
	int publisherSock;
	int readSize;
	char publisherMessage[DEFAULT_BUFLEN];
	char* serverMessage;
	Input pubInput;

	publisherSock = *(int*)param;

	while ((readSize =
				recv(publisherSock, publisherMessage, DEFAULT_BUFLEN, 0)) > 0) {
		*(publisherMessage + readSize) = '\0';

		ParseInput(publisherMessage, &pubInput, 1); // Flag for publisher

		if (strcmp(pubInput.command, "QUIT") == 0) {
			GSList* listT = NULL;

			if (g_hash_table_lookup_extended(topicsHashMap, pubInput.topic,
											 NULL, (gpointer*)&listT) == 0) {
				serverMessage = "You are successfully disconnected. Thank you "
								"for providing information for us!";
			} else {
				// Topic exists!
				// Iterate through list to notify everyone that publisher is
				// gone
				g_slist_foreach(listT, (GFunc)NotifySubsForLeaving,
								pubInput.topic);

				// Remove list for this topic and topic from hash map
				g_slist_free(listT);

				// Remove topic
				if (g_hash_table_remove(topicsHashMap, pubInput.topic) == 0) {
					fprintf(stderr, "Removing %s topic failed: %s\n",
							pubInput.topic, strerror(errno));
				}

				serverMessage = "You are successfully disconnected. Thank you "
								"for providing information for us!";
			}

			if ((send(publisherSock, serverMessage, strlen(serverMessage), 0)) <
				0) {
				fprintf(
					stderr,
					"Sending '%s' message to publisher ID = %d failed: %s\n",
					serverMessage, publisherSock, strerror(errno));
			}

			printf("Publisher ID = %d has disconnected\n", publisherSock);
			fflush(stdout);

			close(publisherSock);
			break;
		} else if (strcmp(pubInput.command, "PUBLISH") == 0) {
			GSList* listT = NULL;

			if (g_hash_table_lookup_extended(topicsHashMap, pubInput.topic,
											 NULL, (gpointer*)&listT) == 0) {

				// Add topic(key) to hash map with the value of NULL(listT)
				g_hash_table_insert(topicsHashMap, pubInput.topic, listT);

				char msg[DEFAULT_BUFLEN] =
					"You will publish information for topic: ";
				strcat(msg, pubInput.topic);
				strcat(msg, "!");
				serverMessage = msg;
			} else {
				// Send data to everyone subscribed to this topic
				Input* pInput = &pubInput;

				if (listT != NULL) {
					g_slist_foreach(listT, (GFunc)SendDataToSubs,
									(gpointer)pInput);
				}

				serverMessage = "You published successfully!";
			}
		} else {
			serverMessage = "Command you entered is not recognized! Check your "
							"spelling, or see documentation!";
		}

		if ((send(publisherSock, serverMessage, strlen(serverMessage), 0)) <
			0) {
			fprintf(stderr, "Send of '%s' message failed: %s\n", serverMessage,
					strerror(errno));
		}
	}

	return NULL;
}

void ParseInput(char* message, Input* input, int flag)
{
	int i;
	char* temp;

	// Parsing command
	temp = message;
	i = 0;

	while (*temp != ' ' && *temp != '\0') {
		input->command[i++] = *temp++;
	}

	input->command[i] = '\0';

	// Only working with big letters, giving case insesitive option to clients
	for (i = 0; i < strlen(input->command); i++) {
		input->command[i] = toupper(input->command[i]);
	}

	// If command is quit stop parsing to avoid SEGFAULT or similar
	if (strcmp(input->command, "QUIT") == 0) {
		input->text[0] = '\0';

		/* If publisher enters quit, topic needs to be erased, but if he didn't
		 * publish anything (entered quit right after connecting) key is not in
		 * the hash map.
		 * This leads to SEGFALUTS. It is solved by checking first topic's
		 * character.
		 * If topic[0] is != '\0' it means quit isn't typed right after
		 * connection and it has previous value.
		 * By doing this topic is saved for other publishers
		 */
		if (flag && input->topic[0] != '\0')
			return;

		input->topic[0] = '\0';
		return;
	}

	// Parsing topic
	temp++; // Skiping space
	i = 0;
	while (*temp != ' ' && *temp != '\0') {
		input->topic[i++] = *temp++;
	}

	input->topic[i] = '\0';

	for (i = 0; i < strlen(input->topic); i++) {
		input->topic[i] = toupper(input->topic[i]);
	}

	if (flag == 0) { // It is subscriber and further parsing is not neccessary
		input->text[0] = '\0';
		return;
	}

	// Parsing text from publishers
	temp++; // Skiping space
	i = 0;

	while (*temp != '\0') {
		input->text[i++] = *temp++;
	}

	input->text[i] = '\0';
}

void SendDataToSubs(gpointer data, gpointer inp)
{
	int subSock = *(int*)data;
	Input input = *(Input*)inp;

	char sendStr[DEFAULT_BUFLEN] = "[";
	strcat(sendStr, input.topic);
	strcat(sendStr, "] ");
	strcat(sendStr, input.text);

	if ((send(subSock, sendStr, strlen(sendStr), 0)) < 0) {
		fprintf(stderr, "Sending %s message about successfull subscription to "
						"ID = %d failed: %s\n",
				sendStr, subSock, strerror(errno));
	}
}

void RemovingSubFromAllTopics(gpointer key, gpointer value, gpointer socket)
{
	int* subSock = (int*)socket;

	GSList* listT = g_slist_remove((GSList*)value, subSock);

	g_hash_table_insert(topicsHashMap, key, listT);
}

void NotifySubsForLeaving(gpointer data, gpointer top)
{
	int subSock = *(int*)data;
	char* topic = (char*)top;

	char sendStr[DEFAULT_BUFLEN] = "Publisher for [";
	strcat(sendStr, topic);
	strcat(sendStr, "] topic is disconnected!");

	if ((send(subSock, sendStr, strlen(sendStr), 0)) < 0) {
		fprintf(stderr, "Sending %s message about disconnected publisher to "
						"subscriber ID = %d failed: %s\n",
				sendStr, subSock, strerror(errno));
	}
}
