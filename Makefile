CFLAGS=-std=gnu99 -Wall -Wextra -Os -g3
LDFLAGS=
CPPFLAGS=

BINDIR=output

SRC=forza_quattro.c
BIN=$(BINDIR)/forza_quattro

all: $(BIN)

run: $(BIN)
	./$^

$(BIN): $(SRC)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm -rf $(BINDIR)
