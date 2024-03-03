#include "system_calls.h"
#include "types.h"
#include "lib.h"
#include "x86_desc.h"
#include "page.h"
#include "terminal.h"
#include "rtc.h"
#include "filesystem.h"
#include "scheduling.h"
#include "pcspeaker.h"
int32_t digits=0;
int32_t exeret=0;
file_descriptor_t fd_array[MAX_FD]; // array of file descriptors
uint8_t PCBs=0x00;  //from highest bit stands for PCB0,PCB1...PCB5,we do not allow more than six
uint32_t terminal_counter=0;

// ---------------------in the following context, 0x80 means bitmask for the first pcb----------------------------- //

static file_ops_table_t rtc_ops_table = {rtc_open, rtc_close, rtc_read, rtc_write};
static file_ops_table_t dir_ops_table = {dir_open, dir_close, dir_read, dir_write};
static file_ops_table_t file_ops_table = {file_open, file_close, file_read, file_write};
static file_ops_table_t term_ops_table = {open_term, close_term, read_term, write_term};

//helper function for help page
void print_helper_page(){
    printf("\nHelper Page for EchOS by Group 06\n");
    printf("Author: Group 06\n");
    printf("(C)Copyright 2023-2024 Group 06 && ECE391. All Rights Reserved.)\n\n");
    printf("Important Commands:\n");
    printf("status:                 show current status of the OS\n");
    printf("Useful Key-Combinations:\n");
    printf("Ctrl+L:                 clear the screen\n");
    printf("Ctrl+C:                 kill the current process\n");
    printf("Ctrl+R/G/B/W/D:         change current terminal color\n");
    printf("Alt+F1/F2/F3:           change terminals\n");
    printf("Tab(for executables):   auto-complete\n");
    printf("Up/Down(for history):   terminal input history\n");
    printf("speak+number in 1 to 999:  play sound at the freqency\n");
    printf("stop:  stop playing sound\n");
    printf("mouse left click:  send USR1 signal\n");
    printf("mouse right click:  send USR2 signal\n");
    printf("mouse middle click:  switch to next terminal\n\n");
    printf("didzero:   trigger the DIVZERO signal");
    printf("register:   show information of registers");
}

