/*
 * socket-client.c
 * Simple TCP/IP communication using sockets
 *
 * Giannis Giortzis <giortzis.giannis@gmail.com>
 * Dimitris Lampros <dim.lambros@gmail.com>
 */

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "socket-common.h"
#include "chat.h"

//-----------------Function to check if input is integer--------------------////
bool isNumeric( char str[]) {
   for (int i = 0; i < strlen(str); i++)
      if ((isdigit(str[i]) ||  str[i]=='\n' || str[i]==' ')== false)
      return false; //when one non numeric value is found, return false
   return true;
}

void usage(const char *prog) {
    printf("Usage: %s hostname port\n",prog);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	int sd, port;
	char *hostname;
	struct hostent *hp;
	struct sockaddr_in sa;

//Arguments ERROR CHEKCING
	if (argc != 3) {
		usage(argv[0]);
	}

	hostname = argv[1];
//---------------------------------------Checking if argv[2] is valid numeric-------------------
 if (!(isNumeric(argv[2])) )         //if not integer,exit
	{
	usage(argv[0]);
	}
	port = atoi(argv[2]); 			//if negative or zero, exit
if (port<=0)
	{
    usage(argv[0]);
	}
//-------------------------------------------------------------------------------------------

	/* Create TCP/IP socket, used as main chat channel */
	if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	fprintf(stderr, "Created TCP socket\n");

	/* Look up remote hostname on DNS */
	if ( !(hp = gethostbyname(hostname))) {
		printf("DNS lookup failed for host %s\n", hostname);
		exit(1);
	}

	/* Connect to remote TCP port */
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	memcpy(&sa.sin_addr.s_addr, hp->h_addr, sizeof(struct in_addr));
	fprintf(stderr, "Connecting to remote host... "); fflush(stderr);
	if (connect(sd, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		perror("connect");
		exit(1);
	}
	fprintf(stderr, "Connected.\n");
	chat_init(sd);
	fprintf(stderr, "\nDone.\n");
	return 0;
}
