CC = gcc
CFLAGS = -Wall -Wextra -pthread

all: multithreaded singlethreaded

multithreaded: sort_multithread.c
	$(CC) $(CFLAGS) -o multithreaded sort_multithread.c

singlethreaded: sort.c
	$(CC) $(CFLAGS) -o singlethreaded sort.c

clean:
	rm -f multithreaded singlethreaded