// description: create PCB structrue
// input: none
// output: uint8 i
// side effects: change PCBs and the kenel space
uint8_t create_PCB(int8_t* argument,uint8_t length){
    int i, k;
    uint8_t j;
    PCB_t* parent_pcb;
    for(i=0;i<MAX_PCB;i++){
        if(0!=(PCBs & (0x80>>i))){  //from left to right check which PCB to be created
            continue;
        }
        break;
    }
    if(i>5){
        printf("Warning: Executions maximum! Please Remove a SHELL!\n");
        return 10;
    } //we only need 2 pcb for now so ignore i>=2 , 10 is an unused number for following test
    PCBs = PCBs |(0x80>>i); //update the bitmap, 0x80 is the bitmask for PCB
    PCB_t* pcb=(PCB_t*)(KENEL_BOT-(i+1)*BLOCK);   //8MB - 8KB*(i+1)
    if(argument!=NULL){
        for(j=0;j<length;j++){
            pcb->arguments[j]=argument[j];
        }
    }else{
        for(j=0;j<32;j++){              // 32 is the maximum length of argument
            pcb->arguments[j]='\0';
        }
    }
    pcb->pid=i;
    pcb->arglength=length;
    pcb->pcb_esp=KENEL_BOT-i*BLOCK;     //this is the initial esp, which may change as time goes by
    // initialize signal
    for(k=0;k<SIGNAL_NUM;k++){
        pcb->signal_status[k]=OFF; //set all signal status to OFF
    }
    for (k=0;k<SIGNAL_NUM;k++)
    {
        if(k<=2){
            pcb->handler[k]=kill_task;  //set the handler for signal 0 and 1
        }
        else{
            pcb->handler[k]=signal_ignore;  //set the handler for signal 2,3,4
        }
    }
    pcb->handler[4]= left_key_mouse;
    pcb->handler[5]= right_key_mouse;
    if(i==0 || i==1 || i==2){
        pcb->parent_pcb=NULL;
    }else{
        parent_pcb=get_current_pcb();
        pcb->parent_pcb=parent_pcb;
    }
    pcb->file_array[0].inode=0; //stdin
    pcb->file_array[0].file_position=0;
    pcb->file_array[0].flags=1;
    pcb->file_array[0].file_ops_table_ptr = &term_ops_table;
    pcb->file_array[1].inode=0; //stdout
    pcb->file_array[1].file_position=0;
    pcb->file_array[1].flags=1;
    pcb->file_array[1].file_ops_table_ptr = &term_ops_table;
    pcb->file_array[2].flags=0;
    pcb->file_array[3].flags=0;
    pcb->file_array[4].flags=0;
    pcb->file_array[5].flags=0;
    pcb->file_array[6].flags=0;
    pcb->file_array[7].flags=0;
    return i;
}
// description: remove the PCB structure
// input: pid
// output: uint8 none
// side effects: change the bitmap
void remove_PCB(uint8_t pid){
    int32_t i;
    PCB_t* pcb_ptr;
    pcb_ptr=(PCB_t*)(KENEL_BOT-(pid+1)*BLOCK);
    PCBs &= ~(0x80>>pid); // set the current pcb to not in use, 0x80 is the bitmask for PCB
    for(i=0;i<32;i++){          // 32 is the maximum length of argument
        pcb_ptr->arguments[i]='\0';
    }
}
// description: update the esp of the pcb
// input: pointer to the pcb, the esp to be updated
// output: none
// side effects: none
void store_current_esp(PCB_t* pcb,int32_t esp){
    pcb->pcb_esp=esp;
}
// description: update the esp of the pcb
// input: pointer to the pcb, the esp to be updated
// output: none
// side effects: none
void store_current_ebp(PCB_t* pcb,int32_t ebp){
    pcb->pcb_ebp=ebp;
}

// description: halt a program
// input: status
// output: 0 on success,-1 on failure,256 on execption
// side effects: none
int32_t
halt(uint8_t status)
{   //cli();
    terminal_t* term=get_term(cur_term);
    exeret=(int32_t)status;
    if(status == 1) exeret=256; // return 256 if halt was called by exception
    int32_t i;
    PCB_t* pcb_ptr;
    PCB_t* parent_pcb;
    pcb_ptr=get_current_pcb();
    parent_pcb=pcb_ptr->parent_pcb;
    if(pcb_ptr->pid==0 || pcb_ptr->pid==1 || pcb_ptr->pid== 2){
        printf("Can't exit base shell.\n");
        remove_PCB(pcb_ptr->pid);
        execute((uint8_t*)"shell");
    }

    for(i=0; i<MAX_FD; i++){
        close(i); // close all files
    }
    for(i=0;i<100;i++){
        term->curprogram[i]='\0';
    }
    term->curprogram[0]='s';
    term->curprogram[1]='h';
    term->curprogram[2]='e';
    term->curprogram[3]='l';
    term->curprogram[4]='l';
    user_paging(parent_pcb->pid);

    remove_PCB(pcb_ptr->pid);   //remove the current PCB
    schedule_pcb[cur_term]=parent_pcb; //update the process of scheduling
    tss.ss0=KERNEL_DS;      // set the TSS for privilege switch
    tss.esp0=KENEL_BOT-BLOCK*(parent_pcb->pid)-4; // set esp0 to parent pcb's esp

    asm volatile("movl %0, %%eax \n\
                  movl %1, %%ebp \n\
                  movl %2,%%esp \n"
                 ::"r" (exeret), // restore esp
                  "r" (pcb_ptr->pcb_ebp), // restore eax
                  "r" (pcb_ptr->pcb_esp)
                 : "eax","ebp","esp");
    sti();
    asm volatile("jmp haltret");
    return 0;                
}

// grep 8048094

