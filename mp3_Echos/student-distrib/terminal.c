
#include "lib.h"
#include "keyboard.h"
#include "types.h"
#include "terminal.h"
#include "system_calls.h"
#include "scheduling.h"
static terminal_t terminals[3];         // three terminals
volatile uint8_t visible_term;  // the terminal that is currently displayed
terminal_t* terminal = &(terminals[0]);         // the terminal that is currently running

uint8_t frame_sign = 0;

uint8_t _ca[BUF_SIZE] = "cat";
uint8_t _co[BUF_SIZE] = "counter";
uint8_t _fi[BUF_SIZE] = "fish";
uint8_t _gr[BUF_SIZE] = "grep";
uint8_t _he[BUF_SIZE] = "hello";
uint8_t _ls[BUF_SIZE] = "ls";
uint8_t _pi[BUF_SIZE] = "pingpong";
uint8_t _sh[BUF_SIZE] = "shell";
uint8_t _si[BUF_SIZE] = "sigtest";
uint8_t _st[BUF_SIZE] = "status";
uint8_t _sy[BUF_SIZE] = "syserr";
uint8_t _te[BUF_SIZE] = "testprint";
uint8_t _ve[BUF_SIZE] = "verylargetextwithverylongname.tx";
uint8_t _fr0[BUF_SIZE] = "frame0.txt";
uint8_t _fr1[BUF_SIZE] = "frame1.txt";

#define NUM_COL 80

/* change_term
    *  Change the terminal
    *  INPUT: term_num -- terminal number to change to
    * RETURN VALUE: None
    * SIDE EFFECT: change the terminal
*/
void change_term(int32_t term_num){
    if(term_num<0 || term_num>2) return;            // invalid terminal number
    if(term_num == visible_term) return;            // already in the terminal
    terminal = &(terminals[term_num]);
    change_term_vidmap_same();                      // change the vidmap to the new terminal
    memcpy((void*)(VIDEO_PAGE_BEGIN + 2 * PAGE_SIZE * (visible_term + 1)), (void*)VIDEO_PAGE_BEGIN, PAGE_SIZE);     // save the current terminal
    visible_term = term_num;
    memcpy((void*)VIDEO_PAGE_BEGIN, (void*)(VIDEO_PAGE_BEGIN + 2*(term_num+1)*PAGE_SIZE), PAGE_SIZE);               // change to the new terminal
    change_term_vidmap(cur_term);                   // change the vidmap back to the current terminal
    cursor_moving(terminal->cursor_offset % NUM_COL, terminal->cursor_offset / NUM_COL);        // set the cursor position
}
/* get_term
    *  Get the terminal
    *  INPUT: term_num -- terminal number to get
    * RETURN VALUE: pointer to the terminal
    * SIDE EFFECT: None
*/
terminal_t* get_term(int32_t term_num){
    if(term_num<0 || term_num>2) return NULL;
    return (terminal_t*)&(terminals[term_num]);
}

/* open_term
    *  Open the terminal
    *  INPUT: None
    * RETURN VALUE: 0 if success
    * SIDE EFFECT: open the terminal
 */
int32_t open_term(const uint8_t *filename) {
    terminal->active = 1;
    return 0;
}

/* close_term
    *  Close the terminal
    *  INPUT: None
    * RETURN VALUE: 0 if success
    * SIDE EFFECT: close the terminal
*/
int32_t close_term(int32_t fd) {
    // if(fd==1 || fd==0){
    //     printf("Error: Stdin or Stdout cannot close terminal!");
    //     return -1;
    // }
    terminal->active = 0;
    return 0;
}

