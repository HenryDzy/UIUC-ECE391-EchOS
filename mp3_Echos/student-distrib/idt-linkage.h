#ifndef IDT_LNK_H
#define IDT_LNK_H

#include "keyboard.h"
#include "rtc.h"
#include "idt.h"
#include "system_calls.h"
#include "scheduling.h"
#include "signal.h"

extern void link_handler_keyboard(void);
extern void link_handler_rtc(void);
extern void link_handler_sysc(void);
extern void link_handler_pit(void);
extern void link_handler_mouse(void);
#endif
