OBJS=server.o

all: server
server: $(OBJS)
	gcc -o $@ $^
%.o: %.c
	gcc -c -o $@ $^
clean:
	rm server
