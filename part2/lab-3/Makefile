CC = gcc
CFLAGS = -Wall -Werror

SRCS = src/main.c src/process.c
OBJS = $(SRCS:.c=.o)
EXECUTABLE = my_ps

.PHONY: all clean test

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXECUTABLE)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(EXECUTABLE)
	./$(EXECUTABLE) -l

clean:
	rm -f $(OBJS) $(EXECUTABLE)