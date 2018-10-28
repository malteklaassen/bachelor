CC = clang
CFLAGS = -Wall --pedantic
LDFLAGS = -lcriu

.PHONY: all
all: client launcher

client: client.o

client.o: client.c

launcher: launcher.o

launcher.o: launcher.c

.PHONY: prep
prep:
	mkdir -p /tmp/criu/client

.PHONY: clean
clean:
	rm -rf *.o
