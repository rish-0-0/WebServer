OBJS=server.o serverQueue.o
CFLAGS=-lpthread

all: server
server: $(OBJS)
	gcc -o $@ $^ $(CFLAGS)
%.o: %.c
	gcc -c -o $@ $^
clean:
	rm server
