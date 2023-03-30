#include "ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <locale.h>
#include <sys/ioctl.h>

static struct termios initialattr;
static struct winsize ttydimensions;

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
	uishowcur();
	return tcsetattr(STDIN_FILENO, TCSADRAIN, &initialattr);
}

static void uiatexit(void) {
	uirestoretty();
}

static void uisighandler(int sig) {
	signal(sig, SIG_IGN);
	uirestoretty();
	exit(0);
}

static void uiwinchhandler(int sig) {
	signal(sig, SIG_IGN);
	ioctl(STDERR_FILENO, TIOCGWINSZ, &ttydimensions);
	signal(sig, uiwinchhandler);
}

bool uiinit(void) {
	if (!isatty(STDERR_FILENO))
		return false;

	setlocale(LC_ALL, "");
	uirawtty();
	atexit(uiatexit);
	signal(SIGINT, uisighandler);
	signal(SIGTERM, uisighandler);
	signal(SIGWINCH, uiwinchhandler);
	ioctl(STDERR_FILENO, TIOCGWINSZ, &ttydimensions);
	return true;
}

void uidimensions(unsigned short int wh[2]) {
	wh[0] = ttydimensions.ws_col;
	wh[1] = ttydimensions.ws_row;
}

/// trashes SGR state
void uistatusd(int line, const char *fmt, ...) {
	va_list ap;

	fprintf(stderr, "\033[s\033[0m\033[%d;1H\033[K", line);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\033[u");
}


void uileft(unsigned int n) {
	uiprintf(ANSI_CSI "%u" "D", n);
}

void uiright(unsigned int n) {
	uiprintf(ANSI_CSI "%u" "C", n);
}

void uiup(unsigned int n) {
	uiprintf(ANSI_CSI "%u" "A", n);
}

void uidown(unsigned int n) {
	uiprintf(ANSI_CSI "%u" "B", n);
}

void uimovh(int n) {
	if (n > 0)
		uiright(n);
	else if (n < 0)
		uileft(-n);
}

void uimovv(int n) {
	if (n > 0)
		uidown(n);
	else if (n < 0)
		uiup(-n);
}

void uimovrel(int x, int y) {
	uimovh(x); uimovv(y);
}

void uicurs(void) {
	uiprintf(ANSI_CURS);
}

void uicurl(void) {
	uiprintf(ANSI_CURL);
}

void uihidecur(void) {
	uiprintf(ANSI_CSI "?25l");
}

void uishowcur(void) {
	uiprintf(ANSI_CSI "?25h");
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
