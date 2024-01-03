#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

#define  keyboard_port_DATA 0x60 //port to receive data

// #define  keyboard_port_STATUS 0x64 //port to receive status
// below are the scan codes for keyboard
#define left_shift 0x2A
#define left_shift_release 0xAA
#define right_shift 0x36
#define right_shift_release 0xB6
#define caps_lock 0x3A
#define caps_lock_release 0xBA
#define left_ctrl 0x1D
#define left_ctrl_release 0x9D
#define left_alt 0x38
#define left_alt_release 0xB8
#define num_lock 0x45
#define num_lock_release 0xC5

#define f1 0x3B
#define f2 0x3C
#define f3 0x3D

#define up_arrow 0x48
#define up_arrow_release 0xC8
#define down_arrow 0x50
#define down_arrow_release 0xD0
#define left_arrow 0x4B
#define right_arrow 0x4D

#define KEYBOARD_IRQ_NUM 1 //IR1

void keyboard_init();
void handler_keyboard();
int set_keyboard_status(uint8_t scan);


#endif
