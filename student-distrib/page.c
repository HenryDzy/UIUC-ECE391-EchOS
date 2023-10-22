// this file - page.c,  is the main c file for the page directory and page table initialization
// for the kernel and user program
// Author: zixuanq3, haigel2

#include "page.h"
#include "types.h"  
#include "lib.h"
#include "x86_desc.h" 

// set the global variables for page directory and page table
page_directory_entry_t page_directory[PAGE_DIRECTORY_SIZE] __attribute__((aligned(PAGE_SIZE))); // 1024 entries in page directory
page_table_entry_t page_table[PAGE_TABLE_SIZE] __attribute__((aligned(PAGE_SIZE)));      // 1024 entries in page table

// initialize page directory and page table
// input: none
// output: none
// side effect: initialize page directory and page table
void paging_init(void){
    int i;
    for(i = 0; i < 1024; i++){ // initialize page directory and page table because they have 1024 entries, same size
        page_directory[i].val = 0; // initialize page directory
        page_table[i].val = 0; // initialize page table
    }
    
    // set up the entry 0xB8 in page table for video memory
    // all these number are referenced in intel version 3
    page_table[VIDEO_PAGE_BEGIN>>12].val |= 0x00000003; // set present bit to 1, read and write bit to 1
    page_table[VIDEO_PAGE_BEGIN>>12].val |= VIDEO_PAGE_BEGIN; // set page table address to 0xB8

    // set up the page directory entry for kernel
    page_directory[1].val |= 0x00000193; // set present bit to 1, read and write bit to 1, page size bit to 1
    page_directory[1].val |= KERNEL_PAGE_BEGIN; // set page table address to 0x4

    // set up the page directory entry for page table
    page_directory[0].val |= 0x0000000B; // set present bit to 1, read and write bit to 1, page size bit to 1
    page_directory[0].val |= ((uint32_t) &page_table); // set page table address to 0x4

    ENABLE_PAGE(page_directory); // enable paging
}

// description: enable paging by setting cr3, cr0, cr4
// input: page_directory, a pointer of the page directory to be enabled
// output: none
// side effect: enable paging by setting cr3, cr0, cr4, and eax will be modified
void 
ENABLE_PAGE(page_directory_entry_t* pg_directory){
    asm volatile("              \n\
        movl %0, %%eax          \n\
        movl %%eax, %%cr3       \n\
        movl %%cr4, %%eax       \n\
        orl $0x00000010, %%eax  \n\
        movl %%eax, %%cr4       \n\
        movl %%cr0, %%eax       \n\
        orl $0x80000000, %%eax  \n\
        movl %%eax, %%cr0       \n\
        "
        :                           
        : "r" (pg_directory)      
        : "%eax"                     
    );
}




