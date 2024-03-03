#include "signal.h"
#include "lib.h"
#include "system_calls.h"
#include "page.h"
#include "terminal.h"
/*
*   kill_task
*   description: kill the task
*   input: None
*   output: None
*   side effect: kill the task
*/
void 
kill_task()
{

    // putc_by_keyboard('\n');
    // putc_by_keyboard('E');
    // putc_by_keyboard('c');
    // putc_by_keyboard('h');
    // putc_by_keyboard('O');
    // putc_by_keyboard('S');
    // putc_by_keyboard(':');
    // putc_by_keyboard(' ');
    // putc_by_keyboard('K');
    // putc_by_keyboard('i');
    // putc_by_keyboard('l');
    // putc_by_keyboard('l');
    // putc_by_keyboard('e');
    // putc_by_keyboard('d');
    // putc_by_keyboard(' ');
    // putc_by_keyboard('b');
    // putc_by_keyboard('y');
    // putc_by_keyboard(' ');
    // putc_by_keyboard('S');
    // putc_by_keyboard('I');
    // putc_by_keyboard('G');
    // putc_by_keyboard('I');
    // putc_by_keyboard('N');
    // putc_by_keyboard('T');
    // putc_by_keyboard('!');    
    // putc_by_keyboard('\n');
    printf("\n EchOS: Killed by SIGINT!\n");
    halt(0); //halt the program
    return;
}

/*
*   signal_ignore
*   description: ignore the signal
*   input: None
*   output: None
*   side effect: ignore the signal
*/
void
signal_ignore()
{   
    //putc_by_keyboard('M');
    return;
}

// void 
// ssss(){
//     PCB_t* pcb=get_current_pcb();
//     int i;
//     for (i=0;i<SIGNAL_NUM;i++)
//     {
//         pcb->signal_mask[i]=UNMASK; //unmask all signals
//     }
//     putc_by_keyboard('e');
//     return;
// }

void
left_key_mouse(){
    //uint8_t* badbuf1 = 0;
    // putc_by_keyboard('\n');
    // putc_by_keyboard('U');
    // putc_by_keyboard('s');
    // putc_by_keyboard('e');
    // putc_by_keyboard('r');
    // putc_by_keyboard('1');
    // putc_by_keyboard(' ');
    // putc_by_keyboard('s');
    // putc_by_keyboard('i');
    // putc_by_keyboard('g');
    // putc_by_keyboard('n');
    // putc_by_keyboard('a');
    // putc_by_keyboard('l');
    // putc_by_keyboard(':');
    // putc_by_keyboard('M');
    // putc_by_keyboard('o');
    // putc_by_keyboard('u');
    // putc_by_keyboard('s');
    // putc_by_keyboard('e');
    // putc_by_keyboard(' ');
    // putc_by_keyboard('l');
    // putc_by_keyboard('e');
    // putc_by_keyboard('f');
    // putc_by_keyboard('t');
    // putc_by_keyboard(' ');
    // putc_by_keyboard('c');
    // putc_by_keyboard('l');
    // putc_by_keyboard('i');
    // putc_by_keyboard('c');
    // putc_by_keyboard('k');
    // putc_by_keyboard('\n');
    // putc_by_keyboard('T');
    // putc_by_keyboard('e');
    // putc_by_keyboard('r');
    // putc_by_keyboard('m');
    // putc_by_keyboard(' ');
    // putc_by_keyboard('@');
    // putc_by_keyboard(' ');
    // putc_by_keyboard('E');
    // putc_by_keyboard('c');
    // putc_by_keyboard('h');
    // putc_by_keyboard('O');
    // putc_by_keyboard('S');
    // putc_by_keyboard('>');
    // putc_by_keyboard(' ');
    //set_handler(3,ssss);
    //(*badbuf1) = 1;
    printf("\nUSR1 SIGNAL: Mouse Left button Clicked \n");
    update_current_global_time();
    if(minute>=10){
        printf("Time: %u-%u-%u %u:%u \n", year,month,day,hour,minute);}
        else{
        printf("Time: %u-%u-%u %u:0%u \n", year,month,day,hour,minute);
        }
    printf("Term%d @ EchOS> ", visible_term);
    return;
}
void
right_key_mouse(){
    //uint8_t* badbuf1 = 0;
    // putc_by_keyboard('\n');
    // putc_by_keyboard('U');
    // putc_by_keyboard('s');
    // putc_by_keyboard('e');
    // putc_by_keyboard('r');
    // putc_by_keyboard('2');
    // putc_by_keyboard(' ');
    // putc_by_keyboard('s');
    // putc_by_keyboard('i');
    // putc_by_keyboard('g');
    // putc_by_keyboard('n');
    // putc_by_keyboard('a');
    // putc_by_keyboard('l');
    // putc_by_keyboard(':');
    // putc_by_keyboard('M');
    // putc_by_keyboard('o');
    // putc_by_keyboard('u');
    // putc_by_keyboard('s');
    // putc_by_keyboard('e');
    // putc_by_keyboard(' ');
    // putc_by_keyboard('r');
    // putc_by_keyboard('i');
    // putc_by_keyboard('g');
    // putc_by_keyboard('h');
    // putc_by_keyboard('t');
    // putc_by_keyboard(' ');
    // putc_by_keyboard('c');
    // putc_by_keyboard('l');
    // putc_by_keyboard('i');
    // putc_by_keyboard('c');
    // putc_by_keyboard('k');
    // putc_by_keyboard('\n');
    // putc_by_keyboard('T');
    // putc_by_keyboard('e');
    // putc_by_keyboard('r');
    // putc_by_keyboard('m');
    // putc_by_keyboard(' ');
    // putc_by_keyboard('@');
    // putc_by_keyboard(' ');
    // putc_by_keyboard('E');
    // putc_by_keyboard('c');
    // putc_by_keyboard('h');
    // putc_by_keyboard('O');
    // putc_by_keyboard('S');
    // putc_by_keyboard('>');
    // putc_by_keyboard(' ');
    //set_handler(3,ssss);
    //(*badbuf1) = 1;printf("\nUSR1 SIGNAL: Mouse Left button Clicked \n");
    printf("\nUSR2 SIGNAL: Mouse Right button Clicked \n");
    update_current_global_time();
       if(minute>=10){
        printf("Time: %u-%u-%u %u:%u \n", year,month,day,hour,minute);}
        else{
        printf("Time: %u-%u-%u %u:0%u \n", year,month,day,hour,minute);
        }
    printf("Term%d @ EchOS> ", visible_term);
    return;
}

