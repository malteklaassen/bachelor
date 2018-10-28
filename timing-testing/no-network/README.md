# no-restore C/R testing

Most simple test for multiple CRIU restores with varying amounts of used memory.

## Prerequisites

libcriu

## Building

```
make all
make prep
```

## Running

Start `criu server`. Run `./launcher 1000000 100` (or whatever port you want).
