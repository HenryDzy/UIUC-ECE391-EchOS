#include "mouse.h"
#include "types.h"
#include "i8259.h"
#include "signal.h"
#include "terminal.h"
//     
//the code is referenced from https://wiki.osdev.org/Mouse, and https://forum.osdev.org/viewtopic.php?t=10247
//
uint8_t packets[3];
int16_t cursor_x;
int16_t cursor_y;
int16_t last_x;
int16_t last_y;
/* 
 * mouse_wait
 * DESCRIPTION: read data from mouse
 * INPUTS: uint8_t type
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
void mouse_wait(uint8_t type){
  uint32_t time=100000; 
  if(type==0){        // 0 for input
    while(time--) 
    {
      if((inb(0x64) & 1)==1)  //wait input
      {
        return;
      }
    }
    return;
  }
  if(type==1){        // 1 for output
    while(time--) 
    {
      if((inb(0x64) & 2)==1)  //wait output
      {
        return;
      }
    }
    return;
  }

}

/* 
 * read_from_mouse
 * DESCRIPTION: read data from mouse
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: a byte from the mouse port
 * SIDE EFFECTS: none
 */
uint8_t read_mouse(void)
{
    mouse_wait(0);
    while((inb(0x64) & 0x01) != 1); //wait until we get input
    return inb(0x60);
}


/* 
 * mouse_install
 * DESCRIPTION: enable mouse
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
void mouse_install(void)
{

    uint8_t _status;

    mouse_wait(1);      // wait for input 
    while((inb(0x64) & 0x02) != 0);   // wait for output 
    outb(0xD4,0x64);


    mouse_wait(1);    // wait for input  
    while((inb(0x64) & 0x02) != 0);  // wait for output 
    outb(0xFF,0x60);    

    while (read_mouse() != 0xFA);

    mouse_wait(1);
    while((inb(0x64) & 0x02) != 0);
    outb(0x20,0x64);

    _status = (read_mouse() |0x02)& (~0x20);

    mouse_wait(1);
    while((inb(0x64) & 0x02) != 0);
    outb(0x60,0x64);

    mouse_wait(1);
    while((inb(0x64) & 0x02) != 0);
    outb(_status,0x60);

    mouse_wait(1);
    while((inb(0x64) & 0x02) != 0);
    outb(0xD4,0x64);

    mouse_wait(1);
    while((inb(0x64) & 0x02) != 0);
    outb(0xF4,0x60);

    while (read_mouse() != 0xFA);
    enable_irq(12);
    return;
}

/* 
 * mouse_handler
 * DESCRIPTION: this is the handler of mouse interrupt
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
void mouse_handler(void)
{   int32_t mouse_x;
    int32_t mouse_y;
    last_x = cursor_x;
    last_y = cursor_y;
    if (((inb(0x64)&0x01)==0) || ((inb(0x64)&0x20)==0)){
        send_eoi(12);
        return;
    }
    packets[0] = inb(0x60);
    if (packets[0] == 0xFA) {
        send_eoi(12);
        return;
    }
    if((packets[0] & 0x40) || (packets[0] & 0x80)){
        send_eoi(12);
        return;
    }
    packets[1]=read_mouse();
    packets[2]=read_mouse(); 
    mouse_x =(int32_t) packets[1];
    mouse_y = (int32_t)packets[2];
    if (packets[0] & 0x10){
        mouse_x=mouse_x|0xFFFFFF00;     // change it to negative 
    }
    if (packets[0] & 0x20){
        mouse_y=mouse_y|0xFFFFFF00;
    }
    if(packets[0]&0x01){
      make_signal(USER1);
    }else
    {
      if(packets[0]&0x04){
        change_term((visible_term+1)%3);
      }else{
        if(packets[0]&0x02){
        make_signal(USER2);
      }
      }
    }
    mouse_x /= 40;          // the movement is too big for our kernel
    mouse_y /= 40;          
    mouse_y = -mouse_y;
    cursor_x = cursor_x + mouse_x;
    cursor_y = cursor_y + mouse_y;
    if (cursor_x + mouse_x < 0) {     // limit it in the range
            cursor_x  = 0;} 
    if (cursor_x + mouse_x > 79) {
            cursor_x = 79;} 
    if (cursor_y + mouse_y < 0) {
        cursor_y  = 0; } 
    if (cursor_y + mouse_y > 24) {
            cursor_y = 24;} 
    if( (cursor_x == last_x) && (cursor_y == last_y) ){
      send_eoi(12);
      return;
    }
    cursor_moving(cursor_x,cursor_y);
    send_eoi(12);   
}






