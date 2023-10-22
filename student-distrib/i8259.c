/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */
/*
*   disable_irq
*   description: Initialize the 8259 PIC
*   input: none
*   output: None
*   side effect: intial the pic
*/
void i8259_init(void) {
    master_mask=0xFF;           //update the mask number(1 in each bit means a interrupt masked)
    slave_mask=0xFF;
    outb(master_mask,MASTER_8259_PORT+1);  //write 0xFF(all interrupts bit as 1) to second port to mask all interrupts,8259A-1
    outb(slave_mask,SLAVE_8259_PORT+1);   //8259A-2
    //init master
    outb(ICW1,MASTER_8259_PORT);   
    outb(ICW2_MASTER,MASTER_8259_PORT+1);   //mapped from 0x20 to 0x27
    outb(ICW3_MASTER ,MASTER_8259_PORT+1);  //master has a secondary in IR2
    outb(ICW4,MASTER_8259_PORT+1);          //normal EOI signaling
    //init slave
    outb(ICW1,SLAVE_8259_PORT);
    outb(ICW2_SLAVE,SLAVE_8259_PORT+1);
    outb(ICW3_SLAVE,SLAVE_8259_PORT+1);     //mapped from 0x28 to 0x2F
    outb(ICW4,SLAVE_8259_PORT+1);

    master_mask=0xFB;
    outb(master_mask,MASTER_8259_PORT+1);  //enable the IR2, connect the secondary in IR2
}
/*
*   disable_irq
*   description: Enable (unmask) the specified IRQ
*   input: int irq_num -- which interrupt finished
*   output: None
*   side effect: enable a interrupt and change the mask number
*/
void enable_irq(uint32_t irq_num) {
    uint8_t temp=0xFF;
    if(irq_num>15)return;
    if (irq_num<=7)
    {   temp = temp ^ (1<<irq_num); // let the irq bit as 0
        master_mask = master_mask & temp;
        outb(master_mask,MASTER_8259_PORT+1);
        }
    else{
        temp = temp ^ (1<<(irq_num-8)); // let the irq bit as 0
        slave_mask = slave_mask & temp;
        outb(slave_mask,SLAVE_8259_PORT+1);    //write to the PIC
    }
}

/*
*   disable_irq
*   description: Disable (mask) the specified IRQ
*   input: int irq_num -- which interrupt finished
*   output: None
*   side effect: disable a interrupt and change the mask number
*/
void disable_irq(uint32_t irq_num) {
    uint8_t temp=0x00;
    if(irq_num>15)return;
    if(irq_num<=7){
        temp = temp ^ (1<<irq_num); // let the irq bit as 1 to enable
        master_mask = master_mask | temp;
        outb(master_mask,MASTER_8259_PORT+1);
    }
    else{
        temp = temp ^ (1<<(irq_num-8)); // let the irq bit as 1 to enable
        slave_mask = slave_mask | temp;
        outb(slave_mask,SLAVE_8259_PORT+1);    //write to the PIC
    }
}

/*
*   send_eoi
*   description:Send end-of-interrupt signal for the specified IRQ 
*   input: int irq_num -- which interrupt finished
*   output: None
*   side effect: EOI sent to PIC
*/
void send_eoi(uint32_t irq_num) {
    if(irq_num>15)return;
    if(irq_num<=7){
        outb(EOI+irq_num,MASTER_8259_PORT);         
       }
    else{
        outb(EOI+(irq_num-8),SLAVE_8259_PORT);          
        outb(EOI+2,MASTER_8259_PORT);           //We also need to tell the master that IR2 finished
       
    }
}
