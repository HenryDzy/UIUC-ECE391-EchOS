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
          Buddha Bless   No BUG


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


