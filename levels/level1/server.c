#include "constants.h"

void* get_in_addr(struct sockaddr* sa);

int main() {

	int sockfd, clientfd;
	struct addrinfo server_addr_hints, *serverinfo, *p;
	struct sockaddr_storage client_addr;
	char client_addr_string[INET6_ADDRSTRLEN];
	socklen_t sin_size;
	int return_value;

	int allow_port_reuse = 1;
	memset(&server_addr_hints, 0, sizeof server_addr_hints);
	server_addr_hints.ai_family = AF_UNSPEC;
	server_addr_hints.ai_socktype = SOCK_STREAM;
	server_addr_hints.ai_flags = AI_PASSIVE;

	if ( (return_value = getaddrinfo(NULL, SERVER_PORT, &server_addr_hints, &serverinfo) ) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(return_value));
		exit(1);
	}

	// Iterate the linked list returned by getaddrinfo whose head pointer is in serverinfo
	for (p = serverinfo; p != NULL; p = p->ai_next) {
		if ( (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("server: socket\n");
			// exit(1);
			continue;
		}

		if ( (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &allow_port_reuse, sizeof(int))) == -1) {
			perror("server: setsockopt\n");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(serverinfo);

	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		exit(2);
	}

	if (listen(sockfd, SERVER_MAX_CONNECTIONS) == -1) {
		perror("server: listen");
		exit(2);
	}

	printf("Waiting for connections on port %s ... \n", SERVER_PORT);

	char buffer[MAX_BUFFER_SIZE];
	int receivedBytes;
	// Main Loop
	while (true) {

		sin_size = sizeof client_addr;
		if ( (clientfd = accept(sockfd, (struct sockaddr* )&client_addr, &sin_size)) == -1) {
			perror("server: accept: Connection failed\n");
			continue;
		}


		// Extract client address
		inet_ntop(client_addr.ss_family, 
			get_in_addr((struct sockaddr* )&client_addr),
			client_addr_string, sizeof client_addr_string);

		printf("server: GOT Connection from: %s\n", client_addr_string);

		if ( (receivedBytes = recv(clientfd, buffer, MAX_BUFFER_SIZE - 1, 0)) == -1) {
			perror("server: recv\n");
			continue;
		}
		buffer[receivedBytes] = 0;
		printf("server: received from client: %s\n", buffer);

		close(clientfd);

	}

	return 0;
}

