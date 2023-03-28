#ifndef F4_UI_H
#define F4_UI_H

#include "definitions.h"

extern const char *state_visual[CELL_STATE_LENGTH];
extern const char *turn_visual[CELL_STATE_LENGTH];
extern const char *won_visual[CELL_STATE_LENGTH];
extern const char *invert_visual[CELL_STATE_LENGTH];

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