/* init_term
    *  Initialize the terminal
    *  INPUT: None
    * RETURN VALUE: 0 if success
    * SIDE EFFECT: initialize the terminal
*/
int32_t init_term() {
    int i;
    int term_counter;
    int x, y;
    for(term_counter = 0; term_counter<3; term_counter++){
        terminal = &(terminals[term_counter]);
        terminal->buffer_count = 0;         /* Reset the buffer count */
        terminal->key_read = 0;             /* Reset the "endline" flag */
        terminal->cursor_offset = cursor_get_pos(); /* Get the cursor position */
        //    terminal->cursor_offset = 0;        /* Reset the cursor position */
        for(i = 0; i < BUF_SIZE; i++) {     /* Clear the buffer */
            terminal->buffer[i] = 0x0;      /* Fill the buffer with 0 */
        }
        for(i = 0; i<100; i++){
            terminal->cmd_history[i].cmd[0] = '\0';
            terminal->curprogram[i]='\0';
        }
        terminal->cmd_history_idx = 0;
        terminal->find_his_counter = terminal->cmd_history_idx;
        terminal->buffer_cursor = 0;
    }
    terminal = &(terminals[0]); // reset current terminal to 0
    cursor_enable(0, 14);            // enable cursor, 0 is the start position, 14 is the end position
    x = terminal->cursor_offset % NUM_COL;
    y = terminal->cursor_offset / NUM_COL;
    cursor_moving(x, y);    /* Set cursor position */
    visible_term = 0;       // the terminal that is currently displayed
    return 0;
}
/* read_term
    *  Read from terminal
    *  INPUT: buf    -- buffer to read into
    *         nbytes -- number of bytes to read
    * RETURN VALUE: number of bytes read, -1 if fail
    * OUTPUT: change the contents of given buffer
    * SIDE EFFECT: change the contents of given buffer
*/
int32_t read_term(int32_t fd, void* buf, int32_t nbytes) {
    int i, endline;
    terminal_t* new_terminal;
    int term_id=0;
    new_terminal = &(terminals[cur_term]);
    // C = cur_term;
    cli();
    endline = 0;
    if(fd == 1) {
        printf("Stdout cannot read!\n");
        sti();
        return -1;}
    if(buf == NULL) return -1;

    for(i = 0; i < 3; i++){                 // find the terminal, 3 terminals
        if(new_terminal==(&terminals[i])){
            term_id = i;
            break;
        }
    }

    if(new_terminal->buffer_count!=0){
        for(i=0; i<new_terminal->buffer_count; i++){
            if(new_terminal->buffer[i]=='\n'){
                new_terminal->buffer[i] = '\0';
            }
        }
        if(new_terminal->buffer[0]!='\0'){
            memcpy(new_terminal->cmd_history[new_terminal->cmd_history_idx].cmd, new_terminal->buffer, BUF_SIZE);
            new_terminal->cmd_history_idx = (new_terminal->cmd_history_idx + 1) % 100;              // update the command history, 100 commands of max size
            new_terminal->find_his_counter = new_terminal->cmd_history_idx;
        }
    }

    if((new_terminal->curprogram[0]=='s')&&(new_terminal->curprogram[1]=='h')&&(new_terminal->curprogram[2]=='e')&&(new_terminal->curprogram[3]=='l')&&(new_terminal->curprogram[4]=='l')){
        update_current_global_time();
        printf("Time: %u-%u-%u %u:%u \n", year,month,day,hour,minute);
        printf("Term%d @ 391OS> ", term_id);
    }

    new_terminal->buffer_count = 0;
    //terminals[cur_term].buffer_count = 0;
    memset(new_terminal->buffer, 0, BUF_SIZE);    
    //memset(terminals[cur_term].buffer, 0, BUF_SIZE); 
    new_terminal->key_read = 0;
    //terminals[cur_term].key_read = 0;
    sti();
    while (new_terminal->key_read == 0);
    //while (terminals[cur_term].key_read == 0);
    for (i = 0; i < BUF_SIZE; i++) {
        //if (new_terminal->buffer[i] == '\0') break;
        if (terminals[cur_term].buffer[i] == '\0') break;
        //((char*)buf)[i] = new_terminal->buffer[i];
        ((char*)buf)[i] = terminals[cur_term].buffer[i];
        endline++;
    }                                     
    memset(&(((char*)buf)[endline]), 0, BUF_SIZE - endline);
    return endline - 1; // minus one because of the enter key
}

