#include "ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <locale.h>

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
