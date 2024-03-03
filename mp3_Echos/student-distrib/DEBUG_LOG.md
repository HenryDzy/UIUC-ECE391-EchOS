# ECE391 MP3 DEBUG_LOG ------ GROUP 6


                   _ooOoo_
                  o8888888o
                  88" . "88
                  (| -_- |)
                  O\  =  /O
               ____/`---'\____
             .'  \\|     |//  `.
            /  \\|||  :  |||//  \
           /  _||||| -:- |||||-  \
           |   | \\\  -  /// |   |
           | \_|  ''\---/''  |   |
           \  .-\__  `-`  ___/-. /
         ___`. .'  /--.--\  `. . __
      ."" '<  `.___\_<|>_/___.'  >'"".
     | | :  `- \`.;`\ _ /`;.`/ - ` : | |
     \  \ `-.   \_ __\ /__ _/   .-` /  /
      `-.____`-.___\_____/___.-`____.-'
                   '=---='
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            佛祖保佑      永无BUG


## 1. Group Info
### Group Name : Segmentation Correct
### Group Number : 6
### Group Member :

| Member Name    |    NetID    |      Work Distribute         |
| -------------- | ----------- |------------------------------|
| Zixuan Qu      | zixuanq3    |           25%                |
| Ziye Deng      | ziyed2      |              25%             |                 
| Haige Liu      | haigel2     |                 25%          |             
| Dianxing Tang  | dt12        |                    25%       |             

_______________________________________________________________

## System Feature:
### 1. the terminal can change color by ctrl + 'r' 'g' 'b' (3 color background)
### 2. command history for one time, the command history will remain in buffer until user use up_arrow to obtain it, one the user obtain, it will be removed.

_______________________________________________________________

# ****** *LATEST UPDATE: CHECKPOINT 5* ******

## 2. Checkpoint 1
### 2.1. Content of Checkpoint 1
1. boot.S (modified) ----------------------> Initializing GDT
2. idt.c/idt.h (added) ----------------------> Initializing IDT
3. page.c/page.h (added) -----------------> Paging
4. i8259.c/i8259.h (added) ----------------> PIC initialization and setting
5. keyboard.c/keyboard.h (added) -------> keyboard interrupt
6. rtc.c/rtc.h (added) ----------------------> Initializing RTC and handle RTC interrupt
7. kernel.c (modified) ---------------------> Initializing the kernel
8. test.c/tests.h (modified) ----------------> Test cases for checkpoint 1

### 2.2. Bug & Debug
#### MAKE ERROR / MAKE WARNING
1. Undeclared functions
    - **Problem**: Undeclared functions in page.c
    - **Solution**: Add the function declaration in page.h
    - **TimeTaking**: 3 min to find, 1 min to fix
2. Funtion no such field
    - **Problem**: The struct and the items of page directory was not defined correctly
    - **Solution**: modified the struct of page directory in page.h
    - **TimeTaking**: 5 min to find, 3 min to fix


#### EXECUTION ERROR
1. Test machine crashed
    - **Problem**: The test machine crashed when booting the kernel, paging error occurs
    - **Solution**: Modified the setting page directory and page table, because it modified a new value but not the previous value of the global variable.
    - **TimeTaking**: 10 min to find, 20 min to fix

2. Test machine repeatly reboot
    - **Problem**: The test machine repeatly reboot when booting the kernel. paging initialization error occurs caused reboot. The helper function of paging was not used correctly, it pass the value but not pointer make it always reference to a invalid address.
    - **Solution**: Pass the argument with pointer of global variable.
    - **TimeTaking**: 60 min to find, 20 min to fix

3. Test machine reboot when testing idt
    - **Problem**: The test machine reboot when testing rtc and idt. The rtc interrupt handler and idt handler was not initialized correctly. IDT was not initialized.
    - **Solution**: initialize the IDT. 
    - **TimeTaking**: 30 min to find, 10 min to fix

4. Test machine reboot when testing rtc
    - **Problem**: The test machine reboot when testing rtc. The rtc interrupt handler was not initialized correctly. In kernel.c, the rtc was not disabled for testing, the cli was not enabled.
    - **Solution**: uncomment the sti in kernel.c
    - **TimeTaking**: 30 min to find, 10 min to fix

5. Keyboard no input response
    - **Problem**: The keyboard.c has format error in "if-else" sentence
    - **Solution**: change the format of if-else sentence
    - **TimeTaking**: 5 min to find, 10 min to fix

---------------------------------------------
# NOTICE: MAKE ERROR IGNORED FROM CHECKPOINT2

## 3. Checkpoint 2
### 3.1 Content of Checkpoint 2
1. keyboard.c/h (modified) ----------------------> Adding New scan table and command
2. lib.c/lib.h (modified) ----------------------> adding cursor control and screen modify
3. terminal.c/h (added) -----------------> for terminal initialization
4. test.c/tests.h (modified) ----------------> Test cases for checkpoint 2
5. filesystem.c/h (added) ------------------> file system driver
6. rtc.c/h (modified) --------------------> rtc can support many frequency

