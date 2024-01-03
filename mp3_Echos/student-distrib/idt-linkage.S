#define ASM
# interface for all the functions inside the file
# link_handler_xx
# this set of functions works as assambly linkage
# input:none
# output:now
# side effects: none(we restore all registers)
.globl link_handler_DE
.globl link_handler_DB
.globl link_handler_NMI
.globl link_handler_BP
.globl link_handler_OF
.globl link_handler_BR
.globl link_handler_UD
.globl link_handler_NM
.globl link_handler_DF
.globl link_handler_CSO
.globl link_handler_TS
.globl link_handler_NP
.globl link_handler_SS
.globl link_handler_GP
.globl link_handler_PF
.globl link_handler_MF
.globl link_handler_AC
.globl link_handler_MC
.globl link_handler_XF
.globl link_handler_keyboard
.globl link_handler_pit
.globl link_handler_rtc
.globl link_handler_sysc
sys_call_table:
 .long 1,halt,execute,read,write,open,close,getargs,vidmap,set_handler,sigreturn


link_handler_DE:
    pushal
    pushfl
    CALL handler_DE
    popfl
    popal
    iret
link_handler_DB:
    pushal
    pushfl
    CALL handler_DB
    popfl
    popal
    iret

link_handler_NMI:
    pushal
    pushfl
    CALL handler_NMI
    popfl
    popal
    iret

link_handler_BP:
    pushal
    pushfl
    CALL handler_BP
    popfl
    popal
    iret

link_handler_OF:
    pushal
    pushfl
    CALL handler_OF
    popfl
    popal
    iret
link_handler_BR:
    pushal
    pushfl
    CALL handler_BR
    popfl
    popal
    iret
link_handler_UD:
    pushal
    pushfl
    CALL handler_UD
    popfl
    popal
    iret
link_handler_NM:
    pushal
    pushfl
    CALL handler_NM
    popfl
    popal
    iret
link_handler_DF:
    pushal
    pushfl
    CALL handler_DF
    popfl
    popal
    iret
link_handler_CSO:
    pushal
    pushfl
    CALL handler_CSO
    popfl
    popal
    iret
link_handler_TS:
    pushal
    pushfl
    CALL handler_TS
    popfl
    popal
    iret
link_handler_NP:
    pushal
    pushfl
    CALL handler_NP
    popfl
    popal
    iret
link_handler_SS:
    pushal
    pushfl
    CALL handler_SS
    popfl
    popal
    iret
link_handler_GP:
    pushal
    pushfl
    CALL handler_GP
    popfl
    popal
    iret
link_handler_PF:
    pushal
    pushfl
    CALL handler_PF
    popfl
    popal
    iret
link_handler_MF:
    pushal
    pushfl
    CALL handler_MF
    popfl
    popal
    iret
link_handler_AC:
    pushal
    pushfl
    CALL handler_AC
    popfl
    popal
    iret
link_handler_MC:
    pushal
    pushfl
    CALL handler_MC
    popfl
    popal
    iret
link_handler_XF:
    pushal
    pushfl
    CALL handler_XF
    popfl
    popal
    iret
link_handler_keyboard:
    pushal
    pushfl
    CALL handler_keyboard
    popfl
    popal
    iret
link_handler_rtc:
    pushal
    pushfl
    CALL handler_rtc
    popfl
    popal
    iret

store_eax: 
    .long 0
# link_handler_sysc
# input: parameters of system call
# output: -1 on faliure otherwise success
# side effect: execute handler
link_handler_sysc:
    pushal
    pushfl
    cmpl $1, %eax
    jl fail_sysc
    cmpl $10, %eax
    jg fail_sysc
    pushl %edx      
    pushl %ecx
    pushl %ebx
    call *sys_call_table(,%eax,4)
    movl %eax,store_eax
    add $12,%esp      
    jmp done
fail_sysc:
    movl $-1,%eax
    movl %eax,store_eax
done:   
    popfl
    popal
    movl store_eax,%eax
    iret

link_handler_pit:
    pushal
    pushfl
    CALL pit_handler
    popfl
    popal
    iret

    

