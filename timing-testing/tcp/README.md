# TCP C/R testing

Most simple test for multiple CRIU restores of a connected TCP connection. `server` listens on port 8080, `client` connects to port 8080, sends some data, `server` responds with the current unix time, `client prints the response.

## Prerequisites

libcriu

## Building

```
make all
make prep
```

## Running

Execute in parallel, line-by-line, first block always first, as root (sudo might not suffice):

```
$ ./server
$ criu restore --tcp-established --shell-job -v4 --images-dir /tmp/criu/server
```

```
$ ./client
$ criu restore --tcp-established --shell-job -v4 --images-dir /tmp/criu/client
```

## Problems

Running the 2nd line again after it has been run already SHOULD work but currently doesn't. This needs to be fixed.
