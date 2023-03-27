CFLAGS=-std=gnu99 -Wall -Wextra -Os -g3
LDFLAGS=
CPPFLAGS=

BINDIR=output

SRC=forza_quattro.c
OBJ=$(addprefix $(BINDIR)/, $(SRC:.c=.o))
BIN=$(BINDIR)/forza_quattro

all: $(BIN)

run: $(BIN)
	./$^

$(BIN): $(OBJ)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^

$(BINDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -rf $(BINDIR)
