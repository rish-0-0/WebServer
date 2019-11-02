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
#include <stdbool.h>

#include "webServerInclude.h"
#include "serverQueue.h"

// Thread related initialization
pthread_t thread_pool[THREAD_POOL_SIZE];

// Making enqueue and dequeue thread safe with mutex locks
pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;

// THREAD condition variable for removing inefficiency caused by busy waiting
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;


void* get_in_addr(struct sockaddr* sa);
void* thread_function(void* args);
void handle_connection(int* p_client);

int main()
{

	/*


		WILL BE MAKING A MULTI-THREADED WEB SERVER RUNNING ON 8080 (SERVER_PORT) PORT to avoid superuser priveleges

		HAVE TO SET UP A THREAD POOL (around 35 threads will be fine, SERVER BACKLOG WILL BE AROUND 100 connections)

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
	char client_addr_string[INET6_ADDRSTRLEN];
	socklen_t sin_size;
	int return_value;

	/*



	INITIALIZING THREAD POOL



	*/
	for (int i=0; i<THREAD_POOL_SIZE; i++)
	{
		pthread_create(&thread_pool[i], NULL, thread_function, NULL);
	}

	/*

	END OF INITIALIZING THREAD POOL


	*/

	int allow_port_reuse = 1;

	memset(&server_addr_hints, 0, sizeof server_addr_hints);
	server_addr_hints.ai_family = AF_UNSPEC;
	server_addr_hints.ai_socktype = SOCK_STREAM;
	// USE MY IP
	server_addr_hints.ai_flags = AI_PASSIVE; 

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

	printf("Waiting for connections on port %s ... \n", SERVER_PORT);



	while (true)
	{
		sin_size = sizeof client_addr;
		if ( (clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &sin_size)) == -1 )
		{
			perror("accept: connection failed\n");
			continue;
		}

		// EXTRACT CLIENT ADDRESS

		inet_ntop(client_addr.ss_family,
		 get_in_addr((struct sockaddr*)&client_addr),
	 	 client_addr_string, sizeof client_addr_string);

		printf("SERVER: connection from %s\n", client_addr_string);

		// Pointer to clientfd

		int* p_client = malloc(sizeof(int));
		*p_client = clientfd;
		pthread_mutex_lock(&mutex_lock);
		enqueue(p_client);
		pthread_cond_signal(&condition_var);
		pthread_mutex_unlock(&mutex_lock);


		/*

			THREAD HANDLING
	
		*/

	}

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

void* thread_function(void* args)
{
	while(true)
	{
		int* p_client;
		pthread_mutex_lock(&mutex_lock);
		if ( (p_client = dequeue()) == NULL)
		{
			// Only wait if there is no other work on the queue
			pthread_cond_wait(&condition_var,&mutex_lock);
			// Once the signal arrives this next line of code will execute
			// Try again
			p_client = dequeue();
		}
		pthread_mutex_unlock(&mutex_lock);

		if (p_client != NULL)
		{
			handle_connection(p_client);
		}
	}
}

void handle_connection(int* p_client)
{
	printf("Thread in use\n");
	free(p_client);
}