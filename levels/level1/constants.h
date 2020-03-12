#ifndef WEBSERVER_INCLUDE_H
#define WEBSERVER_INCLUDE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>

#define SERVER_PORT "8090"
#define SERVER_MAX_CONNECTIONS 100
#define MAX_BUFFER_SIZE 100

void* get_in_addr(struct sockaddr* sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


#endif