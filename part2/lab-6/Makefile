CC = gcc
CFLAGS = -Wall

SERVER_SRC = server.c
CLIENT_SRC = client.c

SERVER_EXE = server
CLIENT_EXE = client

all: $(SERVER_EXE) $(CLIENT_EXE)

$(SERVER_EXE): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER_EXE) $(SERVER_SRC)

$(CLIENT_EXE): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT_EXE) $(CLIENT_SRC)

clean:
	rm -f $(SERVER_EXE) $(CLIENT_EXE)