/* write_term
    *  Write bytes in the given buffer to screen
    *  INPUT: buf    -- buffer to write from
    *         nbytes -- number of bytes to write
    * RETURN VALUE: number of bytes written, -1 if fail
    * SIDE EFFECT: write bytes in the given buffer to screen
*/
int32_t write_term(int32_t fd, const void* buf, int32_t nbytes) {
    int i, endline;
    int32_t limit = 0;
    endline = 0;
    if(fd == 0) {
        printf("Stdin cannot written!\n");
        return -1; // read only
    }
//  if(terminal->active == 0) return -1;
    if(buf == NULL) return -1;
    else limit = nbytes;
    for (i = 0; i < limit; i++) {
        //if (((char*)buf)[i] == '\0') break;
        putc_by_program(((uint8_t*)buf)[i]);
        endline++;
    }
    terminal->cursor_offset = cursor_get_pos();
//    get_cursor(terminal->cursor_offset);
    sti();          
    return endline - 1; // minus one because of the enter key
}

/* scroll_term
    *  Scroll the terminal
    *  INPUT: None
    * RETURN VALUE: None
    * SIDE EFFECT: scroll the terminal
*/
// void scroll_term() {
//     lines_moving_up();                         
//     terminal->cursor_offset = cursor_get_pos();
// //    get_cursor(terminal->cursor_offset);          
// }

/* clear_term
    *  Clear the terminal
    *  INPUT: None
    * RETURN VALUE: None
    * SIDE EFFECT: clear the terminal
*/
void clear_term() {
    clear();            
    clear_screen();                    
//    terminal->cursor_offset = 0;

    //printf("391OS> ");
    putc_by_keyboard('c');
    putc_by_keyboard('l');
    putc_by_keyboard('e');
    putc_by_keyboard('a');
    putc_by_keyboard('r');
    putc_by_keyboard('i');
    putc_by_keyboard('n');
    putc_by_keyboard('g');
    putc_by_keyboard('.');
    putc_by_keyboard('.');
    putc_by_keyboard('.');
    putc_by_keyboard('\n');

    putc_by_keyboard('3');
    putc_by_keyboard('9');
    putc_by_keyboard('1');
    putc_by_keyboard('O');
    putc_by_keyboard('S');
    putc_by_keyboard('>');
    putc_by_keyboard(' ');
//   terminal->cursor_offset = cursor_get_pos();   //要改
//    get_cursor(terminal->cursor_offset);          
}

/* add_to_buffer
    *  Add a character to the buffer
    *  INPUT: c -- character to add
    * RETURN VALUE: 0 if success, -1 if fail
    * SIDE EFFECT: add a character to the buffer
*/
int32_t add_to_buffer(uint8_t c) {
    if (terminal->buffer_count >= BUF_SIZE - 1) return -1;  // -1 is to reserve space for '\0'
    if (c != '\n' && terminal->buffer_count >= BUF_SIZE - 2) return -1;
    terminal->buffer[terminal->buffer_count] = c;
    terminal->buffer_count++;
    return 0;
}

/* set_key_read
    *  Set the "endline" flag
    *  INPUT: None
    * RETURN VALUE: None
    * SIDE EFFECT: set the "endline" flag
*/
void set_key_read() {
    terminal->key_read = 1;             // read from keyboard buffer
}

/* get_buf_count
    *  Get the buffer count
    *  INPUT: None
    * RETURN VALUE: buffer count
    * SIDE EFFECT: None
*/
uint8_t get_buf_count() {
    return terminal->buffer_count;
}

uint8_t get_buf_first_char(int idx){
    return terminal->buffer[idx];
}

/* remove_from_buffer
    *  Remove a character from the buffer
    *  INPUT: None
    * RETURN VALUE: 0 if success, -1 if fail, 1 if delete a "tab"
    * SIDE EFFECT: remove a character from the buffer
*/
int32_t remove_from_buffer(){
    int tabflag = 0;
    if (terminal->buffer_count <= 0) return -1;     // buffer is empty
    terminal->buffer_count--;
    if(terminal->buffer[terminal->buffer_count]=='\t'){
        tabflag = 1;
    }
    //terminal->buffer_count--;
    terminal->buffer[terminal->buffer_count] = '\0';
    return tabflag;
}

