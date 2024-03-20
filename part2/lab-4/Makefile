CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS =

TARGET = mydaemon
SOURCES = src/daemon.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run:
	./$(TARGET)