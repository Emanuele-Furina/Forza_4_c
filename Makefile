CFLAGS=-std=c99 -Wall -Wextra -Wpedantic -Os -g3
LDFLAGS=
CPPFLAGS=

BINDIR=output

SRC=$(wildcard *.c)
OBJ=$(addprefix $(BINDIR)/, $(SRC:.c=.o))
DEP=$(OBJ:.o=.d)
BIN=$(BINDIR)/forza_quattro

all: $(BIN)

run: $(BIN)
	./$^

$(BIN): $(OBJ)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^

$(BINDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c -MMD $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -rf $(BINDIR)

-include $(DEP)
