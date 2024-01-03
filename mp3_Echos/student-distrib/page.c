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
page_table_entry_t vid_page_table[PAGE_TABLE_SIZE]__attribute__((aligned(PAGE_SIZE)));

// initialize page directory and page table
// input: none
// output: none
// side effect: initialize page directory and page table
void paging_init(void){
    int i;
    for(i = 0; i < 1024; i++){ // initialize page directory and page table because they have 1024 entries, same size，1024 is the size of page directory and page table
        page_directory[i].val = 0; // initialize page directory
        page_table[i].val = 0; // initialize page table
        vid_page_table[i].val=0; //initialize video page table
    }
    
    // set up the entry 0xB8 in page table for video memory
    // all these number are referenced in intel version 3
    //  kernal vid
    page_table[VIDEO_PAGE_BEGIN>>12].val |= 0x00000003; // set present bit to 1, read and write bit to 1, 12 is the offset of the page table
    page_table[VIDEO_PAGE_BEGIN>>12].val |= VIDEO_PAGE_BEGIN; // set page table address to 0xB8
    
    //  term 1 vid
    page_table[(VIDEO_PAGE_BEGIN>>12)+2].val |= 0x00000003; // set present bit to 1, read and write bit to 1, 12 is the offset of the page table
    page_table[(VIDEO_PAGE_BEGIN>>12)+2].val |= (VIDEO_PAGE_BEGIN+PAGE_SIZE*2); // set page table address to 0xB8
    //  term 2 vid
    page_table[(VIDEO_PAGE_BEGIN>>12)+4].val |= 0x00000003; // set present bit to 1, read and write bit to 1, 12 is the offset of the page table
    page_table[(VIDEO_PAGE_BEGIN>>12)+4].val |= (VIDEO_PAGE_BEGIN+PAGE_SIZE*4); // set page table address to 0xB8
    // term 3 vid
    page_table[(VIDEO_PAGE_BEGIN>>12)+6].val |= 0x00000003; // set present bit to 1, read and write bit to 1, 12 is the offset of the page table
    page_table[(VIDEO_PAGE_BEGIN>>12)+6].val |= (VIDEO_PAGE_BEGIN+PAGE_SIZE*6); // set page table address to 0xB8

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
        :: "r" (pg_directory): "%eax");
}

