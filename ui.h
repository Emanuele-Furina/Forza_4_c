#ifndef F4_UI_H
#define F4_UI_H

#include <stdbool.h>

#define ANSI_CSI "\033["
#define ANSI_CURS ANSI_CSI "s"
#define ANSI_CURL ANSI_CSI "u"
#define ANSI_CLR ANSI_CSI "K"

#define SGR_RESET ANSI_CSI "0m"
#define SGR_BLINK ANSI_CSI "5m"

#define FG_256 ANSI_CSI "38;5;"

bool uiinit(void);
int uirawtty(void);
int uirestoretty(void);

void uidimensions(unsigned short int wh[2]);

void uileft(unsigned int n);
void uiright(unsigned int n);
void uiup(unsigned int n);
void uidown(unsigned int n);

void uimovh(int n);
void uimovv(int n);
void uimovrel(int x, int y);

void uicurs(void);
void uicurl(void);

void uihidecur(void);
void uishowcur(void);

char uigetchar(void);

void uiprintf(const char *fmt, ...);

void uistatusd(int line, const char *fmt, ...);

#endif /* F4_UI_H */
