CC = gcc
CFLAGS = -Wall -Werror

SRCS = src/main.c src/process.c
OBJS = $(SRCS:.c=.o)
EXECUTABLE = ps_analog

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXECUTABLE)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXECUTABLE)