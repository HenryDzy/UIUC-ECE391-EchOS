// this file - page.h, is the header for the page directory and page table initialization
// for the kernel and user program
// Author: zixuanq3, haigel2

#ifndef _PAGE_H
#define _PAGE_H

#include "types.h"
#include "lib.h"
#include "x86_desc.h"

#define PAGE_SIZE 4096 // 4KB page size
#define BIG_PAGE_SIZE 0x400000 // 4MB page size

// --------------------rules for paging -------------------- //
// for kernel and video memory, addr of physical memory is the same as virtual memory
// for user program, addr of physical memory starts at 8MB, virtual memory starts at 128MB
// --------------------------end---------------------------- //

// begin at 4MB, end at 8MB (4MB) for kernel
#define KERNEL_PAGE_BEGIN 0x400000 // 0x400000 is the beginning of kernel memory (4MB) in physical memory same as 4MB in virtual memory

// from lib.c 
// and we find in history: 0xB8000 is the beginning of video memory in VGA, 0xC0000 is the end of video memory in VGA
#define VIDEO_PAGE_BEGIN 0xB8000 // 0xB8000 is the beginning of video memory in VGA
#define PROGRAM_IMAGE 0x8000000 // 128 MB
#define PROGRAM_First 0x800000 // 8 MB
#define PROGRAM_Size 0x400000 // 4 MB
#define PAGE_DIRECTORY_SIZE 1024 // 1024 entries in page directory
#define PAGE_TABLE_SIZE 1024 // 1024 entries in page table
#define PROGRAM_VIDEO (PROGRAM_IMAGE+PROGRAM_Size)

// modified from x86_desc.h descriptor type
// page directory entry type, each entry to a page table
typedef struct page_directory_entry_t {
    union{
        uint32_t val; // 32 bits value for page directory entry
        struct{
            uint32_t present          :1; // 1 means present 
            uint32_t read_write       :1; // 1 means read and write
            uint32_t supervisor       :1; // 1 means kernel level 
            uint32_t write_through    :1; // 1 means write through
            uint32_t cache_disabled   :1; // 1 means cache disabled 
            uint32_t accessed         :1; // 1 means accessed 
            uint32_t dirty            :1; // 1 means reserved
            uint32_t page_size        :1; // 1 means 4096 page size
            uint32_t global_page      :1; // 1 bits available for global FOR BIG PAGE
            uint32_t available        :3; // 4 bits available for programmer
            uint32_t page_table_address :20; // 20 bits page table base address 
        } __attribute__((packed)); // packed attribute is used to avoid the compiler to optimize the struct
    };
} page_directory_entry_t;


// modified from page_directory_entry_t
// page table entry type, each entry to a page
typedef struct page_table_entry_t {
    union{
        uint32_t val; // 32 bits value for page table entry
        struct{
            uint32_t present                    :1; // 1 means present
            uint32_t read_write                 :1; // 1 means read and write
            uint32_t supervisor                 :1; // 1 means kernel level
            uint32_t write_through              :1; // 1 means write through 
            uint32_t cache_disabled             :1; // 1 means cache disabled 
            uint32_t accessed                   :1; // 1 means accessed 
            uint32_t dirty                      :1; // 1 means dirty 
            uint32_t page_table_attribute       :1; // 1 means page table attribute index
            uint32_t global_page                :1; // 1 means global page 
            uint32_t available                  :3; // 3 bits available for programmer 
            uint32_t page_address :20; // 20 bits page base address 
        } __attribute__((packed)); // packed attribute is used to avoid the compiler to optimize the struct
    };
} page_table_entry_t;                                                                                                                  


extern page_directory_entry_t page_directory[PAGE_DIRECTORY_SIZE]; // 1024 entries in page directory
extern page_table_entry_t page_table[PAGE_TABLE_SIZE]; // 1024 entries in page table
extern page_table_entry_t vid_page_table[PAGE_TABLE_SIZE];

// --------------------Functions for paging -------------------- //

// initialize page directory and page table for user program
void paging_init(void);

//helper function to enable paging and set cr3
void ENABLE_PAGE(page_directory_entry_t* pg_directory); // enable paging by setting cr3, cr0, cr4

// helper func for create the user prog page
void user_paging(uint8_t shell);

// helper func for creating the user program video page
void set_user_vidmap_paging(uint8_t** screen_start);

void change_term_vidmap(int32_t term_num);

void change_term_vidmap_same(void);
#endif

