#ifndef _SCHEDULING_H
#define _SCHEDULING_H
#define PIT_IRQ_NUM  0
#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL_PORT 0x40
#define WAVE_MODE   0x36
#define DEFAULT 1193181
#define PERIOD 10
#define PIT_IRQ_NUM 0
extern uint8_t next_term;
extern uint8_t cur_term;

void init_pit();            // initialize the pit
void pit_handler();         // handler of pit interrupt

#endif

