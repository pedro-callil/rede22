CC	= gcc
CC_W	= x86_64-w64-mingw32-gcc
SRC	= main.c \
	  read_options.c \
	  initialize.c \
	  iterate.c \
	  print_and_write.c \
	  finalize.c

HDR	= header.h

BIN	= rede22
BINW	= rede22.exe

DFLAGS	= -ggdb3
CFLAGS	= -Ofast -Wall -Werror -Wextra -pedantic -std=gnu99

LDFLAGS		= -lm -lmvec
LDFLAGSW	= -lm

.PHONY: all clean windows

all: $(BIN) $(BINW)

clean:
	rm -rf $(BIN) $(BINW)

windows: $(BINW)

$(BIN): $(SRC) $(HDR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(DFLAGS) $(SRC) -o $@

$(BINW): $(SRC) $(HDR)
	$(CC_W) $(CFLAGS) $(LDFLAGSW) $(DFLAGS) $(SRC) -o $@

