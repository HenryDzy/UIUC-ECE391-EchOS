#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

#define  keyboard_port_DATA 0x60 //port to receive data


#define KEYBOARD_IRQ_NUM 1 //IR1

void keyboard_init();
void handler_keyboard();





#endif