// clear the buffer
// INPUT: None
// RETURN VALUE: None
// SIDE EFFECT: clear the buffer
void clear_buffer(void){
    int i;
    // terminal->buffer[terminal->buffer_count] = '\0';
    // memcpy(terminal->cmd_history[terminal->cmd_history_idx].cmd, terminal->buffer, BUF_SIZE);
    // terminal->cmd_history_idx = (terminal->cmd_history_idx + 1) % 100;
    for(i = 0; i < BUF_SIZE; i++){
        terminal->buffer[i] = '\0';
    }
    terminal->buffer_count = 0;
}

// get the previous command
// INPUT: None
// RETURN VALUE: None
// SIDE EFFECT: get the previous command
void get_prev_cmd(void){
    int i;
    terminal->find_his_counter--;
    if(terminal->find_his_counter<0) terminal->find_his_counter=0;
    if(terminal->cmd_history[terminal->find_his_counter].cmd[0] == '\0'){
        terminal->find_his_counter++;
        return;
    }
    //terminal->cmd_history_idx = temp_counter;
    memcpy(terminal->buffer, terminal->cmd_history[terminal->find_his_counter].cmd, BUF_SIZE);
    for(i = 0; i < terminal->buffer_count; i++){
        putc_by_keyboard('\b');
    }
    terminal->buffer_count = strlen((int8_t*)terminal->buffer);
    for(i = 0; i < terminal->buffer_count; i++){
        putc_by_keyboard(terminal->buffer[i]);
    }
}


// get next cmd his
// INPUT: None
// RETURN VALUE: None
// SIDE EFFECT: get the next command
void get_next_cmd(void){
    int i;
    terminal->find_his_counter++;
    if(terminal->find_his_counter>100) terminal->find_his_counter=100;
    if(terminal->cmd_history[terminal->find_his_counter].cmd[0] == '\0'){
        terminal->find_his_counter--;
        // for(i = 0; i < terminal->buffer_count; i++){
        //     putc_by_keyboard('\b');
        // }
        // memset(terminal->buffer, 0, BUF_SIZE);
        return;
    }
    //terminal->cmd_history_idx = temp_counter;
    memcpy(terminal->buffer, terminal->cmd_history[terminal->find_his_counter].cmd, BUF_SIZE);
    for(i = 0; i < terminal->buffer_count; i++){
        putc_by_keyboard('\b');
    }
    terminal->buffer_count = strlen((int8_t*)terminal->buffer);
    for(i = 0; i < terminal->buffer_count; i++){
        putc_by_keyboard(terminal->buffer[i]);
    }
}

