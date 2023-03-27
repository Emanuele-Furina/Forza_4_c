#include "ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <locale.h>

#define ANSI_CSI "\e["
#define ANSI_CURS ANSI_CSI "s"
#define ANSI_CURL ANSI_CSI "u"

#define SGR_RESET ANSI_CSI "0m"
#define SGR_RED ANSI_CSI "38;5;9m"
#define SGR_BLU ANSI_CSI "38;5;12m"

const char *state_visual[CELL_STATE_LENGTH] = {
	[CELL_EMPTY] = SGR_RESET " " SGR_RESET,
	[CELL_PLAYER1] = SGR_RED "O" SGR_RESET,
	[CELL_PLAYER2] = SGR_BLU "O" SGR_RESET,
};

static struct termios initialattr;

int uirawtty(void) {
	struct termios raw;

	if (tcgetattr(STDIN_FILENO, &initialattr) != 0) return -1;
	raw = initialattr;

	raw.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP |
	                 INLCR | IGNCR | ICRNL | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	raw.c_cflag &= ~(CSIZE | PARENB);
	raw.c_cflag |= CS8;

	raw.c_cc[VMIN] = 1;
	raw.c_cc[VTIME] = 0;

	return tcsetattr(STDIN_FILENO, TCSADRAIN, &raw);
}

int uirestoretty(void) {
	return tcsetattr(STDIN_FILENO, TCSADRAIN, &initialattr);
}

static void uiatexit(void) {
	uirestoretty();
}

static void uisighandler(int sig) {
	signal(sig, NULL);
	uirestoretty();
	exit(0);
}

void uiinit(void) {
	setlocale(LC_ALL, "");
	uirawtty();
	atexit(uiatexit);
	signal(SIGINT, uisighandler);
	signal(SIGTERM, uisighandler);
}

void uileft(int n) {
	uiprintf(ANSI_CSI "%d" "D", n);
}

void uiright(int n) {
	uiprintf(ANSI_CSI "%d" "C", n);
}

void uiup(int n) {
	uiprintf(ANSI_CSI "%d" "A", n);
}

void uidown(int n) {
	uiprintf(ANSI_CSI "%d" "B", n);
}

void uicurs(void) {
	uiprintf(ANSI_CURS);
}

void uicurl(void) {
	uiprintf(ANSI_CURL);
}

char uigetchar(void) {
	char c;
	while (read(STDIN_FILENO, &c, 1) != 1);

	if (c == initialattr.c_cc[VINTR] || c == initialattr.c_cc[VQUIT])
		exit(0);

	return c;
}

void uiprintf(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}
