#ifndef _SYSTEM_CALLS_H
#define _SYSTEM_CALLS_H

#include "types.h"
#include "lib.h"
#include "x86_desc.h"
#include "page.h"
#include "terminal.h"
#include "rtc.h"
#include "filesystem.h"
#include "scheduling.h"

#define MAX_FD 8
#define MAX_PCB 6   
#define KENEL_BOT 0x800000 //8MB
#define BLOCK     0x2000 //8KB    
#define USER_ESP (0x8000000+0x400000)   //128MB + 4MB(the size the program)
extern uint8_t PCBs;

// struct for file operations table
// four func pointer is included
typedef struct file_ops_table_t {
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t id);
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
} file_ops_table_t;

// struct for file descriptor
// as describde in mp3 doc
typedef struct file_descriptor_t {
    file_ops_table_t* file_ops_table_ptr;
    int32_t inode;
    int32_t file_position;
    int32_t flags;
} file_descriptor_t;

// struct for pcb
// save the important things for proccess
typedef struct PCB PCB_t;
struct PCB{
   uint32_t pid;
   file_descriptor_t file_array[8]; // maxinmum length of fd is 8
   PCB_t    *parent_pcb;
   uint32_t pcb_esp;
   uint32_t pcb_ebp;
   uint32_t schedule_esp;
   uint32_t schedule_ebp;
   int32_t signal;
   uint8_t arguments[32];
   uint8_t arglength;
};

// all sys call functions
int32_t halt(uint8_t status);
int32_t execute(const uint8_t* command);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);
int32_t getargs(uint8_t* buf, int32_t nbytes);
int32_t vidmap(uint8_t** screen_start);
int32_t set_handler(int32_t signum, void* handler_address);
int32_t sigreturn(void);
PCB_t* get_current_pcb();
PCB_t* schedule_pcb[3];
void print_helper_page();
#endif

