#include "constants.h"

int main() {

	int sockfd;
	struct addrinfo socket_addr_hints, *socketinfo, *p;
	char addr[INET6_ADDRSTRLEN];
	int return_value;

	int allow_port_reuse = 1;
	memset(&socket_addr_hints, 0, sizeof socket_addr_hints);
	socket_addr_hints.ai_family = AF_UNSPEC;
	socket_addr_hints.ai_socktype = SOCK_STREAM;
	socket_addr_hints.ai_flags = AI_PASSIVE;

	if ( (return_value = getaddrinfo(NULL, SERVER_PORT, &socket_addr_hints, &socketinfo)) != 0) {
		fprintf(stderr, "client: getaddrinfo: %s\n", gai_strerror(return_value));
		exit(1);
	}

	// Iterate the linked list returned by getaddrinfo whose head is p;

	for (p = socketinfo; p != NULL; p = p->ai_next) {
		if ( (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("client: socket\n");
			continue;
		}

		if ( (connect(sockfd, p->ai_addr, p->ai_addrlen)) == -1) {
			perror("client: connect\n");
			close(sockfd);
			continue;
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		exit(2);
	}

	inet_ntop(p->ai_family,
		get_in_addr((struct sockaddr* )p->ai_addr),
		addr, sizeof addr);

	printf("client: connecting to %s\n\n", addr);

	freeaddrinfo(socketinfo);

	char buffer[MAX_BUFFER_SIZE];

	char a[] = "B";
	int sentBytes;
	if ( (sentBytes = send(sockfd, a, strlen(a), 0)) == -1) {
		perror("client: send\n");
		exit(3);
	}

	printf("Sent something to server\n");

	int receivedNum;

	if ((receivedNum = recv(sockfd, buffer, MAX_BUFFER_SIZE - 1, 0)) == -1) {
		perror("client: recv\n");
		exit(1);
	}

	buffer[receivedNum] = 0;

	printf("client: received '%s' \n", buffer);

	close(sockfd);

	return 0;
}