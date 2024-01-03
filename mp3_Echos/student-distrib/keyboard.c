#include "keyboard.h"
#include "i8259.h"
#include "lib.h"
#include "types.h"
#include "terminal.h"

char scan_table_1[59] = {             //table with 59 entries to record ascii 
	'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	'\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
	'[', ']', '\n', NULL,
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l' ,
	';', '\'', '`', NULL, '\\',
	'z', 'x', 'c', 'v', 'b', 'n', 'm',
	',', '.', '/', NULL, '\0', NULL, ' ', NULL,
};							// lowercase & character below

char scan_table_2[59] = {
	'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	'\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
	'{', '}', '\n', NULL,
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L' ,
	':', '\"', '~', NULL, '|',
	'Z', 'X', 'C', 'V', 'B', 'N', 'M',
	'<', '>', '?', NULL, '\0', NULL, ' ', NULL,
};							// uppercase & character above

uint8_t caps_lock_sign = 0;
uint8_t shift_sign = 0;
uint8_t ctrl_sign = 0;
uint8_t alt_sign = 0;

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
	uint8_t scan_ascii;
	uint8_t att = 0;
	int counter = 0;
    int status_key = 0;
	int del_tab = 0;
	//cli();
    scan = inb(keyboard_port_DATA);
	status_key = set_keyboard_status(scan);
    if(status_key == 1){
		send_eoi(KEYBOARD_IRQ_NUM);
		sti(); 
		return; //if special key, return
	}
	// if the code is f1-f3 when alt is on, change the terminal
	if((scan == f1 || scan == f2 || scan == f3) && (alt_sign == 1)){
		if(scan == f1){
			change_term(0);
		}
		else if(scan == f2){
			change_term(1);
		}
		else if(scan == f3){
			change_term(2);
		}
		send_eoi(KEYBOARD_IRQ_NUM);
		sti();
		return;
	}

	if(scan == up_arrow){
		// get previous command in terminal.c
		get_prev_cmd();
		send_eoi(KEYBOARD_IRQ_NUM);
		sti();
		return;
	}

	if(scan == down_arrow){
		get_next_cmd();
		send_eoi(KEYBOARD_IRQ_NUM);
		sti();
		return;
	}


	if(scan>=59) { // if invalid scan code
        send_eoi(KEYBOARD_IRQ_NUM); //59 is the size of the list
        sti(); 
        return;
    }      //if invalid send eoi
    if(scan_table_1[scan]==NULL){ // if not a valid key
        send_eoi(KEYBOARD_IRQ_NUM);
        sti(); 
        return;
    }      //leave out some special ticks for ck1


	// if the scan code is a letter, check the shift and caps lock status
	// 15-26 is q-p, 29-39 is a-l, 43-51 is z-m
	if(((scan < 26)&&(scan > 15))||((scan < 39)&&(scan > 29))||((scan < 51)&&(scan > 43))){  
		if(shift_sign ^ caps_lock_sign){ // if shift and caps lock are different
			scan_ascii = scan_table_2[scan];
		}
		else{
			scan_ascii = scan_table_1[scan];
		}
	} // if the scan code is a number or symbol
	else{
		if(shift_sign){			// if shift is pressed
			scan_ascii = scan_table_2[scan];
		}
		else{
			scan_ascii = scan_table_1[scan];
		}
	}

	// if enter key is pressed
	if(scan_ascii == '\n'){
		// cmd_history[visible_term][terminal[visible_term].cmd_index].cmd[terminal[visible_term].buffer_index] = '\0';
		// terminal[visible_term].cmd_index++;
		// terminal[visible_term].buffer_index = 0;
		if(add_to_buffer('\n') == -1){
				send_eoi(KEYBOARD_IRQ_NUM);
				sti();
				return;
		}
		putc_by_keyboard('\n');
		set_key_read();
		//clear_buffer(); // clear the buffer maybe need to delete
		send_eoi(KEYBOARD_IRQ_NUM);
		sti();
		return;
	}
	// if backspace is pressed
	if(scan_ascii == '\b'){
		del_tab = remove_from_buffer();
		if(del_tab == -1){
			send_eoi(KEYBOARD_IRQ_NUM);
			sti();
			return;
		}
		if(del_tab == 1){
			putc_by_keyboard('\b');
			putc_by_keyboard('\b');
			putc_by_keyboard('\b'); // delete 4 space for delete tab
			putc_by_keyboard('\b');
			send_eoi(KEYBOARD_IRQ_NUM);
			sti();
			return;
		}
		putc_by_keyboard('\b');
		send_eoi(KEYBOARD_IRQ_NUM);
		sti();
		return;
	}
	// if tab is pressed, if the cursor is at the end of the line, move to the next line
	if(scan_ascii == '\t'){
		if((get_buf_first_char(0)=='c')||(get_buf_first_char(0)=='f')||(get_buf_first_char(0)=='g')||(get_buf_first_char(0)=='h')||(get_buf_first_char(0)=='l')||(get_buf_first_char(0)=='p')||(get_buf_first_char(0)=='s')||(get_buf_first_char(0)=='t')){
			if(((get_buf_first_char(3)==' ')&&(get_buf_first_char(0)=='c')&&(get_buf_first_char(4)!='\0'))||((get_buf_first_char(0)=='g')&&(get_buf_first_char(4)==' ')&&(get_buf_first_char(5)!='\0'))){
				simple_y_c_m(get_buf_first_char(0), 1);
				send_eoi(KEYBOARD_IRQ_NUM);
				sti();
				return;
			}
			simple_y_c_m(get_buf_first_char(0), 0);
			send_eoi(KEYBOARD_IRQ_NUM);
			sti();
			return;
		}
		counter = 4;
		if(add_to_buffer('\t') == -1){
				send_eoi(KEYBOARD_IRQ_NUM);
				sti();
				return;
		}
		while(counter > 0){
			putc_by_keyboard(' ');
			counter--;
		}
		send_eoi(KEYBOARD_IRQ_NUM);
		sti();
		return;
	}
	// if ctrl + l is pressed, clear the screen
	if(scan_ascii == 'l' || scan_ascii == 'L'){
		if(ctrl_sign == 1){
			clear_term();
			//clear_buffer(); MAYBE NEED TO DELETE
			send_eoi(KEYBOARD_IRQ_NUM);
			sti();
			return;
		}
	}
	// obtain red and yellow screen
	if(scan_ascii == 'r' || scan_ascii == 'R'){
		if(ctrl_sign == 1){
			att = get_ATTRIB(visible_term);
			if(att == 0x02) clear_term();
			change_ATTRIB(visible_term, 0x4E); // 0x4E is red and yellow
			send_eoi(KEYBOARD_IRQ_NUM);
			sti();
			return;
		}
	}
	// obtain green and yellow screen
	if(scan_ascii == 'g' || scan_ascii == 'G'){
		if(ctrl_sign == 1){
			att = get_ATTRIB(visible_term);
			if(att == 0x02) clear_term();
			change_ATTRIB(visible_term, 0x2E); // 0x2E is green and yellow
			send_eoi(KEYBOARD_IRQ_NUM);
			sti();
			return;
		}
	}
	// obtain blue and yellow screen
	if(scan_ascii == 'b' || scan_ascii == 'B'){
		if(ctrl_sign == 1){
			att = get_ATTRIB(visible_term);
			if(att == 0x02) clear_term();
			change_ATTRIB(visible_term, 0x1E); //	0x1E is blue and yellow
			send_eoi(KEYBOARD_IRQ_NUM);
			sti();
			return;
		}
	}

	//obtain white and black screen
	if(scan_ascii == 'w' || scan_ascii == 'W'){
		if(ctrl_sign == 1){
			att = get_ATTRIB(visible_term);
			if(att == 0x02) clear_term();
			change_ATTRIB(visible_term, 0xF1); // 0x0F is white and black
			send_eoi(KEYBOARD_IRQ_NUM);
			sti();
			return;
		}
	}

	// obtain vscode dark and white screen
	if(scan_ascii == 'd' || scan_ascii == 'D'){
		if(ctrl_sign == 1){
			att = get_ATTRIB(visible_term);
			if(att == 0x02) clear_term();
			change_ATTRIB(visible_term, 0x0F); // 0x1F is vscode dark and blue
			send_eoi(KEYBOARD_IRQ_NUM);
			sti();
			return;
		}
	}

	// obtain classic black and green screen (hacker theme screen favoured by qzx)
	if(scan_ascii == 'q' || scan_ascii == 'Q'){
		if(ctrl_sign == 1){
			change_ATTRIB(visible_term, 0x02); // 0x02 is classic black and green
			clear_term();
			anEgg();
			send_eoi(KEYBOARD_IRQ_NUM);
			sti();
			return;
		}
	}

	// // interrupt for ctrl + c, not complete
	// if(scan_ascii == 'c' || scan_ascii == 'C'){
	// 	if(ctrl_sign == 1){
	// 		//terminal->buffer[terminal->buffer_index] = '\0';
	// 		//terminal->key_read = 1;
	// 	}
	// }
	if(scan_ascii != '\0'){
		if(add_to_buffer(scan_ascii) == 0){
			putc_by_keyboard(scan_ascii);
		}
	}
    send_eoi(KEYBOARD_IRQ_NUM);
    sti();
	return;
}

// set the ctrl, alt, shift, caps lock status
// call when a key is pressed
// input: scan code
// output: 1 if the scan code is a special key, 0 for skip, -1 if error
// side effect: set the status
int set_keyboard_status(uint8_t scan){
	if(scan == left_shift || scan == right_shift){
		shift_sign = 1;
		return 1;
	}
	else if(scan == left_shift_release || scan == right_shift_release){
		shift_sign = 0;
		return 1;
	}
	else if(scan == caps_lock){
		if(caps_lock_sign == 0){
			caps_lock_sign = 1;
		}
		else{
			caps_lock_sign = 0;
		}
		return 1;
	}
	else if(scan == left_ctrl){
		ctrl_sign = 1;
		return 1;
	}
	else if(scan == left_ctrl_release){
		ctrl_sign = 0;
		return 1;
	}
	else if(scan == left_alt){
		alt_sign = 1;
		return 1;
	}
	else if(scan == left_alt_release){
		alt_sign = 0;
		return 1;
	}
	else if(scan == caps_lock_release){
		return 1;
	}
	return 0;
	// should never execute next line
	// return -1;
}

