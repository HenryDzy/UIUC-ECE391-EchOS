/* terminal.h     - Defines functions and structures for terminal
 * Created on 10/19/2023
 */
#ifndef TERMINAL_H
#define TERMINAL_H

#include "lib.h"
#include "keyboard.h"
#include "types.h"

#define BUF_SIZE 128

typedef struct cmd_history_t {
    uint8_t cmd[BUF_SIZE];
} cmd_history_t;

typedef struct terminal_t {
    uint8_t buffer[BUF_SIZE];    // buffer for the terminal, 128 chars of max size
    uint8_t buffer_count;   // current index of the buffer
    volatile uint8_t key_read;      // flag for reading from the keyboard buffer 
    uint16_t cursor_offset;      // current cursor position
    uint8_t active;         // 0 if not active, 1 if active
    uint32_t freq;          // frequency of the terminal
    uint8_t interrupt;          // interrupt flag
    cmd_history_t cmd_history[100];         // command history, 100 commands of max size
    int cmd_history_idx;                    // current index of the command history
    uint8_t curprogram[100];                // current program, 100 chars of max size
    int find_his_counter;                   // current index of the command history
    int buffer_cursor;
//  uint8_t pid;            // pid of the process that owns this terminal
} terminal_t;

extern void change_term(int32_t term_num);              // change terminal
extern terminal_t* get_term(int32_t term_num);             // get current terminal
extern int32_t open_term(const uint8_t *filename);      // open terminal
extern int32_t close_term(int32_t fd);                   // close terminal
extern int32_t read_term(int32_t fd, void* buf, int32_t nbytes);        // read terminal
extern int32_t write_term(int32_t fd, const void* buf, int32_t nbytes); // write terminal
extern int32_t init_term();                         // initialize terminal
//extern void scroll_term();                          // scroll terminal
extern void clear_term();                           // clear terminal
extern int32_t add_to_buffer(uint8_t c);            // add a char to the buffer
extern int32_t remove_from_buffer();                // remove a char from the buffer
extern void clear_buffer(void);                     // clear the buffer
extern uint8_t get_buf_count();                     // get the current buffer count
extern void set_key_read();                         // set the key read flag
extern void get_prev_cmd(void);                            // get the previous command
extern void get_next_cmd(void);                            // get the previous command
extern void simple_y_c_m(uint8_t c, uint8_t arg);             // auto cmlt
extern uint8_t get_buf_first_char(int idx);         // get the first char of the buffer at idx

extern terminal_t *terminal;                        // current terminal
extern volatile uint8_t visible_term;               // visible terminal

#endif


// #endif /* TERMINAL_H */

