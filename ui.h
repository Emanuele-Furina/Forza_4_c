#ifndef F4_UI_H
#define F4_UI_H

#define ANSI_CSI "\033["
#define ANSI_CURS ANSI_CSI "s"
#define ANSI_CURL ANSI_CSI "u"
#define ANSI_CLR ANSI_CSI "K"

#define SGR_RESET ANSI_CSI "0m"
#define SGR_INVERT ANSI_CSI "7m"
#define SGR_BLINK ANSI_CSI "5m"

#define FG_256 ANSI_CSI "38;5;"
#define BG_256 ANSI_CSI "48;5;"
#define FG_BLK FG_256  "0m"

void uiinit(void);
int uirawtty(void);
int uirestoretty(void);

void uileft(int n);
void uiright(int n);
void uiup(int n);
void uidown(int n);

void uicurs(void);
void uicurl(void);

char uigetchar(void);

void uiprintf(const char *fmt, ...);

#endif /* F4_UI_H */
