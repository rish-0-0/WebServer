CFLAGS=-lm
FILES=server client
all: $(FILES)
%: %.o
	gcc -o $@ $^ $(CFLAGS)
%.o: %.c 
	gcc -c -o $@ $^
clean:
	rm $(FILES)