### 3.2. Bug & Debug
#### EXECUTION ERROR
1. Paging Fault Exception:
    - **Problem**: When test the terminal, throw a page fault exception, that's because the terminal is not locally created but only declarate a pointer
    - **Solution**: define a new terminal type struct rather than only declarate a pointer
    - **TimeTaking**: 10 min to find, 20 min to fix

2. Paging Fault Exception (without initialized):
    - **Problem**: The screen_x and screen_y was not initialize to 0 and it was not set to a correct value for the first time being used
    - **Solution**: add an initialization function for screen_x, screen_y
    - **TimeTaking**: 20 min to find, 10 min to fix    

3. Cannot update the cursor when enter a linechange
    - **Problem**: when add a new line in the terminal, the screen_y was not update by adding one
    - **Solution**: Add one line: screen_y++; after add one line to the terminal
    - **TimeTaking**: 10min to find, 5min to fix

4. no uppercase letter nor special chars
    - **Problem**: the scan table should be update to 2 kinds with condition logic
    - **Solution**: modified the logic to adapt the 4kind of special key input
    - **TimeTaking**: 10min to find, 30min to fix

5. delete will make an space between cursor and last char
    - **Problem**: the delete didn't move the screen_x left after put a ' '
    - **Solution**: modified the logic of write buffer by minus screen_x when input is for delete
    - **TimeTaking**: 10min to find, 5min to fix

6. cannot read the filename to test buffer
    - **Problem**: the data type are not consistent, one is int32_t while the other is int8_t, making only 0s appear in test buf.
    - **Solution**: change the data type of test buffer to be int32_t
    - **TimeTaking** 10min to find, 10min to fix

## 4. Checkpoint3 
### 4.1 Content of Checkpoint 3
1. keyboard.c/h (modified) ----------------------> modify buffer logic
2. terminal.c/h (modified) -----------------> support pcb and fd
3. test.c (modified) ----------------> Test cases for checkpoint 3
4. filesystem.c/h (added) ------------------> support pcb and fd
5. system_calls.c/h (added) -------------------> handle systemcalls including execute, halt, owrc
6. idt.c/h (modified) -------------------> modified for excep handling
7. idt-linkage.S (modified) -------------------> add a syscall table and handler
8. kernel.c (modified) -------------------> add a kernel shell to execute 
9. page.c/h (modified) -------------------> support a user program paging

### 4.2 Bug & Debug
#### EXECUTION ERROR
1. Paging Fault Exception:
    - **Problem**: When initialize the fd for stdin and stdout terminal, the page fault happened, because the fd_ops_table was only a pointer to null, it's not initialized. 
    - **Solution**: change the fd_ops_table_ptr type from fd_ops_table* to fd_ops_table inside file_descriptor_t, so that it contain a full struct of fd_ops_table but not a pointer tonull
    - **TimeTaking**: 10 min to find, 20 min to fix

2. the shell will not execute any command
    - **Problem**: the keyboard clear the terminal_buffer every time we enter '\n'
    - **Solution**: keep the terminal buffer in the '\n' but clean in every time enter ternimal_read
    - **TimeTaking**: 20 min to find, 20 min to fix

3. always print a "program terminated abnormally"
    - **Problem**: In systemcall linkage, the eax register always been pushed and poped, (we used pushal) making the "supposed return value" of 0 or -1 becaming the number of systemcalls
    - **Solution**: save the eax from syscall in another variable and move it to eax agian after popal
    - **TimeTaking**: 20 min to find, 10 min to fix

4. a single word could run the execute, for example, "l can run ls" and "c can run counter"
    - **Problem**: In read_dentry_by_name, the strcmp will accept the len of the smaller len's but not the longer filename, make ls=l 
    - **Solution**: add judgment logic for using the correct length at strcmp make it a throughout compare
    - **TimeTaking**: 15 min to find, 20 min to fix

5. In ls, read directory fail for ls execution
    - **Problem**: the dir_read will return -1 because of the index(32) was larger than 17
    - **Solution**: modify the logic of dir read to make it correspond to the ls function
    - **TimeTaking**: 30 min to find, 60 min to fix

6. halt will result in page fault
    - **Problem**: the child process obtain the parent pcb's esp and ebp but not its owns, making it jump to strange place
    - **Solution**: change the parent_pcb_ptr->esp to pcb_ptr->esp
    - **TimeTaking**: 1.5h to find, 30 min to fix

7. In ls, it will print a "ghost" before "ls"
    - **Problem**: the terminal write exceed the length of nbytes and print things out of 32, making unexpected things happened
    - **Solution**: add logical limit to terminal write, making it write the number of shortest limit
    - **TimeTaking**: 1h to find, 20 min to fix

8. the exception occur with halt but not stop execution
    - **Problem**: the halt got wrong status input, should be 1 for exception and 0 for normal
    - **Solution**: change the halt(0) in exception idts to halt(1)
    - **TimeTaking**: 10 min to find, 5 min to fix

9. if close base shell, it will infinitly generate protection exception
    - **Problem**: the base shell should not be closed and also cannot return to kernel, so we have to open other shell to avoid crash
    - **Solution**: remove current shell's pcb as it should not appear but have to be in context and reopen a new shell as base shell
    - **TimeTaking**: 20 min to find, 10 min to fix


