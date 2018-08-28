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

Start `criu server`. Run `./launcher 8080` (or whatever port you want).


## Problems

~~Running the 2nd line again after it has been run already SHOULD work but currently doesn't. This needs to be fixed.~~ This was fixed by adding iptable rules preventing resets.

## Results

Running `time ./launcher 8080` will yield results of just below 10s realtime for 100 iterations, just below 19s for 200 iterations, ... on my laptop and similar times on a different machine, so a linear increase of 0.1s per iterations seems likely. This is with stdout output and could possibly improved by detaching from the shell though previous such trials showed little improvements.