/*
*   signal_handler
*   description: handle the signal
*   input: None
*   output: None
*   side effect: handle the signal
*/
void 
make_signal(uint32_t signal_number){
    PCB_t* pcb;
    cli();
    if ((signal_number==SIG_INTERRUPT) || (signal_number== USER1) || (signal_number== USER2))
    {
        pcb=schedule_pcb[visible_term];  // need change based on scheduling
    }
    else{
        pcb=get_current_pcb(); //get the current pcb
    }
    // pcb=get_current_pcb(); //get the current pcb
    pcb->signal_status[signal_number]=ON; //set the signal status to ON
    sti();
    return;
}

/*
*   signal_handler
*   description: handle the signal
*   input: None
*   output: None
*   side effect: handle the signal
*/
void
signal_handler()
{
    PCB_t* current_pcb=get_current_pcb();
    int32_t i=0;
    int32_t signalnumber=-1; //the signal number, -1 means no signal
    uint32_t return_handler_size;
    //register uint32_t uebp asm("ebp"); //get the saved ebp
    uint32_t uebp;
    asm volatile(
        "movl %%ebp, %0" //get the saved ebp
        :"=r"(uebp)
        ::"memory"
    );
    hardware_context_t* hwcontext=(hardware_context_t*)(uebp+8);//pointer to hwcontext
    uint32_t uesp = hwcontext->esp;
    uint32_t ret_addr;
    //find the signal
    while(i<SIGNAL_NUM){
        //if there is a signal
        if(current_pcb->signal_status[i]==ON){
            current_pcb->signal_status[i]=OFF; //set the signal status to OFF
            signalnumber=i;
            break;
        }
        i++;
    }
    //if there is no signal, return
    if (signalnumber==-1){
        return;
    }
    //if the signal is not masked, call the handler
    else if (current_pcb->handler[signalnumber]==kill_task||current_pcb->handler[signalnumber]==signal_ignore||current_pcb->handler[signalnumber]==left_key_mouse || current_pcb->handler[signalnumber]==right_key_mouse ){
        current_pcb->handler[signalnumber](); //call the handler
        return;
    }
    return_handler_size = link_handler_sig_return_end - link_handler_sig_return; //get the size of handler
    if(uesp<USER_ESP-BIG_PAGE_SIZE||uesp>USER_ESP){
        if(signalnumber==ALARM){
            current_pcb->handler[signalnumber](signalnumber); //call the alarm handler
            return;
        }
        return; //if the user stack is not valid, return
    }
    ret_addr = uesp-return_handler_size; //get the return address
    //build the hardware context
    build_hwcontext((uint32_t*)(&ret_addr),(uint32_t*) (&signalnumber), hwcontext, return_handler_size); 

    //set the return address to the handler
    hwcontext->ret_addr =(uint32_t)current_pcb->handler[signalnumber]; 
    uesp = uesp-return_handler_size-sizeof(hardware_context_t)-8; //update the user stack pointer
    hwcontext->esp = uesp; //set the user stack pointer 
    return;
}
