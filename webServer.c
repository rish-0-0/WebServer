#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include "webServerInclude.h"

void* get_in_addr(struct sockaddr* sa);

int main()
{

	/*


		WILL BE MAKING A MULTI-THREADED WEB SERVER RUNNING ON 8080 (SERVER_PORT) PORT to avoid superuser priveleges

		HAVE TO SET UP A THREAD POOL (around 100 threads will be fine, SERVER BACKLOG WILL BE AROUND 30 connections)

		HAVE TO SET UP A LOCK SYSTEM: PREFERABLY MUTEX LOCKS BECAUSE SIMPLICITY

		HAVE TO SUSPEND THREADS WHEN THEY FINISH WORKING (MAKING THREADSAFE FUNCTION)

		EACH THREAD WILL BE DOING FILE I/O FOR A CLIENT.

		ROUTES: GET / HTTP/1.1\r\n -----> index.html
				GET /img.jpg HTTP/1.1\r\n -----> img.jpg
				GET /about HTTP/1.1\r\n -----> about.html
				GET /favicon.ico HTTP/1.1\r\n -----> favicon.ico



	*/

	int sockfd, clientfd;
	struct addrinfo server_addr_hints, *serverinfo, *p;
	struct sockaddr_storage client_addr;
	socklen_t sin_size;
	int return_value;

	int allow_port_reuse = 1;

	memset(&server_addr_hints, 0, sizeof server_addr_hints);
	server_addr_hints.ai_family = AF_UNSPEC;
	server_addr_hints.ai_socktype = SOCK_STREAM;

	if ( (return_value = getaddrinfo(NULL, SERVER_PORT, &server_addr_hints, &serverinfo)) != 0 )
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(return_value));
		exit(1);
	}

	// ITERATING THE LINKED LIST
	for (p = serverinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &allow_port_reuse, sizeof(int)) == -1)
		{
			perror("server: sockopt");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("server: bind");
			continue;
		}
		// If everything goes well, the first item in the linked list will be AF_INET type, so not a big deal
		break;
		// Hence break because we don't want IPv6 address and all that hassle
	}

	freeaddrinfo(serverinfo);

	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(2);
	}

	if (listen(sockfd, SERVER_MAX_CONNECTIONS) == -1)
	{
		perror("listen");
		exit(2);
	}

	printf("Waiting for connections ... \n");



	return 0;
}

void* get_in_addr(struct sockaddr* sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}