## 5. Checkpoint4
### 5.1 Content of Checkpoint 4 
1. keyboard.c/h (modified) ----------------------> modify keyboard buffer logic
2. terminal.c/h (modified) -----------------> support pcb and fd, modify terminal buffer logic
3. filesystem.c/h (added) ------------------> support pcb and fd, modified the read data part
4. system_calls.c/h (added) -------------------> modifed logic and add vidmap and getargs
5. kernel.c (modified) -------------------> add a kernel shell to execute and something
6. page.c/h (modified) ------------------->  a user program paging and new video page
7. lib.c (modifed) ---------------------> modifed putc to print "null"

### 5.2 Bug & Debug
#### EXECUTION ERROR
1. paging exception for vidmap
    - **Problem**: The page fault happened when use set_user_vidmap because when set the page table for video memory didn't set mask bit for 8MB>>12
    - **Solution**: add a &0x3FF after indexing 8MB>>12
    - **TimeTaking**: 30 min to find 30 min to find

2. only support 2 shell and generate PF for the third execution.
    - **Problem**: The PCB* pointer type was wrongly used and it made the the offset of pcb wrongly at unused virtual memory 
    - **Solution**: replace the pcb_ptr=(PCB_t*)KENEL_BOT - (i+1) * BLOCK to (PCB_t*)(KENEL_BOT-(i+1) * BLOCK)
    - **TimeTaking**: 20 min to find, 1 h to modifed because many documents are effected

3. cat didn't functioned as "Worked Demo", missing spaces for cat executables
    - **Problem**: In our modified putc, we IGNORED if the char is "\0" but it will not move the cursor so it will not have same content with the workding Demo
    - **Solution**: delete the if(c = "\0") return -1;
    - **TimeTaking**: 1h to find, 1h to fix

4. grep can only find one file then go PF
    - **Problem**: the offset of endblock is calculated wrongly, simply speaking, (a+b)%c is not a%c+b%c but (a%c + b%c)%c
    - **Solution**: modifed the offset calculate logic 
    - **TimeTaking**: 4h to find & fixed
    - **Thoughts**: It is a very simple but stupid mistake but took us a lot of time, we gdb multiple(even thousands of breakpoints were made) times, and we even found that the "fd" were modifed to 0 while in do_one_file function which is impossible, that's large probably becuase of the illegal access because the wrong offset.

- other modification in mp3-4 to beautified the OS:
    - 1. clear the screen content after initialization
    - 2. add a starting header for launching
    - 3. change the theme color of shells
    - 4. ctrl+l can retain the ">391OS "
    - 5. add a new line for every execution.


## 6. Checkpoint5 & Extra Credit
### 6.1 Content of Checkpoint 5
1. scheduling.c/h ------------------> support scheduling and pit
2. system_calls.c/h ---------------------> support scheduling modification
3. keyboard.c/h -----------------------> support terminal change and extra
4. terminal.c/h -----------------------> support terminal change and extra
5. lib.c ---------------------> support scheduling and extra
6. rtc.c/h -----------------------> support scheduling
7. page.c/h ------------------> support scheduling
8. idt-relate-files -----------------> support pit 

### 6.2 Bug & Debug
#### EXECUTION ERROR
1. cannot change terminal by alt+fn 
    - **Problem**: the fn was not used at all because it was ignored by previous logic in keyboard
    - **Solution**: change the keyboard logic by writing fn judgement before ignored garbage
    - **TimeTaking**: 30 min to find 30 min to find

2. cannot execute after boot/ page fault after the first shell/ reboot continously
    - **Problem**: the scheduling was wrong, didn't store the esp and ebp of current pcb, so it cannot go back to execute. And the asm in c was wrong, "=r" is not "r"
    - **Solution**: change the asm "=r" to be "r" and save the current pcb's esp and ebp.
    - **TimeTaking**: 1 h to find, 1 h to fix

3. ls was not able to execute sometime
    - **Problem**: in read_term, we didn't used current term pointer but used a fixed different pointer
    - **Solution**: change the target terminal to be current terminal using cur_term variable.
    - **TimeTaking**: 30 min to find 30 min to find.

4. the pingpong was not able to execute after print some chars
    - **Problem**: the interrupt was disabled and the rtc interrupt cannot generate
    - **Solution**: add a sti() in read_rtc
    - **TimeTaking**: 30 min to find 30 min to find 

5. executing pingpong, cannot update video in vidmap
    - **Problem**: the vidmap was paged wrongly
    - **Solution**: modifed the addr of pte to be (video>>12)+2+2*term_num
    - **TimeTaking**: 30 min to find 30 min to find

6. when using clear screen (ctrl+l), the 391OS> was printed wrongly
    - **Problem**: the ctrl clean the screen but when print the 391OS> used putc_by_program which is not the current visible terminal 
    - **Solution**: change the putc_by_program to putc_by_keyboard, meaning the content is updated in visible terminal but not cur_term
    - **TimeTaking**: 30 min to find 30 min to find

