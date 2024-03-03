#ifndef SIGNAL_H
#define SIGNAL_H

#include "types.h"
#include "lib.h"
#include "x86_desc.h"
#include "page.h"
#include "terminal.h"
#include "rtc.h"
#include "filesystem.h"
#include "scheduling.h"
#include "system_calls.h"
#include "idt-linkage.h"


#define DIV_ZERO 0
#define SEGFAULT 1
#define INTERRUPT 2
#define ALARM 3
#define USER1 4
#define USER2 5
#define MASK 1
#define ON 1
#define OFF 0

#define SIG_DIV_ZERO 0
#define SIG_SEGFAULT 1
#define SIG_INTERRUPT 2
#define SIG_ALARM 3
#define SIG_USER1 4
#define SIG_USER2 5

extern void kill_task();
extern void signal_ignore();
extern void left_key_mouse();
extern void signal_handler();
extern void make_signal(uint32_t signal_number);
extern void link_handler_sig_return(void);
extern void link_handler_sig_return_end(void);
extern void right_key_mouse();
#endif
