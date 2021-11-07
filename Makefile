CC	= gcc
SRC	= main.c \
	  read_options.c \
	  initialize.c \
	  iterate.c \
	  finalize.c

HDR	= header.h

BIN	= rede2

DFLAGS	= -ggdb3
CFLAGS	= -Ofast -Wall -Werror -pedantic
LDFLAGS	= -lm -lmvec

.PHONY: all clean

all: $(BIN)

clean: $(BIN)
	rm -rf $(BIN)

$(BIN): $(SRC) $(HDR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(DFLAGS) $(SRC) -o $@



