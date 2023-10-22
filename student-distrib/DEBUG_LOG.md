# ECE391 MP3  DEBUG_LOG ------ GROUP 6

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
    
