/* lib.c - Some basic library functions (printf, strlen, etc.)
 * vim:ts=4 noexpandtab */

#include "lib.h"
#include "rtc.h"
#include "scheduling.h"
#include "terminal.h"
#include "page.h"

#define VIDEO       0xB8000
#define NUM_COLS    80
#define NUM_ROWS    25

//static ATTRIB = 0x1E;

static uint8_t term_color[3] = {0x1E, 0x1E, 0x1E};      // default color, 3 terminals

static char* video_mem = (char *)VIDEO;                 // video memory


// void change_ATTRIB(uint8_t attrib);
// input: uint8_t attrib
// output: none
// side effect: change the attrib
void change_ATTRIB(uint8_t term_num, uint8_t attrib){
    int32_t i;
    term_color[visible_term] = attrib; // change the attrib
    change_term_vidmap_same();
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(video_mem + (i << 1) + 1) = term_color[visible_term];
    }
    change_term_vidmap(cur_term);
}

// uint8_t get_ATTRIB(uint8_t term_num);
// input: uint8_t term_num
// output: uint8_t attrib
// side effect: none
uint8_t get_ATTRIB(uint8_t term_num){
    return term_color[term_num];
}

/* void clear(void);
 * Inputs: void
 * Return Value: none
 * Function: Clears video memory */
void clear(void) {
    int32_t i;
    change_term_vidmap_same();
    (get_term(visible_term))->cursor_offset = 0;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(video_mem + (i << 1)) = ' ';
        *(uint8_t *)(video_mem + (i << 1) + 1) = term_color[visible_term];
    }
    change_term_vidmap(cur_term);
}

/* void clear_vidmap(int32_t term_num);
 * Inputs: int32_t term_num
 * Return Value: none
 * Function: Clears video memory */
void clear_vidmap(int32_t term_num) {
    int32_t i;
    change_term_vidmap(term_num);
    (get_term(term_num))->cursor_offset = 0;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(video_mem + 2*(term_num+1)*PAGE_SIZE + (i << 1)) = ' ';        // clear the video memory, 2*(term_num+1)*PAGE_SIZE is the offset
        *(uint8_t *)(video_mem + 2*(term_num+1)*PAGE_SIZE + (i << 1) + 1) = term_color[term_num];     // clear the video memory, 2*(term_num+1)*PAGE_SIZE is the offset
    }
    change_term_vidmap(cur_term);
}

/* Standard printf().
 * Only supports the following format strings:
 * %%  - print a literal '%' character
 * %x  - print a number in hexadecimal
 * %u  - print a number as an unsigned integer
 * %d  - print a number as a signed integer
 * %c  - print a character
 * %s  - print a string
 * %#x - print a number in 32-bit aligned hexadecimal, i.e.
 *       print 8 hexadecimal digits, zero-padded on the left.
 *       For example, the hex number "E" would be printed as
 *       "0000000E".
 *       Note: This is slightly different than the libc specification
 *       for the "#" modifier (this implementation doesn't add a "0x" at
 *       the beginning), but I think it's more flexible this way.
 *       Also note: %x is the only conversion specifier that can use
 *       the "#" modifier to alter output. */
int32_t printf(int8_t *format, ...) {

    /* Pointer to the format string */
    int8_t* buf = format;

    /* Stack pointer for the other parameters */
    int32_t* esp = (void *)&format;
    esp++;

    while (*buf != '\0') {
        switch (*buf) {
            case '%':
                {
                    int32_t alternate = 0;
                    buf++;

format_char_switch:
                    /* Conversion specifiers */
                    switch (*buf) {
                        /* Print a literal '%' character */
                        case '%':
                            putc_by_program('%');
                            break;

                        /* Use alternate formatting */
                        case '#':
                            alternate = 1;
                            buf++;
                            /* Yes, I know gotos are bad.  This is the
                             * most elegant and general way to do this,
                             * IMHO. */
                            goto format_char_switch;

                        /* Print a number in hexadecimal form */
                        case 'x':
                            {
                                int8_t conv_buf[64];
                                if (alternate == 0) {
                                    itoa(*((uint32_t *)esp), conv_buf, 16);     // convert the number to a string, 16 is the base
                                    puts(conv_buf);
                                } else {
                                    int32_t starting_index;
                                    int32_t i;
                                    itoa(*((uint32_t *)esp), &conv_buf[8], 16);     // convert the number to a string, 16 is the base,8 is the offset
                                    i = starting_index = strlen(&conv_buf[8]);      // get the length of the string, 8 is the offset
                                    while(i < 8) {
                                        conv_buf[i] = '0';
                                        i++;
                                    }
                                    puts(&conv_buf[starting_index]);
                                }
                                esp++;
                            }
                            break;

                        /* Print a number in unsigned int form */
                        case 'u':
                            {
                                int8_t conv_buf[36];                    // 36 is the max length of the string
                                itoa(*((uint32_t *)esp), conv_buf, 10);     // convert the number to a string, 10 is the base
                                puts(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a number in signed int form */
                        case 'd':
                            {
                                int8_t conv_buf[36];
                                int32_t value = *((int32_t *)esp);
                                if(value < 0) {
                                    conv_buf[0] = '-';
                                    itoa(-value, &conv_buf[1], 10);
                                } else {
                                    itoa(value, conv_buf, 10);
                                }
                                puts(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a single character */
                        case 'c':
                            putc_by_program((uint8_t) *((int32_t *)esp));
                            esp++;
                            break;

                        /* Print a NULL-terminated string */
                        case 's':
                            puts(*((int8_t **)esp));
                            esp++;
                            break;

                        default:
                            break;
                    }

                }
                break;

            default:
                putc_by_program(*buf);
                break;
        }
        buf++;
    }
    return (buf - format);
}

/* int32_t puts(int8_t* s);
 *   Inputs: int_8* s = pointer to a string of characters
 *   Return Value: Number of bytes written
 *    Function: Output a string to the console */
int32_t puts(int8_t* s) {
    register int32_t index = 0;
    while (s[index] != '\0') {
        putc_by_program(s[index]);
        index++;
    }
    return index;
}

/* void putc(uint8_t c);
 * Inputs: uint_8* c = character to print
 * Return Value: void
 *  Function: Output a character to the console */
// modified to handle backspace and newline at checkpoint 2
void putc_by_program(uint8_t c) {
    int flags;
    int x, whether_visible;
    terminal_t* temp_terminal;
    whether_visible = 0;
    cli_and_save(flags); // disable interrupts and save flags
    if(cur_term == visible_term) whether_visible = 1; // check if the terminal is currently displayed
    if(whether_visible) change_term_vidmap_same();
    //else change_term_vidmap(cur_term);
    temp_terminal = get_term(cur_term); // get the terminal that is currently running
    if(c == '\n' || c == '\r') {
        add_newline(cur_term); // add a new line if the char is a newline
    }
    else if (c == '\b'){
        delete_char(cur_term); // delete a char if the char is a backspace
    } 
    else {
        x = temp_terminal->cursor_offset % NUM_COLS;
        if(x >= NUM_COLS-1){
            add_newline(cur_term); // add a new line if the cursor is at the end of the line
        }
        write_char(c, 0, cur_term); // write the char to the screen
    }
    if(whether_visible) change_term_vidmap(cur_term);
    restore_flags(flags); // restore flags
}

void putc_by_keyboard(uint8_t c) {
    int flags;
    int x;
    cli_and_save(flags); // disable interrupts and save flags
    change_term_vidmap_same();
    if(c == '\n' || c == '\r') {
        add_newline(visible_term); // add a new line if the char is a newline
    }
    else if (c == '\b'){
        delete_char(visible_term); // delete a char if the char is a backspace
    } 
    else {
        x = terminal->cursor_offset % NUM_COLS;
        if(x >= NUM_COLS-1){
            add_newline(visible_term); // add a new line if the cursor is at the end of the line
        }
        write_char(c, 0, visible_term); // write the char to the screen
    }
    change_term_vidmap(visible_term);
    restore_flags(flags); // restore flags
}

/* int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix);
 * Inputs: uint32_t value = number to convert
 *            int8_t* buf = allocated buffer to place string in
 *          int32_t radix = base system. hex, oct, dec, etc.
 * Return Value: number of bytes written
 * Function: Convert a number to its ASCII representation, with base "radix" */
int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix) {
    static int8_t lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int8_t *newbuf = buf;
    int32_t i;
    uint32_t newval = value;

    /* Special case for zero */
    if (value == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }

    /* Go through the number one place value at a time, and add the
     * correct digit to "newbuf".  We actually add characters to the
     * ASCII string from lowest place value to highest, which is the
     * opposite of how the number should be printed.  We'll reverse the
     * characters later. */
    while (newval > 0) {
        i = newval % radix;
        *newbuf = lookup[i];
        newbuf++;
        newval /= radix;
    }

    /* Add a terminating NULL */
    *newbuf = '\0';

    /* Reverse the string and return */
    return strrev(buf);
}

/* int8_t* strrev(int8_t* s);
 * Inputs: int8_t* s = string to reverse
 * Return Value: reversed string
 * Function: reverses a string s */
int8_t* strrev(int8_t* s) {
    register int8_t tmp;
    register int32_t beg = 0;
    register int32_t end = strlen(s) - 1;

    while (beg < end) {
        tmp = s[end];
        s[end] = s[beg];
        s[beg] = tmp;
        beg++;
        end--;
    }
    return s;
}

/* uint32_t strlen(const int8_t* s);
 * Inputs: const int8_t* s = string to take length of
 * Return Value: length of string s
 * Function: return length of string s */
uint32_t strlen(const int8_t* s) {
    register uint32_t len = 0;
    while (s[len] != '\0')
        len++;
    return len;
}

/* void* memset(void* s, int32_t c, uint32_t n);
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set n consecutive bytes of pointer s to value c */
void* memset(void* s, int32_t c, uint32_t n) {
    c &= 0xFF;
    asm volatile ("                 \n\
            .memset_top:            \n\
            testl   %%ecx, %%ecx    \n\
            jz      .memset_done    \n\
            testl   $0x3, %%edi     \n\
            jz      .memset_aligned \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            subl    $1, %%ecx       \n\
            jmp     .memset_top     \n\
            .memset_aligned:        \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            movl    %%ecx, %%edx    \n\
            shrl    $2, %%ecx       \n\
            andl    $0x3, %%edx     \n\
            cld                     \n\
            rep     stosl           \n\
            .memset_bottom:         \n\
            testl   %%edx, %%edx    \n\
            jz      .memset_done    \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            subl    $1, %%edx       \n\
            jmp     .memset_bottom  \n\
            .memset_done:           \n\
            "
            :
            : "a"(c << 24 | c << 16 | c << 8 | c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memset_word(void* s, int32_t c, uint32_t n);
 * Description: Optimized memset_word
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set lower 16 bits of n consecutive memory locations of pointer s to value c */
void* memset_word(void* s, int32_t c, uint32_t n) {
    asm volatile ("                 \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            cld                     \n\
            rep     stosw           \n\
            "
            :
            : "a"(c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memset_dword(void* s, int32_t c, uint32_t n);
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set n consecutive memory locations of pointer s to value c */
void* memset_dword(void* s, int32_t c, uint32_t n) {
    asm volatile ("                 \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            cld                     \n\
            rep     stosl           \n\
            "
            :
            : "a"(c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memcpy(void* dest, const void* src, uint32_t n);
 * Inputs:      void* dest = destination of copy
 *         const void* src = source of copy
 *              uint32_t n = number of byets to copy
 * Return Value: pointer to dest
 * Function: copy n bytes of src to dest */
void* memcpy(void* dest, const void* src, uint32_t n) {
    asm volatile ("                 \n\
            .memcpy_top:            \n\
            testl   %%ecx, %%ecx    \n\
            jz      .memcpy_done    \n\
            testl   $0x3, %%edi     \n\
            jz      .memcpy_aligned \n\
            movb    (%%esi), %%al   \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            addl    $1, %%esi       \n\
            subl    $1, %%ecx       \n\
            jmp     .memcpy_top     \n\
            .memcpy_aligned:        \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            movl    %%ecx, %%edx    \n\
            shrl    $2, %%ecx       \n\
            andl    $0x3, %%edx     \n\
            cld                     \n\
            rep     movsl           \n\
            .memcpy_bottom:         \n\
            testl   %%edx, %%edx    \n\
            jz      .memcpy_done    \n\
            movb    (%%esi), %%al   \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            addl    $1, %%esi       \n\
            subl    $1, %%edx       \n\
            jmp     .memcpy_bottom  \n\
            .memcpy_done:           \n\
            "
            :
            : "S"(src), "D"(dest), "c"(n)
            : "eax", "edx", "memory", "cc"
    );
    return dest;
}

/* void* memmove(void* dest, const void* src, uint32_t n);
 * Description: Optimized memmove (used for overlapping memory areas)
 * Inputs:      void* dest = destination of move
 *         const void* src = source of move
 *              uint32_t n = number of byets to move
 * Return Value: pointer to dest
 * Function: move n bytes of src to dest */
void* memmove(void* dest, const void* src, uint32_t n) {
    asm volatile ("                             \n\
            movw    %%ds, %%dx                  \n\
            movw    %%dx, %%es                  \n\
            cld                                 \n\
            cmp     %%edi, %%esi                \n\
            jae     .memmove_go                 \n\
            leal    -1(%%esi, %%ecx), %%esi     \n\
            leal    -1(%%edi, %%ecx), %%edi     \n\
            std                                 \n\
            .memmove_go:                        \n\
            rep     movsb                       \n\
            "
            :
            : "D"(dest), "S"(src), "c"(n)
            : "edx", "memory", "cc"
    );
    return dest;
}

/* int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
 * Inputs: const int8_t* s1 = first string to compare
 *         const int8_t* s2 = second string to compare
 *               uint32_t n = number of bytes to compare
 * Return Value: A zero value indicates that the characters compared
 *               in both strings form the same string.
 *               A value greater than zero indicates that the first
 *               character that does not match has a greater value
 *               in str1 than in str2; And a value less than zero
 *               indicates the opposite.
 * Function: compares string 1 and string 2 for equality */
int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n) {
    int32_t i;
    for (i = 0; i < n; i++) {
        if ((s1[i] != s2[i]) || (s1[i] == '\0') /* || s2[i] == '\0' */) {

            /* The s2[i] == '\0' is unnecessary because of the short-circuit
             * semantics of 'if' expressions in C.  If the first expression
             * (s1[i] != s2[i]) evaluates to false, that is, if s1[i] ==
             * s2[i], then we only need to test either s1[i] or s2[i] for
             * '\0', since we know they are equal. */
            return s1[i] - s2[i];
        }
    }
    return 0;
}

/* int8_t* strcpy(int8_t* dest, const int8_t* src)
 * Inputs:      int8_t* dest = destination string of copy
 *         const int8_t* src = source string of copy
 * Return Value: pointer to dest
 * Function: copy the source string into the destination string */
int8_t* strcpy(int8_t* dest, const int8_t* src) {
    int32_t i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

/* int8_t* strcpy(int8_t* dest, const int8_t* src, uint32_t n)
 * Inputs:      int8_t* dest = destination string of copy
 *         const int8_t* src = source string of copy
 *                uint32_t n = number of bytes to copy
 * Return Value: pointer to dest
 * Function: copy n bytes of the source string into the destination string */
int8_t* strncpy(int8_t* dest, const int8_t* src, uint32_t n) {
    int32_t i = 0;
    while (src[i] != '\0' && i < n) {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i] = '\0';
        i++;
    }
    return dest;
}

/* void test_interrupts(void)
 * Inputs: void
 * Return Value: void
 * Function: increments video memory. To be used to test rtc */
void test_interrupts(void) {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        video_mem[i << 1]++;
    }
}

// ------------------------------------------------------------------------------------------//
//--------------------------------------FUNCTIONS ADD CKP2-----------------------------------//
// ------------------------------------------------------------------------------------------//

// as described in: wiki.osdev.org/Text_Mode_Cursor
// but change the port and data
void cursor_enable(uint8_t cursor_start, uint8_t cursor_end){
    outb(0x0A, 0x3D4);
    outb(cursor_start | (inb(0x3D5) & 0xC0), 0x3D5); // instructions in wiki.osdev.org/Text_Mode_Cursor
    outb(0x0B, 0x3D4);
    outb(cursor_end | (inb(0x3D5) & 0xE0), 0x3D5); // instructions in wiki.osdev.org/Text_Mode_Cursor
}

// as described in: wiki.osdev.org/Text_Mode_Cursor
// but change the port and data
void cursor_disable(void){
    outb(0x0A, 0x3D4);
    outb(0x20, 0x3D5); // instructions in wiki.osdev.org/Text_Mode_Cursor
}

// as described in: wiki.osdev.org/Text_Mode_Cursor
// but change the port and data
void cursor_moving(int x, int y){
    uint16_t pos = y * NUM_COLS + x;
    outb(0x0F, 0x3D4);
    outb(((uint8_t)(pos & 0xFF)), 0x3D5); // instructions in wiki.osdev.org/Text_Mode_Cursor
    outb(0x0E, 0x3D4);
    outb(((uint8_t)((pos >> 8)) & 0xFF), 0x3D5); // instructions in wiki.osdev.org/Text_Mode_Cursor
}

// as described in: wiki.osdev.org/Text_Mode_Cursor
// but change the port and data
uint16_t cursor_get_pos(void){
    uint16_t pos = 0;
    outb(0x0F, 0x3D4);
    pos |= inb(0x3D5); // instructions in wiki.osdev.org/Text_Mode_Cursor
    outb(0x0E, 0x3D4);
    pos |= (((uint16_t)inb(0x3D5)) << 8);
    return pos; // return as a 16-bit val like y*NUM_COLS + x
}

// moving up the terminal by one line
// input: none
// output: none
// side effect: move the screen up by one line
void lines_moving_up(uint8_t term_num){
    int i, x, y;
    x = terminal->cursor_offset % NUM_COLS; // get the x position of the cursor
    y = terminal->cursor_offset / NUM_COLS; // get the y position of the cursor
    for(i = 0; i < NUM_ROWS-1 ; i++){
        // copy the next line to the current line (i.e. move the screen up by one line)
        uint8_t* curr_line = (uint8_t*)(video_mem + ((NUM_COLS * i + 0) << 1)); // << 1 because we want to move by 2 bytes
        uint8_t* next_line = (uint8_t*)(video_mem + ((NUM_COLS * (i+1) + 0) << 1)); // << 1 because we want to move by 2 bytes
        memcpy(curr_line, next_line, (NUM_COLS << 1));
    }
    for(i = 0; i < NUM_COLS; i++){
        uint8_t* last_line_begin = (uint8_t*)(video_mem + ((NUM_COLS * (NUM_ROWS-1) + i) << 1));
        *last_line_begin = ' ';
        *(last_line_begin + 1) = term_color[term_num]; // set the last line to be empty
    }
    if(y > 0){
        y--; // move the cursor to the previous line if possible (cannot move up if the cursor is at the first line)
    }
    x=0; // move the cursor to the beginning of the line
    terminal->cursor_offset = y * NUM_COLS + x; // update the cursor offset
    if(term_num == visible_term) cursor_moving(x, y); // move the cursor to the new position
}

// add a new line to the screen
// input: none
// output: none
// side effect: add a new line to the screen (the first line maybe be removed)
void add_newline(uint8_t term_num){
    int x, y;
    terminal_t* temp_terminal;
    temp_terminal = get_term(term_num); // get the terminal that is currently running
    x = temp_terminal->cursor_offset % NUM_COLS; // get the x position of the cursor
    y = temp_terminal->cursor_offset / NUM_COLS; // get the y position of the cursor
    if(y == NUM_ROWS-1){ // if the cursor is at the last line of the screen
        lines_moving_up(term_num); // move the screen up by one line
    }else{
        y++; // move the cursor to the next line
    }
    x = 0; // move the cursor to the beginning of the line
    temp_terminal->cursor_offset = y * NUM_COLS + x; // update the cursor offset
    if(term_num == visible_term) cursor_moving(x, y); // move the cursor to the new position if the terminal is currently displayed
}

// delete a char at current cursor of the screen
// input: none
// output: none
// side effect: delete a char at current cursor of the screen
void delete_char(uint8_t term_num){
    int x, y;
    terminal_t* temp_terminal;
    temp_terminal = get_term(term_num); // get the terminal that is currently running
    x = temp_terminal->cursor_offset % NUM_COLS; // get the x position of the cursor
    y = temp_terminal->cursor_offset / NUM_COLS; // get the y position of the cursor
    if(x == 0){ // if the cursor is at the beginning of the line
        if(y == 0){ // if the cursor is at the beginning of the screen
            return;
        }else{
            y--; // move the cursor to the end of the previous line
            x = NUM_COLS-1;
        }
    }else{
        x--; // move the cursor to the left by one char
    }
    temp_terminal->cursor_offset = y * NUM_COLS + x; // update the cursor offset
    write_char(' ', 1, term_num); // write a space to the screen
}

// write a char to the screen
// input: uint8_t c = the char to write
// output: none
// side effect: write a char to the screen
void write_char(uint8_t c, int del, uint8_t term_num){
    int x, y;
    terminal_t* temp_terminal;
    temp_terminal = get_term(term_num); // get the terminal that is currently running
    x = temp_terminal->cursor_offset % NUM_COLS; // get the x position of the cursor
    y = temp_terminal->cursor_offset / NUM_COLS; // get the y position of the cursor
    uint8_t* curr_char = (uint8_t*)(video_mem + ((NUM_COLS * y + x) << 1));
    if(term_num!=visible_term) curr_char = (uint8_t*)(video_mem + 2*PAGE_SIZE*(term_num+1) + ((NUM_COLS * y + x) << 1));
    *curr_char = c; // write the char to the screen
    *(curr_char + 1) = term_color[term_num]; // set the attribute of the char to be ATTRIB
    if(del == 0) x++; // move the cursor to the right by one char
    temp_terminal->cursor_offset = y * NUM_COLS + x; // update the cursor offset
    if(term_num == visible_term) cursor_moving(x, y); // update the cursor position on the screen after writing the char
}

/* void clear_screen(void);
 * Inputs: void
 * Return Value: none
 * Function: Clears video memory and resets cursor position
*/
void clear_screen(void){
    cursor_moving(0, 0); // reset cursor position
}

/* void sleep(uint32_t seconds);
 * Inputs: seconds -- number of seconds to sleep
 * Return Value: none
 * Function: sleep for a certain number of seconds
*/
void sleep(uint32_t seconds) {
    uint32_t fre = get_frequency();
	uint32_t i = seconds * fre;
	while(i > 0) {
		rtc_read(0, NULL, 0);
		i--;
	}
}

// something for fun :)
void anEgg(void){
    if(term_color[visible_term] == 0x02){       // 0x02 is green
        //USING putcharbykeyboard
        putc_by_keyboard('H');
        putc_by_keyboard('a');
        putc_by_keyboard('c');
        putc_by_keyboard('k');
        putc_by_keyboard('e');
        putc_by_keyboard('r');
        putc_by_keyboard(' ');
        putc_by_keyboard('M');
        putc_by_keyboard('o');
        putc_by_keyboard('d');
        putc_by_keyboard('e');
        putc_by_keyboard(' ');
        putc_by_keyboard('O');
        putc_by_keyboard('n');
        putc_by_keyboard('!');
        putc_by_keyboard('\n');
        putc_by_keyboard('3');
        putc_by_keyboard('9');
        putc_by_keyboard('1');
        putc_by_keyboard('O');
        putc_by_keyboard('S');
        putc_by_keyboard('>');
        putc_by_keyboard(' ');
    }
}

