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
.globl link_handler_rtc
# .globl link_handler_sysc()
# calls_table:                     # all system calls
# .long ece391_halt,ece391_execute,ece391_close,ece391_read,ece391_write,ece391_open,ece391_close,ece391_getargs,ece391_vidmap,ece391_set_handler,ece391_sigreturn


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


# link_handler_sysc:

    # PUSHAD                          # store all registers  
    # PUSHL %EFALGS

    # cmpl $1, %eax
    # jl sysc_fail
    # cmpl $10, %eax
    # jg system_call_fail

    # pushl %edx      # push the three paremeters from right to left
    # pushl %ecx
    # pushl %ebx
    # addl $-1, %eax
    # call *jump_table(, %eax, 4)       # jump to certian systerm calls
    # jmp done


# system_call_fail:
    # movl $-1,%eax
# done:   
    # POPL %EFALGS
    # POPAD
    # iret
