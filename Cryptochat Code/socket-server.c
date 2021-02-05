/*
 * socket-server.c
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
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#include "chat.h"
#include "socket-common.h"
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int main(void)
{
	char addrstr[INET_ADDRSTRLEN];
	int sd, newsd;
	socklen_t len;
	struct sockaddr_in sa;
	int cryptodev_fd = open("/dev/crypto", O_RDWR);
	if (cryptodev_fd < 0) {
		perror("open(/dev/crypto)");
		return 1;
	}
	/* Make sure a broken connection doesn't kill us */ //This means, that if process tried to write to an invalid endpoint, it won't be killed.
	signal(SIGPIPE, SIG_IGN);

	/* Create TCP/IP socket, used as main chat channel */
	if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	fprintf(stderr, "Created TCP socket\n");

	/* Bind to a well-known port */
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(TCP_PORT);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		perror("bind");
		exit(1);
	}
	fprintf(stderr, "Bound TCP socket to port %d\n", TCP_PORT);

	/* Listen for incoming connections */
	if (listen(sd, TCP_BACKLOG) < 0) {
		perror("listen");
		exit(1);
	}



	/* Loop forever, accept()ing connections */
	for (;;) {
		fprintf(stderr, "Waiting for an incoming connection...\n");

		/* Accept an incoming connection */
		len = sizeof(struct sockaddr_in);
		if ((newsd = accept(sd, (struct sockaddr *)&sa, &len)) < 0) {
			perror("accept");
			exit(1);
		}
		if (!inet_ntop(AF_INET, &sa.sin_addr, addrstr, sizeof(addrstr))) {
			perror("could not format IP address");
			exit(1);
		}
		fprintf(stderr, "Incoming connection from %s:%d\n",
			addrstr, ntohs(sa.sin_port));

  chat_init(newsd);
  }
	/* This will never happen */
	return 1;
}
