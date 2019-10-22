all: server client
server: webServer.c
	gcc -o server webServer.c
client: webClient.c
	gcc -o client webClient.c
clean:
	rm client server
