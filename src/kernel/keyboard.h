#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "buf.h"
#include "liste.h"
#include "process.h"

#define KEYBOARD_BUFFER_SIZE 20

#define EOL 13
#define BACKSPACE 127
#define BS 8
#define HT 9
#define LF 10
//#define CR 13
#define NAK 21
#define ETB 23

void init_keyboard(void);
void handle_keyboard_interrupt();
void handle_echo(char c);

void cons_echo(int on);
int cons_read(char *string, unsigned long length);

#endif /* __KEYBOARD_H__ */