// setting a page for user
// input: shell, the user program
// output: none
// side effect: creat a page in virtual mem
void 
user_paging(uint8_t shell){
    //we only need the high 6 bits of 128 MB to index so we right shift 22
    page_directory[PROGRAM_IMAGE>>22].present=1;   
    page_directory[PROGRAM_IMAGE>>22].read_write=1; 
    page_directory[PROGRAM_IMAGE>>22].supervisor=1; 
    page_directory[PROGRAM_IMAGE>>22].write_through=0;
    page_directory[PROGRAM_IMAGE>>22].cache_disabled=1;     // fill the corresponding field for user prog
    page_directory[PROGRAM_IMAGE>>22].accessed =0;
    page_directory[PROGRAM_IMAGE>>22].dirty=0;
    page_directory[PROGRAM_IMAGE>>22].page_size=1;
    page_directory[PROGRAM_IMAGE>>22].global_page=0;
    page_directory[PROGRAM_IMAGE>>22].available=0;
    page_directory[PROGRAM_IMAGE>>22].page_table_address=(PROGRAM_First+shell*PROGRAM_Size)>>12;  //we only need the high 20 bits to index
    asm volatile("              \n\
        movl %0, %%eax          \n\
        movl %%eax, %%cr3       \n\
        "
        :: "r" (page_directory): "%eax"                     
    );    
    //ENABLE_PAGE(page_directory); // enable paging flush TLB
}
// header for user video mem paging
// input: pointer to the start of the screen start
// output: none
// side effect: creat a page in virtual mem
void
set_user_vidmap_paging(uint8_t** screen_start){

    vid_page_table[((PROGRAM_VIDEO>>12)&0x3FF)].present=1;          // 12 is the offset of the page table
    vid_page_table[((PROGRAM_VIDEO>>12)&0x3FF)].read_write=1;       // 0x3FF is the mask for the 10 bits
    vid_page_table[((PROGRAM_VIDEO>>12)&0x3FF)].supervisor=1;
    vid_page_table[((PROGRAM_VIDEO>>12)&0x3FF)].write_through=0;
    vid_page_table[((PROGRAM_VIDEO>>12)&0x3FF)].cache_disabled=0;  // fill the corresponding field for user prog vid
    vid_page_table[((PROGRAM_VIDEO>>12)&0x3FF)].accessed=0;
    vid_page_table[((PROGRAM_VIDEO>>12)&0x3FF)].dirty=0;
    vid_page_table[((PROGRAM_VIDEO>>12)&0x3FF)].page_table_attribute=0;
    vid_page_table[((PROGRAM_VIDEO>>12)&0x3FF)].global_page=0;
    vid_page_table[((PROGRAM_VIDEO>>12)&0x3FF)].available=0;
    vid_page_table[((PROGRAM_VIDEO>>12)&0x3FF)].page_address=((VIDEO_PAGE_BEGIN)>>12);
    //vid_page_table[PROGRAM_VIDEO>>12].val |= 0x00000007;
    //vid_page_table[PROGRAM_VIDEO>>12].val |= (&VIDEO_PAGE_BEGIN);
        
    page_directory[PROGRAM_VIDEO>>22].present=1;                    // 22 is the offset of the page directory
    page_directory[PROGRAM_VIDEO>>22].read_write=1; 
    page_directory[PROGRAM_VIDEO>>22].supervisor=1; 
    page_directory[PROGRAM_VIDEO>>22].write_through=0;
    page_directory[PROGRAM_VIDEO>>22].cache_disabled=0;     // fill the corresponding field for user prog vid
    page_directory[PROGRAM_VIDEO>>22].accessed =0;
    page_directory[PROGRAM_VIDEO>>22].dirty=0;
    page_directory[PROGRAM_VIDEO>>22].page_size=0;
    page_directory[PROGRAM_VIDEO>>22].global_page=0;
    page_directory[PROGRAM_VIDEO>>22].available=0;
    page_directory[PROGRAM_VIDEO>>22].page_table_address=(((uint32_t) &vid_page_table)>>12);  //we only need the high 20 bits to index, 12 is the offset of the page table
    //page_directory[PROGRAM_VIDEO>>22].val |= 0x00000007;
    //page_directory[PROGRAM_VIDEO>>22].val |= (&vid_page_table)>>12;

    *screen_start = (uint8_t*)PROGRAM_VIDEO;
    
    asm volatile("              \n\
        movl %0, %%eax          \n\
        movl %%eax, %%cr3       \n\
        "
        :: "r" (page_directory): "%eax"                     
    );    
}
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// unfinished
// need to inplement the function to change the video memory for different terminal and their user video memory
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void change_term_vidmap(int32_t term_num){
    if(term_num<0 || term_num>2) return;
    page_table[VIDEO_PAGE_BEGIN>>12].val = 0; // set present bit to 0, read and write bit to 0, 12 is the offset of the page table
    page_table[VIDEO_PAGE_BEGIN>>12].val |= 0x00000003; // set present bit to 0, read and write bit to 0, 12 is the offset of the page table
    page_table[VIDEO_PAGE_BEGIN>>12].val |= (((VIDEO_PAGE_BEGIN >> 12)+ 2 + 2 * term_num)<<12); // set page table address to 0xB8
    vid_page_table[(PROGRAM_VIDEO>>12)&0X3FF].val = 0; // set present bit to 0, read and write bit to 0, 12 is the offset of the page table
    vid_page_table[(PROGRAM_VIDEO>>12)&0X3FF].val |= 0x00000007; // set present bit to 0, read and write bit to 0, 12 is the offset of the page table
    vid_page_table[(PROGRAM_VIDEO>>12)&0X3FF].val |= (((VIDEO_PAGE_BEGIN >> 12)+ 2 + 2 * term_num)<<12); // set page table address to 0xB8

    asm volatile("              \n\
        movl %0, %%eax          \n\
        movl %%eax, %%cr3       \n\
        "
        :: "r" (page_directory): "%eax"                     
    );
}

void change_term_vidmap_same(void){
    page_table[VIDEO_PAGE_BEGIN>>12].val = 0; // set present bit to 0, read and write bit to 0, 12 is the offset of the page table
    page_table[VIDEO_PAGE_BEGIN>>12].val |= 0x00000003; // set present bit to 0, read and write bit to 0, 12 is the offset of the page table
    page_table[VIDEO_PAGE_BEGIN>>12].val |= (VIDEO_PAGE_BEGIN); // set page table address to 0xB8
    vid_page_table[(PROGRAM_VIDEO>>12)&0X3FF].val = 0; // set present bit to 0, read and write bit to 0, 12 is the offset of the page table
    vid_page_table[(PROGRAM_VIDEO>>12)&0X3FF].val |= 0x00000007; // set present bit to 0, read and write bit to 0, 12 is the offset of the page table
    vid_page_table[(PROGRAM_VIDEO>>12)&0X3FF].val |= (VIDEO_PAGE_BEGIN); // set page table address to 0xB8

    asm volatile("              \n\
        movl %0, %%eax          \n\
        movl %%eax, %%cr3       \n\
        "
        :: "r" (page_directory): "%eax"                     
    );
}