// description: execute an command
// input: command ,pointer to the command
// output: 0 on success,-1 on failure
// side effects: none
int32_t execute(const uint8_t* command){
    //cli();
    terminal_t* term=get_term(cur_term);
    uint8_t prog[32]={'\0'}; //maximum length buf, 32 is the maximum length of argument
    uint8_t i=0;
    uint8_t j=0;
    int a, b;
    dentry_t pro_file; // dentry for file
    uint8_t buffer[4]; //check first ELF, 4 is the length of ELF
    int8_t argument[32]={'\0'};     // 32 is the maximum length of argument
    uint32_t first_instruction;
    uint8_t pid; // local var for storing
    uint32_t pcbesp;
    uint32_t pcbebp;
    uint8_t arg=0;
    uint16_t arglen=0;
    uint32_t p=5;
    uint32_t freq=0;
    uint32_t status_ebp;
    uint32_t status_esp;
    uint32_t status_eax;
    uint32_t status_ebx;
    uint32_t status_ecx;
    uint32_t status_esi;
    uint32_t status_edi;
    if (command[0]==NULL){ // invalid command
        return 0;}
    while(i<strlen((int8_t*)command) && i<32){
        if(command[i]==' ') {   //space-separated
            i++;
            break;
        }
        prog[i]=command[i];
        i++;
    }
    while (i<strlen((int8_t*)command))
    {   
        if(command[i]=='\0') break;
        if(command[i]==' '){
            i++;
            continue;
        }
        arg=1;
        argument[j]=command[i];
        i++;
        j++;
        arglen++;
    }
    asm volatile(
    "movl %%ebp, %0\n\
    movl %%esp, %1 \n\
    movl %%eax, %2 \n\
    movl %%ebx, %3 \n\
    movl %%ecx, %4 \n\
    movl %%esi, %5 \n\
    movl %%edi, %6 \n" : "=m"(status_ebp),"=m"(status_esp),"=m"(status_eax),"=m"(status_ebx),"=m"(status_ecx),"=m"(status_esi),"=m"(status_edi) : : "memory" );

    
    
    if((prog[0]=='r') && (prog[1]=='e') && (prog[2]=='g') && ( prog[3]=='i')&& ( prog[4]=='s')&& ( prog[5]=='t')&& ( prog[6]=='e')&& ( prog[7]=='r')){
        printf("EBP: 0x%x\n", status_ebp);
        printf("ESP: 0x%x\n", status_esp);
        printf("EAX: 0x%x\n", status_eax);
        printf("EBX: 0x%x\n", status_ebx);
        printf("ECX: 0x%x\n", status_ecx);
        printf("ESI: 0x%x\n", status_esi);
        printf("EDI: 0x%x\n", status_edi);
        term->curprogram[0]='s';
        term->curprogram[1]='h';
        term->curprogram[2]='e';
        term->curprogram[3]='l';
        term->curprogram[4]='l';
        return 0;
    }
    if((prog[0]=='h') && (prog[1]=='e') && (prog[2]=='l') && ( prog[3]=='p')){
        print_helper_page();
        term->curprogram[0]='s';
        term->curprogram[1]='h';
        term->curprogram[2]='e';
        term->curprogram[3]='l';
        term->curprogram[4]='l';
        return 0;
    }
    if((prog[0]=='s') && (prog[1]=='t') && (prog[2]=='a') && ( prog[3]=='t') && ( prog[4]=='u') && ( prog[5]=='s')){
        printf(" current time since booted:%u \n current visible terminal: %u \n", seconds,visible_term,get_term(0)->freq,get_term(1)->freq,get_term(2)->freq);
        printf(" program in terminal 0: %s \n program in terminal 1: %s \n program in terminal 2: %s \n",get_term(0)->curprogram,get_term(1)->curprogram,get_term(2)->curprogram);
        term->curprogram[0]='s';
        term->curprogram[1]='h';
        term->curprogram[2]='e';
        term->curprogram[3]='l';
        term->curprogram[4]='l';
        return 0;
    }
    if((prog[0]=='s') && (prog[1]=='p') && (prog[2]=='e')&& (prog[3]=='a')&& (prog[4]=='k')){
        digits=0;
        while(prog[p]!='\0'){
            digits++;
            p++;
        }
        p=5;
        switch (digits)
        {
        case 1: 
            freq=0;
            freq=(uint32_t)(prog[5]-'0');
            play_sound(freq);
            return 0;
        case 2: 
            freq=0;
            freq=((uint32_t)(prog[5]-'0')*10 +(uint8_t) (prog[6]-'0'));
            play_sound(freq);
            return 0;
        case 3: 
            freq=0;
            freq=((uint32_t)(prog[5]-'0')*100 + (uint8_t)(prog[6]-'0')*10 +(uint8_t)(prog[7]-'0') );
            play_sound(freq);
            return 0;
        }
    }
    if((prog[0]=='d' )&& (prog[1]=='i') & (prog[2]=='d' )&& (prog[3]=='z' ) && (prog[4]=='e') && (prog[5]=='r') && (prog[6]=='o')  ){
        a=5;
        b=0;
        return a/b;
    }
    if((prog[0]=='s') && (prog[1]=='t') && (prog[2]=='o')&& (prog[3]=='p')){
        nosound();
        return 0;
    }
    for(i=0;i<100;i++){
        term->curprogram[i]='\0';
    }
    i=0;
    while(prog[i]!='\0'){
        term->curprogram[i]=prog[i];
        i++;
    }
    if(read_dentry_by_name(prog,&pro_file)==-1){ //no found filename
        term->curprogram[0]='s';
        term->curprogram[1]='h';
        term->curprogram[2]='e';
        term->curprogram[3]='l';
        term->curprogram[4]='l';
        return -1;
    }
    read_data(pro_file.inode_index,0,buffer,4); // 4 means read the first 4 bytes
    //check whether this is Executable and Linkable Format
    if(buffer[0]!=127)return -1;    // 127 is first byte of sign of ELF
    if(buffer[1]!=69)return -1;     // 69 is second byte of sign of ELF, 'E'
    if(buffer[2]!=76)return -1;     // 76 is third byte of sign of ELF, 'L'
    if(buffer[3]!=70)return -1;     // 70 is last byte of sign of ELF, 'F'

    if(arg==0) 
    {
    pid = create_PCB(NULL,0);
    }   //create the PCB for this process}
    else{
    pid = create_PCB(argument,arglen);
    }   //create the PCB for this process}

    if(pid==10)return -1;   //can not create a pcb, magic number we use 10 to indicate can not create a pcb since -1 is not allowed
    schedule_pcb[cur_term]=(PCB_t*)(KENEL_BOT-(pid+1)*BLOCK);   //update the process of scheduling
    // corresponding to previous
    user_paging(pid); // set paging for program image

    read_data(pro_file.inode_index,0,(uint8_t*)0x08048000,get_num_bytes(pro_file.inode_index));    //load the program, 0x8048000
                                            //0x8048000 means the first place for user prog
    read_data(pro_file.inode_index,24,(uint8_t*)&first_instruction,4); //get the start instruction
                                //24 means start to read from 24 and rhe 4 means get the starting instruction of prog

   //get the current esp
    tss.ss0=KERNEL_DS;      // set the TSS for privilege switch
    tss.esp0=KENEL_BOT-BLOCK*pid-4;

    asm volatile (    
		"movl %%esp, %0\n\
         movl %%ebp, %1 \n "
		:"=r"(pcbesp),"=r"(pcbebp)
		:
		: "esp","ebp"
	);
    store_current_esp((PCB_t*)(KENEL_BOT-BLOCK*(pid+1)),pcbesp); //update the current PCB's esp field
    store_current_ebp((PCB_t*)(KENEL_BOT-BLOCK*(pid+1)),pcbebp);
    // set up the EIP,CS,EFLAGS,ESP,SS
    // USER_ESP: 0x8400000 - 4 means the 128MB - 4B, the ESP of the user stack, 
    // -4 as explained in ECE391 piazza
    asm volatile("pushl %0 \n\
                  pushl %1 \n\
                  pushfl   \n\
                  pushl %2 \n\
                  pushl %3 \n\
                            "
                :: "r" (USER_DS), \
                  "r" (USER_ESP-4), \
                  "r" (USER_CS), \
                  "r" (first_instruction)
                : "memory");
    sti();
    asm volatile("iret");   //execute iret
    asm volatile("haltret:");
    
    printf("\n");
    return exeret;
}

