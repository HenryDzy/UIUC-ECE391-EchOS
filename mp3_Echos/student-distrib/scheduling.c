#include "system_calls.h"
#include "types.h"
#include "lib.h"
#include "x86_desc.h"
#include "page.h"
#include "terminal.h"
#include "rtc.h"
#include "filesystem.h"
#include "scheduling.h"
#include "i8259.h"
uint8_t cur_term=0;
uint8_t next_term=1;
uint8_t term_num=1;

// scheduling
// input: none
// output: none
// side effect: change the program pagging and vedio paging, and switch to the next process
void scheduling(){
    PCB_t* cur_pcb;
    PCB_t* next_pcb;
    cur_pcb=get_current_pcb();
    asm volatile (    
		"movl %%esp, %0\n\
         movl %%ebp, %1 \n "
		:"=r"(cur_pcb->schedule_esp),"=r"(cur_pcb->schedule_ebp):: "memory");    //get the context
    cur_term=next_term;
    next_pcb = schedule_pcb[next_term];
    next_term=(next_term+1)%3;          // get the next terminal, 3 is the number of terminals
    if(term_num<3){                     // we execute 3 terminals from start
        term_num++;
        //cli();
        change_term_vidmap(cur_term);
        clear_vidmap(cur_term);
        execute((uint8_t*)"shell");
    }
    user_paging(next_pcb->pid);             //set next user paging
    if(cur_term == visible_term) change_term_vidmap_same(); // change the video memory
    else change_term_vidmap(cur_term);              // change vedio memory
    tss.ss0 = KERNEL_DS;
    tss.esp0=(uint32_t)(KENEL_BOT-BLOCK*(next_pcb->pid))-4;   // set tss, 4 is the size of the stack
    asm volatile(
        "movl %0,%%esp \n\
        movl %1,%%ebp"
        ::"r"(next_pcb->schedule_esp),"r"(next_pcb->schedule_ebp):"esp","ebp"
    );
    asm volatile("leave");
    asm volatile("ret");
}
// initialize the pit
// input: none
// output: none
// side effect: out data to the two ports and enable irq0
void init_pit(){
    uint32_t freq = DEFAULT/PERIOD; // THIS IS 1193180/100
    outb(WAVE_MODE,PIT_COMMAND_PORT);
    outb(freq&0x00FF,PIT_CHANNEL_PORT); // set the low 16 bits of the freq
    outb(freq>>8,PIT_CHANNEL_PORT); // set the high 16 bits of the freq, 8 is the size of the byte
    enable_irq(PIT_IRQ_NUM);
    return;
}
// handler of pit interrupt
// input: none
// output: none
// side effect: send eoi to irq0
void pit_handler(){
    send_eoi(PIT_IRQ_NUM);
    scheduling();
}
