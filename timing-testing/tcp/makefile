CC = clang
CFLAGS = -Wall --pedantic
LDFLAGS = -lcriu

.PHONY: all
all: client server launcher

client: client.o

server: server.o

server.o: server.c

client.o: client.c

launcher: launcher.o

launcher.o: launcher.c

.PHONY: prep
prep:
	mkdir -p /tmp/criu/client
	mkdir -p /tmp/criu/server

.PHONY: clean
clean:
	rm -rf *.o