// description: opens the specified file
// input: filename - name of file to open
// output: fd - file descriptor of opened file
// side effects: opens the specified file
int32_t open(const uint8_t* filename){
    PCB_t* pcb_ptr;
    dentry_t dentry;
    int32_t fid = 2; // file descriptor index starts at 2 because 0 and 1 are reserved for stdin and stdout
    pcb_ptr=get_current_pcb();
    if(filename == NULL) return -1; // invalid filename
    if(strlen((int8_t*)filename) == 0) return -1; // invalid filename
    if(read_dentry_by_name(filename, &dentry) == -1) return -1; // file not found
    
    while(pcb_ptr->file_array[fid].flags == 1){ //try to find an available fd
        fid++;
        if(fid == MAX_FD) return -1; // no available fd
    }
    // initialize file descriptor for the file to be opened based on file type
    if(dentry.file_type == 0){
        pcb_ptr->file_array[fid].file_ops_table_ptr = &rtc_ops_table;
        pcb_ptr->file_array[fid].inode = dentry.inode_index;
    }
    if(dentry.file_type == 1){
        pcb_ptr->file_array[fid].file_ops_table_ptr = &dir_ops_table;
        pcb_ptr->file_array[fid].inode = dentry.inode_index;
    }
    if(dentry.file_type == 2){
        pcb_ptr->file_array[fid].file_ops_table_ptr = &file_ops_table;
        pcb_ptr->file_array[fid].inode = dentry.inode_index;
    }

    pcb_ptr->file_array[fid].file_position = 0; // initial file position
    pcb_ptr->file_array[fid].flags = 1; // 1 means fd is used
    
    pcb_ptr->file_array[fid].file_ops_table_ptr->open(filename); // call open function of file operations table
    return fid; // return allocated fid (fd) of opened file
}

