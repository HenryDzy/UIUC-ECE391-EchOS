/* kernel.c - the C part of the kernel
 * vim:ts=4 noexpandtab
 */

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"
#include "idt.h"
#include "rtc.h"
#include "keyboard.h"
#include "page.h"
#include "terminal.h"
#include "filesystem.h"
#include "system_calls.h"
#include "scheduling.h"
#include "mouse.h"
#define RUN_TESTS
#define I_COLOR 0xCC
#define ECHOS_COLOR 0xff
#define char_w 9
#define char_h 5
#define char_start_y 9
#define e_start_x 7
#define c_start_x 21
#define h_start_x 35
#define o_start_x 49
#define s_start_x 63

/* Macros. */
/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags, bit)   ((flags) & (1 << (bit)))

/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void entry(unsigned long magic, unsigned long addr) {
    int32_t i, j;
    int x, y;
    multiboot_info_t *mbi;
    uint32_t fs_start_addr;
    i = 0;
    j = 0;
    /* Clear the screen. */
    clear();
    clear_screen();
    /* Am I booted by a Multiboot-compliant boot loader? */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        printf("Invalid magic number: 0x%#x\n", (unsigned)magic);
        return;
    }

    /* Set MBI to the address of the Multiboot information structure. */
    mbi = (multiboot_info_t *) addr;

    /* Print out the flags. */
    //printf("flags = 0x%#x\n", (unsigned)mbi->flags);                          // 封面去掉了这一行

    /* Are mem_* valid? */
    if (CHECK_FLAG(mbi->flags, 0))
        //printf("mem_lower = %uKB, mem_upper = %uKB\n", (unsigned)mbi->mem_lower, (unsigned)mbi->mem_upper); // 封面去掉了这一行

    /* Is boot_device valid? */
    if (CHECK_FLAG(mbi->flags, 1))
        //printf("boot_device = 0x%#x\n", (unsigned)mbi->boot_device);  // 封面去掉了这一行

    /* Is the command line passed? */
    if (CHECK_FLAG(mbi->flags, 2))
        //printf("cmdline = %s\n", (char *)mbi->cmdline);        // 封面去掉了这一行

    if (CHECK_FLAG(mbi->flags, 3)) {
        int mod_count = 0;
        int i;
        module_t* mod = (module_t*)mbi->mods_addr;
        while (mod_count < mbi->mods_count) {
           // printf("Module %d loaded at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_start);// 封面去掉了这一行
            //printf("Module %d ends at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_end);// 封面去掉了这一行
            //printf("First few bytes of module:\n");// 封面去掉了这一行
            for (i = 0; i < 16; i++) {
                //printf("0x%x ", *((char*)(mod->mod_start+i)));// 封面去掉了这一行
            }
            //printf("\n");// 封面去掉了这一行
            fs_start_addr = mod->mod_start;
            mod_count++;
            mod++;
        }
    }
    /* Bits 4 and 5 are mutually exclusive! */
    if (CHECK_FLAG(mbi->flags, 4) && CHECK_FLAG(mbi->flags, 5)) {
        printf("Both bits 4 and 5 are set.\n");    
        return;
    }

    /* Is the section header table of ELF valid? */
    if (CHECK_FLAG(mbi->flags, 5)) {
        //elf_section_header_table_t *elf_sec = &(mbi->elf_sec);// 封面去掉了这一行
        // printf("elf_sec: num = %u, size = 0x%#x, addr = 0x%#x, shndx = 0x%#x\n",
        //         (unsigned)elf_sec->num, (unsigned)elf_sec->size,
        //         (unsigned)elf_sec->addr, (unsigned)elf_sec->shndx);          // 封面去掉了这一行
    }

    /* Are mmap_* valid? */
    if (CHECK_FLAG(mbi->flags, 6)) {
        memory_map_t *mmap;
        // printf("mmap_addr = 0x%#x, mmap_length = 0x%x\n",
        //         (unsigned)mbi->mmap_addr, (unsigned)mbi->mmap_length);       // 封面去掉了这一行
        for (mmap = (memory_map_t *)mbi->mmap_addr;
                (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length;
                mmap = (memory_map_t *)((unsigned long)mmap + mmap->size + sizeof (mmap->size)));
            // printf("    size = 0x%x, base_addr = 0x%#x%#x\n    type = 0x%x,  length    = 0x%#x%#x\n",
            //         (unsigned)mmap->size,
            //         (unsigned)mmap->base_addr_high,
            //         (unsigned)mmap->base_addr_low,
            //         (unsigned)mmap->type,
            //         (unsigned)mmap->length_high,
            //         (unsigned)mmap->length_low);                     // 封面去掉了这一行
    }

    /* Construct an LDT entry in the GDT */
    {
        seg_desc_t the_ldt_desc;
        the_ldt_desc.granularity = 0x0;
        the_ldt_desc.opsize      = 0x1;
        the_ldt_desc.reserved    = 0x0;
        the_ldt_desc.avail       = 0x0;
        the_ldt_desc.present     = 0x1;
        the_ldt_desc.dpl         = 0x0;
        the_ldt_desc.sys         = 0x0;
        the_ldt_desc.type        = 0x2;

        SET_LDT_PARAMS(the_ldt_desc, &ldt, ldt_size);
        ldt_desc_ptr = the_ldt_desc;
        lldt(KERNEL_LDT);
    }

    /* Construct a TSS entry in the GDT */
    {
        seg_desc_t the_tss_desc;
        the_tss_desc.granularity   = 0x0;
        the_tss_desc.opsize        = 0x0;
        the_tss_desc.reserved      = 0x0;
        the_tss_desc.avail         = 0x0;
        the_tss_desc.seg_lim_19_16 = TSS_SIZE & 0x000F0000;
        the_tss_desc.present       = 0x1;
        the_tss_desc.dpl           = 0x0;
        the_tss_desc.sys           = 0x0;
        the_tss_desc.type          = 0x9;
        the_tss_desc.seg_lim_15_00 = TSS_SIZE & 0x0000FFFF;

        SET_TSS_PARAMS(the_tss_desc, &tss, tss_size);

        tss_desc_ptr = the_tss_desc;

        tss.ldt_segment_selector = KERNEL_LDT;
        tss.ss0 = KERNEL_DS;
        tss.esp0 = 0x800000;
        ltr(KERNEL_TSS);
    }
    init_filesystem(fs_start_addr);
    IDT_init();
    paging_init();
    i8259_init();
    init_term();
    // printf("Enabling Interrupts\n");                 // 封面去掉了这一行
    /* Init the PIC */
    /* Initialize devices, memory, filesystem, enable device interrupts on the
     * PIC, any other initialization stuff... */

    /* Enable interrupts */
    /* Do not enable the following until after you have set up your
     * IDT correctly otherwise QEMU will triple fault and simple close
     * without showing you any output */
    keyboard_init();
    RTC_init();
#ifdef RUN_TESTS
    /* Run tests */
    launch_tests();
    init_pit();
    mouse_install();
#endif
    /* Execute the first program ("shell") ... */
    clear();
    clear_screen();
    // print a beautiful cover, with orange I in the center with white lace and blue background
    for(y = 0; y < 24; y++){
        printf("                                                                               \n");
    }
    for(x = 26; x < 53; x++){
        change_ATTRIB_FOR_COVER(0xff, x, 3);
    }
    for(y = 4; y < 7; y++){
        change_ATTRIB_FOR_COVER(0xff, 26, y);       // 0xff is white
        change_ATTRIB_FOR_COVER(0xff, 52, y);
        for(x = 27; x < 52; x++){     // fill with orange
            change_ATTRIB_FOR_COVER(I_COLOR, x, y);   // 0x33 is red
        }
    }
    for(x = 26; x < 33; x++){
        change_ATTRIB_FOR_COVER(0xff, x, 7);
    }
    for(x = 46; x < 53; x++){
        change_ATTRIB_FOR_COVER(0xff, x, 7);
    }
    for(x = 33; x < 46; x++){
        change_ATTRIB_FOR_COVER(I_COLOR, x, 7);
    }
    for(y = 8; y < 15; y++){
        change_ATTRIB_FOR_COVER(0xff, 32, y);       // 0xff is white
        change_ATTRIB_FOR_COVER(0xff, 46, y);
        for(x = 33; x < 46; x++){     // fill with orange
            change_ATTRIB_FOR_COVER(I_COLOR, x, y);   // 0x33 is red
        }
    }
    for(x = 26; x < 33; x++){
        change_ATTRIB_FOR_COVER(0xff, x, 15);
    }
    for(x = 46; x < 53; x++){
        change_ATTRIB_FOR_COVER(0xff, x, 15);
    }
    for(x = 33; x < 46; x++){
        change_ATTRIB_FOR_COVER(I_COLOR, x, 15);
    }
    for(y = 16; y < 19; y++){
        change_ATTRIB_FOR_COVER(0xff, 26, y);       // 0xff is white
        change_ATTRIB_FOR_COVER(0xff, 52, y);
        for(x = 27; x < 52; x++){     // fill with orange
            change_ATTRIB_FOR_COVER(I_COLOR, x, y);   // 0x33 is red
        }
    }
    for(x = 26; x < 53; x++){
        change_ATTRIB_FOR_COVER(0xff, x, 19);
    }

    while(j < 300000000) j++;

// draw e
    for(y = char_start_y; y < char_start_y + char_h; y++){
        for(x = e_start_x; x < e_start_x + 2; x++){
            change_ATTRIB_FOR_COVER(ECHOS_COLOR, x, y);
        }
    }
    for(x = e_start_x + 2; x < e_start_x + char_w; x++){
        change_ATTRIB_FOR_COVER(ECHOS_COLOR, x, char_start_y);
        change_ATTRIB_FOR_COVER(ECHOS_COLOR, x, char_start_y + 2);
        change_ATTRIB_FOR_COVER(ECHOS_COLOR, x, char_start_y + 4);
    }

    while(j < 350000000) j++;

// draw c
    for(y = char_start_y; y < char_start_y + char_h; y++){
        for(x = c_start_x; x < c_start_x + 2; x++){
            change_ATTRIB_FOR_COVER(ECHOS_COLOR, x, y);
        }
    }
    for(x = c_start_x + 2; x < c_start_x + char_w; x++){
        change_ATTRIB_FOR_COVER(ECHOS_COLOR, x, char_start_y);
        change_ATTRIB_FOR_COVER(ECHOS_COLOR, x, char_start_y + 4);
    }

    while(j < 550000000) j++;

// draw h
    for(y = char_start_y; y < char_start_y + char_h; y++){
        for(x = h_start_x; x < h_start_x + 2; x++){
            change_ATTRIB_FOR_COVER(ECHOS_COLOR, x, y);
        }
        for(x = h_start_x + 7; x < h_start_x + 9; x++){
            change_ATTRIB_FOR_COVER(ECHOS_COLOR, x, y);
        }
    }
    for(x = h_start_x + 2; x < h_start_x + 7; x++){
        change_ATTRIB_FOR_COVER(ECHOS_COLOR, x, char_start_y + 2);
    }

    while(j < 750000000) j++;

// draw o
    for(y = char_start_y; y < char_start_y + char_h; y++){
        for(x = o_start_x; x < o_start_x + 2; x++){
            change_ATTRIB_FOR_COVER(ECHOS_COLOR, x, y);
        }
        for(x = o_start_x + 7; x < o_start_x + 9; x++){
            change_ATTRIB_FOR_COVER(ECHOS_COLOR, x, y);
        }
    }
    for(x = o_start_x + 2; x < o_start_x + 7; x++){
        change_ATTRIB_FOR_COVER(ECHOS_COLOR, x, char_start_y);
        change_ATTRIB_FOR_COVER(ECHOS_COLOR, x, char_start_y + 4);
    }

    while(j < 800000000) j++;

// draw s
    for(x = s_start_x; x < s_start_x + char_w; x++){
        change_ATTRIB_FOR_COVER(ECHOS_COLOR, x, char_start_y);
        change_ATTRIB_FOR_COVER(ECHOS_COLOR, x, char_start_y + 2);
        change_ATTRIB_FOR_COVER(ECHOS_COLOR, x, char_start_y + 4);
    }
    change_ATTRIB_FOR_COVER(ECHOS_COLOR, s_start_x, char_start_y + 1);
    change_ATTRIB_FOR_COVER(ECHOS_COLOR, s_start_x + 1, char_start_y + 1);
    change_ATTRIB_FOR_COVER(ECHOS_COLOR, s_start_x + 7, char_start_y + 3);
    change_ATTRIB_FOR_COVER(ECHOS_COLOR, s_start_x + 8, char_start_y + 3);

    printf("Launching...                                                                  ");
    for(x = 13; x < 33; x++){
        while (i < 30000000*(x - 12)) i++;
        change_ATTRIB_FOR_COVER(0xee, x, 24);
    }
    while (i < 900000000) i++;  
    for(x = 33; x < 40; x++){
        change_ATTRIB_FOR_COVER(0xee, x, 24);
    }
    while (i < 1000000000) i++;
    for(x = 40; x < 55; x++){
        change_ATTRIB_FOR_COVER(0xee, x, 24);
    }  
    while (i < 1100000000) i++;
    for(x = 55; x < 60; x++){
        change_ATTRIB_FOR_COVER(0xee, x, 24);
    }  
    while (i < 1800000000) i++;
    for(x = 60; x < 70; x++){
        change_ATTRIB_FOR_COVER(0xee, x, 24);
    } 
    while (i < 1950000000) i++;
    for(x = 70; x < 80; x++){
        change_ATTRIB_FOR_COVER(0xee, x, 24);
    }
    while (i < 2000000000) i++;
    change_ATTRIB_FOR_COVER_BACK();
    clear();
    clear_screen();
    printf(">> Welcome to EchOS... \n");    // launching message
    printf(">> Supported by Q-T-L-D. \n");
    printf(">> All functions initialized successfully! \n");
    printf(">> Using \"Alt\"+\"F1/F2/F3\" to change terminal. Enter \"help\" for help page.\n");
    printf("\n");                                          
    execute((uint8_t*)"shell");
    /* Spin (nicely, so we don't chew up cycles) */
    asm volatile (".1: hlt; jmp .1;");
}
