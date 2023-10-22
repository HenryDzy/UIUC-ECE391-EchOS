#include "keyboard.h"
#include "i8259.h"
#include "lib.h"
#include "types.h"

char scan_table[59] = {             //table with 59 entries to record ascii 
	'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	NULL, NULL, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
	'[', ']', NULL, NULL,
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l' ,
	';', '\'', '`', NULL, '\\',
	'z', 'x', 'c', 'v', 'b', 'n', 'm',
	',', '.', '/', NULL, '\0',NULL, ' ', NULL,
};


/*
*   keyboard_init
*   initialize the keyboard device
*   input: none
*   output: none
*   side effect: enable IR1 in PIC
*/
void keyboard_init(){
    enable_irq(KEYBOARD_IRQ_NUM);
}
/*
*   handler_keyboard
*   a handler for keyboard interrupt
*   input: none
*   output: none
*   side effect: send EOI signal
*/
void handler_keyboard(){
    uint8_t scan;
    cli();
    scan = inb(keyboard_port_DATA);
    if(scan>=59) {
    send_eoi(KEYBOARD_IRQ_NUM); //59 is the size of the list
    sti(); 
    return;}      //if invalid send eoi
    if(scan_table[scan]==NULL){
    send_eoi(KEYBOARD_IRQ_NUM);
    sti(); 
    return;}      //leave out some special ticks for ck1
    putc(scan_table[scan]);
    send_eoi(KEYBOARD_IRQ_NUM);
    sti();
}