// description: closes the specified file descriptor
// input: fd - file descriptor to close
// output: 0 on success, -1 on failure
// side effects: closes the specified file descriptor
int32_t close(int32_t fd){  
    PCB_t* pcb_ptr;  
    pcb_ptr=get_current_pcb();
    if(fd < 0 || fd >= MAX_FD) return -1; // invalid fd
    if(fd == 1 || fd == 0) return -1; // cannot close stdin or stdout
    if(pcb_ptr->file_array[fd].flags == 0) return -1; // fd is not in use
    pcb_ptr->file_array[fd].file_ops_table_ptr->close(fd); // call close function of file operations table
    pcb_ptr->file_array[fd].flags = 0; // set fd to not in use
    return 0; // return success
}

// description: reads nbytes from the specified file descriptor into buf 
// input: fd - file descriptor to read from, buf - buffer to read into, nbytes - number of bytes to read
// output: number of bytes read on success, -1 on failure
// side effects: reads nbytes from the specified file descriptor into buf
int32_t read(int32_t fd, void* buf, int32_t nbytes){
    PCB_t* pcb_ptr;  
    int32_t byteread;
    pcb_ptr=get_current_pcb();
    if(fd < 0 || fd >= MAX_FD) return -1; // invalid fd
    if(buf == NULL) return -1; // invalid buf
    if(nbytes < 0) return -1; // invalid nbytes
    if(pcb_ptr->file_array[fd].flags == 0) return -1; // fd is not in use
    byteread = pcb_ptr->file_array[fd].file_ops_table_ptr->read(fd, buf, nbytes); // call read function of file operations table
    pcb_ptr->file_array[fd].file_position += byteread; // update file position
    return byteread; // return number of bytes read
}