// a single version of YouCompleteMe, auto complete function by press tab
// INPUT: c -- the first char of the buffer
//        arg -- 0 if no argument, 1 if has argument
// RETURN VALUE: None
// SIDE EFFECT: auto complete function
void simple_y_c_m(uint8_t c, uint8_t arg){
    int i;
    int argnum = 0;

    if(arg==1&&c=='c'){
        argnum = 4;                 // cat, 4 chars
    }

    if(arg==1&&c=='g'){
        argnum = 5;                 // grep, 5 chars
    }

    // clear the buffer and screen  

    if(argnum!=0){
        switch(terminal->buffer[argnum])
        {
        case 'c':
            if(terminal->buffer[argnum+1]=='a'){                // cat
                memcpy(&(terminal->buffer[argnum]), _ca, BUF_SIZE-argnum);      
            }
            else{
                memcpy(&(terminal->buffer[argnum]), _co, BUF_SIZE-argnum);
            }
            break;
        case 'f':                                // fish
            if(terminal->buffer[argnum+1]=='r'){
                if(frame_sign==0){
                    frame_sign = 1;
                    memcpy(&(terminal->buffer[argnum]), _fr0, BUF_SIZE-argnum);  
                }
                else{
                    frame_sign = 0;
                    memcpy(&(terminal->buffer[argnum]), _fr1, BUF_SIZE-argnum); 
                }
                break;
            }
            memcpy(&(terminal->buffer[argnum]), _fi, BUF_SIZE-argnum);
            break;
        case 'g':                                 // grep
            memcpy(&(terminal->buffer[argnum]), _gr, BUF_SIZE-argnum);
            break;
        case 'h':                               // hello    
            memcpy(&(terminal->buffer[argnum]), _he, BUF_SIZE-argnum);
            break;
        case 'l':                                   // ls
            memcpy(&(terminal->buffer[argnum]), _ls, BUF_SIZE-argnum);
            break;
        case 'p':                               // pingpong
            memcpy(&(terminal->buffer[argnum]), _pi, BUF_SIZE-argnum);
            break;
        case 's':                               // shell
            if(terminal->buffer[argnum+1]=='y'){        // syserr
                memcpy(&(terminal->buffer[argnum]), _sy, BUF_SIZE-argnum);
            }
            else if(terminal->buffer[argnum+1]=='i'){   // sigtest
                memcpy(&(terminal->buffer[argnum]), _si, BUF_SIZE-argnum);
            }
            else{                                // status
                memcpy(&(terminal->buffer[argnum]), _sh, BUF_SIZE-argnum);
            }
            break;
        case 't':                               // testprint
            memcpy(&(terminal->buffer[argnum]), _te, BUF_SIZE-argnum);
            break;
        case 'v':
            memcpy(&(terminal->buffer[argnum]), _ve, BUF_SIZE-argnum);
            break;
        default:
            return;
        }
    }
    else{
        switch (c)
        {
        case 'c':
            if(terminal->buffer[1]=='a'){
                memcpy(&(terminal->buffer[argnum]), _ca, BUF_SIZE-argnum);
            }
            else{
                memcpy(&(terminal->buffer[argnum]), _co, BUF_SIZE-argnum);
            }
            break;
        case 'f':
            memcpy(&(terminal->buffer[argnum]), _fi, BUF_SIZE-argnum);
            break;
        case 'g':
            memcpy(&(terminal->buffer[argnum]), _gr, BUF_SIZE-argnum);
            break;
        case 'h':
            memcpy(&(terminal->buffer[argnum]), _he, BUF_SIZE-argnum);
            break;
        case 'l':
            memcpy(&(terminal->buffer[argnum]), _ls, BUF_SIZE-argnum);
            break;
        case 'p':
            memcpy(&(terminal->buffer[argnum]), _pi, BUF_SIZE-argnum);
            break;
        case 's':
            if(terminal->buffer[1]=='y'){
                memcpy(&(terminal->buffer[argnum]), _sy, BUF_SIZE-argnum);
            }
            else if(terminal->buffer[1]=='i'){
                memcpy(&(terminal->buffer[argnum]), _si, BUF_SIZE-argnum);
            }
            else if(terminal->buffer[1]=='t'){
                memcpy(&(terminal->buffer[argnum]), _st, BUF_SIZE-argnum);
            }
            else{
                memcpy(&(terminal->buffer[argnum]), _sh, BUF_SIZE-argnum);
            }
            break;
        case 't':
            memcpy(&(terminal->buffer[argnum]), _te, BUF_SIZE-argnum);
            break;
        default:
            return;
        }
    }

    if(arg==0){
        for(i = 0; i < terminal->buffer_count; i++){
            putc_by_keyboard('\b');
        }
    }
    else{
        i = terminal->buffer_count;
        while(terminal->buffer[i]!=' '){
            i--;
            putc_by_keyboard('\b');
        }
        putc_by_keyboard(' ');
    }

    terminal->buffer_count = strlen((int8_t*)terminal->buffer);
    for(i = argnum; i < terminal->buffer_count; i++){
        putc_by_keyboard(terminal->buffer[i]);
    }
}
