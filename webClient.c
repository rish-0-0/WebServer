#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <fcntl.h>
// Our own header
#include "webServerInclude.h"

void* get_in_addr(struct sockaddr* sa);

int main(int argc, char** argv)
{
	// Socket file descriptors and temporary variables
	int sockfd, sendBytes;

	// For calculating IP address from hostname, p is a pointer to a Linked List
	struct addrinfo addrHints, *serverInfo, *p;

	// Holds the return value of getaddrinfo system call
	int returnValue;

	//Address can have a maximum of IPv6 addr string length (used a macro available in one of std headers)
	char addrString[INET6_ADDRSTRLEN]; 

	// HTTP request to be sent, mostly will be something like "GET / HTTP/1.1\r\n\r\n"
	char sendLine[MAX_BUFFER_SIZE];

	// HTTP response holder "<html></html>"
	char recvLine[MAX_BUFFER_SIZE];

	if (argc != 2)
	{
		fprintf(stderr, "usage: ./webClient <IP_ADDRESS> (or) ./webClient <hostname>\n");
		exit(1);
	}

	memset(&addrHints, 0, sizeof addrHints);
	addrHints.ai_family = AF_UNSPEC; //need not specify it
	addrHints.ai_socktype = SOCK_STREAM;

	if ((returnValue = getaddrinfo(argv[1], SERVER_PORT, &addrHints, &serverInfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(returnValue));
		exit(1);
	}

	// Iterating a linked list
	for (p = serverInfo; p != NULL ; p = p->ai_next)
	{
		if ( (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1 )
		{
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("client: connect");
			continue;
		}	

		break;
	}

	if (p == NULL)
	{
		fprintf(stderr, "client: failed to connect");
		exit(2);
	}

	// DNS Resolution done

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr* )p->ai_addr),
		addrString, sizeof addrString);
	printf("Connected to %s\n", addrString);

	freeaddrinfo(serverInfo);

	/*


		THE START OF SENDING AND RECEIVING GET REQUEST AND RESPONSE



	*/



	// Close the connection

	close(sockfd);

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
