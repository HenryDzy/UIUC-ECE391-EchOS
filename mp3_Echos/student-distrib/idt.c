#include "x86_desc.h"
#include "idt-linkage.h"
#include "idt.h"
#include "rtc.h"
#include "keyboard.h"
#include "lib.h"
#include "system_calls.h"
#include "scheduling.h"

/* handler_DE
 * 
 * Deal with exception: Divide Error
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */

// halt(1) means exception halt with

void handler_DE(){

   printf("Divide Error Exception\n");

    
    halt(1);
}
/* handler_DB
 * 
 * Deal with exception: Debug Exception
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_DB(){

    printf("Debug Exception\n");

    halt(1);
}
/* handler_NMI
 * 
 * Deal with exception: NMI Interrupt generated Exception
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_NMI(){

    printf("NMI Interrupt generated Exception\n");

    halt(1);
}
/* handler_BP
 * 
 * Deal with exception: Breakpoint Exception
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_BP(){

    printf("Breakpoint Exception\n");

    halt(1);
}
/* handler_OF
 * 
 * Deal with exception:Overflow Exception
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_OF(){

    printf("Overflow Exception\n");

    halt(1);
}
/* handler_BR
 * 
 * Deal with exception: BOUND Range Exceeded Exception
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_BR(){

    printf("BOUND Range Exceeded Exception\n");

    halt(1);
}
/* handler_UD
 * 
 * Deal with exception: Invalid Opcode Exception
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_UD(){

    printf("Invalid Opcode Exception\n");

    halt(1);
}
/* handler_NM
 * 
 * Deal with exception: Device Not Available Exception
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_NM(){

    printf("Device Not Available Exception\n");

    halt(1);
}
/* handler_DF
 * 
 * Deal with exception: Double Fault Exception
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_DF(){

    printf("Double Fault Exception\n");

    halt(1);
}
/* handler_CSO
 * 
 * Deal with exception: Coprocessor Segment Overrun Exception
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_CSO(){

    printf("Coprocessor Segment Overrun Exception\n");

   halt(1);
}
/* handler_TS
 * 
 * Deal with exception: Invalid TSS Exception
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_TS(){

    printf("Invalid TSS Exception\n");

    halt(1);
}
/* handler_NP
 * 
 * Deal with exception: Segment Not Present Exception
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_NP(){

    printf("Segment Not Present Exception \n");

    halt(1);
}
/* handler_SS
 * 
 * Deal with exception: Stack Fault Exception
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_SS(){

    printf("Stack Fault Exception\n");

    halt(1);
}
/* handler_GP
 * 
 * Deal with exception: General Protection Exception
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_GP(){

    printf("General Protection Exception\n");
 
    halt(1);
}
/* handler_PF
 * 
 * Deal with exception: Page-Fault Exception
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_PF(){

    printf("Page-Fault Exception\n");
     
    halt(1);
}
/* handler_MF
 * 
 * Deal with exception: x87 FPU Floating-Point Error Exception
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_MF(){

    printf("x87 FPU Floating-Point Error Exception\n");

    halt(1);
}
/* handler_AC
 * 
 * Deal with exception: Alignment Check Exception
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_AC(){

    printf("Alignment Check Exception \n");

    halt(1);
}
/* handler_MC
 * 
 * Deal with exception: Machine-Check Exception
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_MC(){

    printf("Machine-Check Exception\n");

    halt(1);
}
/* handler_XF 
 * 
 * Deal with exception: SIMD Floating-Point
 * Inputs: None
 * Outputs: None
 * Side Effects: stop processing; blue screen; print exception
 */
void handler_XF(){

    printf("SIMD Floating-Point Exception \n");

    halt(1);
}


/* IDT_init
 * 
 * initialize the IDT
 * Inputs: None
 * Outputs: None
 * Side Effects: changing the entires in IDT
 */
void IDT_init(){
    SET_IDT_ENTRY(idt[0x00], link_handler_DE, 0);               // 0 means kernel level(KERNEL DPL)
    SET_IDT_ENTRY(idt[0x01], link_handler_DB, 0);               // if we do not set reserved3=0, then it is a trap gate
    SET_IDT_ENTRY(idt[0x02], link_handler_NMI, 0);
    SET_IDT_ENTRY(idt[0x03], link_handler_BP, 0);
    SET_IDT_ENTRY(idt[0x04], link_handler_OF, 0);
    SET_IDT_ENTRY(idt[0x05], link_handler_BR, 0);
    SET_IDT_ENTRY(idt[0x06], link_handler_UD, 0);
    SET_IDT_ENTRY(idt[0x07], link_handler_NM, 0);
    SET_IDT_ENTRY(idt[0x08], link_handler_DF, 0);
    SET_IDT_ENTRY(idt[0x09], link_handler_CSO, 0);
    SET_IDT_ENTRY(idt[0x0A], link_handler_TS, 0);
    SET_IDT_ENTRY(idt[0x0B], link_handler_NP, 0);
    SET_IDT_ENTRY(idt[0x0C], link_handler_SS, 0);
    SET_IDT_ENTRY(idt[0x0D], link_handler_GP, 0);
    SET_IDT_ENTRY(idt[0x0E], link_handler_PF, 0);
    SET_IDT_ENTRY(idt[0x10], link_handler_MF, 0);
    SET_IDT_ENTRY(idt[0x11], link_handler_AC, 0);
    SET_IDT_ENTRY(idt[0x12], link_handler_MC, 0);
    SET_IDT_ENTRY(idt[0x13], link_handler_XF, 0);
    SET_IDT_ENTRY(idt[0x20], link_handler_pit, 0);
    idt[0x20].reserved3=0;                                  // 0 means interrupt gate
    SET_IDT_ENTRY(idt[0x21], link_handler_keyboard,0);
    idt[0x21].reserved3=0;                                  
    SET_IDT_ENTRY(idt[0x28], link_handler_rtc,0);
    idt[0x28].reserved3=0;      // 0 means interrupt gate
    SET_IDT_ENTRY(idt[0x80], link_handler_sysc,3);          //system calls in 3(user level)
    lidt(idt_desc_ptr);                                     //load into IDTR
}