// description: writes nbytes from buf to the specified file descriptor
// input: fd - file descriptor to write to, buf - buffer to write from, nbytes - number of bytes to write
// output: number of bytes written on success, -1 on failure
// side effects: writes nbytes from buf to the specified file descriptor
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    int32_t bytewritten;
    PCB_t* pcb_ptr;
    pcb_ptr=get_current_pcb();
    if(fd < 0 || fd >= MAX_FD) return -1; // invalid fd
    if(pcb_ptr->file_array[fd].flags == 0) return -1; // fd is not in use
    //if((pcb_ptr->file_array[fd].file_ops_table_ptr) == file_ops_table) return -1; // cannot write to a file
    if(buf == NULL || nbytes < 0) return -1; // invalid buf or nbytes
    bytewritten = pcb_ptr->file_array[fd].file_ops_table_ptr->write(fd, buf, nbytes); // call write function of file operations table
    return bytewritten; // return number of bytes written
}
// description: get the argument
// input: buff to be filled, nbytes-number of bytes
// output: 0 on success, -1 on no argument
// side effects: none
int32_t getargs(uint8_t* buf, int32_t nbytes){
    PCB_t* pcb_ptr;
    int32_t j;
    if(buf==NULL)return -1;
    if(nbytes<0)return -1;
    pcb_ptr=get_current_pcb();
    if(pcb_ptr->arglength==0)return -1;         //no argument   
    if(nbytes<(int32_t)pcb_ptr->arglength){     //nbytes is smaller than the length of argument
    memcpy(buf,pcb_ptr->arguments,nbytes);}
    else{                                       //nbytes is larger than the length of argument
    memcpy(buf,pcb_ptr->arguments,pcb_ptr->arglength);    
    }
    for(j=(int32_t)pcb_ptr->arglength;j<nbytes;j++){
        buf[j]='\0';
    }
    return 0;
}
// description: maps the video memory
// input: pointer to the start of the screen start
// output: 0 on success, -1 on no argument
// side effects: none
int32_t vidmap(uint8_t** screen_start){
    if(screen_start==NULL) return -1; // invalid screen_start
    if((screen_start < (uint8_t**)PROGRAM_IMAGE) || (screen_start >= (uint8_t**)PROGRAM_VIDEO)){
        return -1; // screen start out of user space in virtual, invalid
    }
    set_user_vidmap_paging(screen_start);
    return 0;
}

// side effects: none
int32_t set_handler(int32_t signum, void* handler_address){
    PCB_t* cur_pcb=get_current_pcb();
    if(signum>=0&&signum<=5){ //check whether the signum is valid
        if(handler_address!=NULL){ //if handler_address is not NULL, set the handler
            cur_pcb->handler[signum]=handler_address;
        }
        else{   //if handler_address is NULL, set the default handler
            if(signum==0||signum==1||signum==2){
                cur_pcb->handler[signum]=kill_task;
            }
            else if (signum==3){
                cur_pcb->handler[signum]=signal_ignore;
            }
            else if(signum==4){
                cur_pcb->handler[4]=left_key_mouse;
            }
            else if(signum==5){
                cur_pcb->handler[5]=right_key_mouse;
            }
        }
    }
    return 0;
}

// side effects: none
int32_t sigreturn(void){
    uint32_t uebp;
    asm volatile(
        "movl %%ebp, %0" //get the saved ebp
        :"=r"(uebp)
        ::"memory"
    );
    hardware_context_t* new_hwcontext_ptr=(hardware_context_t*)(uebp+20);
    uint32_t uesp = new_hwcontext_ptr->esp;
    memcpy(new_hwcontext_ptr, (hardware_context_t*)(uesp+4), sizeof(hardware_context_t));
    return 1;
}


// description: get pointer to current pcb
// input: none
// output: pointer to pcb
// side effects: none
PCB_t* get_current_pcb(){
    uint32_t pcbesp;
    asm volatile(    
		"movl %%esp, %0\n\
        "
		:"=r"(pcbesp):: "memory"
    );
    return (PCB_t*) ((0xffffe000) & (pcbesp));   // it is 0x2000 aligned, so they are unused
}
