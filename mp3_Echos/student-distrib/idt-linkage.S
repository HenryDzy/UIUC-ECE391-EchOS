#define ASM
# interface for all the functions inside the file
# link_handler_xx
# this set of functions works as assambly linkage
# input:none
# output:now
# side effects: none(we restore all registers)

.globl link_handler_keyboard
.globl link_handler_mouse
.globl link_handler_pit
.globl link_handler_rtc
.globl link_handler_sysc
.globl link_handler_sig_return
.globl link_handler_sig_return_end
sys_call_table:
 .long 1,halt,execute,read,write,open,close,getargs,vidmap,set_handler,sigreturn


link_handler_rtc:
    add $-8, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    CALL handler_rtc
    CALL signal_handler
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    add $8, %esp
    iret
    
link_handler_keyboard:
    add $-8, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    CALL handler_keyboard
    CALL signal_handler
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    add $8, %esp
    iret


store_eax: 
    .long 0
sig_return_flag:
    .long 0
# link_handler_sysc
# input: parameters of system call
# output: -1 on faliure otherwise success
# side effect: execute handler
link_handler_sysc:
    add $-8, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx

    cmpl $1, %eax
    jl fail_sysc
    cmpl $10, %eax
    jg fail_sysc
    cmpl $10, %eax
    jne not_sig_return
    movl $1, sig_return_flag    
not_sig_return:
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
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    add $8, %esp
    cmpl $1, sig_return_flag
    je when_sig_return
    movl store_eax,%eax
when_sig_return:
    movl $0, sig_return_flag
    iret

link_handler_pit:
    add $-8, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    CALL pit_handler
    CALL signal_handler
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    add $8, %esp
    iret

link_handler_mouse:
    add $-8, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    CALL mouse_handler
    CALL signal_handler
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    add $8, %esp
    iret

link_handler_sig_return:
    movl    $10, %eax
    int     $0x80
    ret
link_handler_sig_return_end:
