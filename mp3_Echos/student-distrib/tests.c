#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "page.h"
#include "terminal.h"
#include "rtc.h"
#include "filesystem.h"

#define PASS 1
#define FAIL 0
#define buffer_size 128

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


// /* Checkpoint 1 tests */

// /* IDT Test - Example
//  * 
//  * Asserts that first 10 IDT entries are not NULL
//  * Inputs: None
//  * Outputs: PASS/FAIL
//  * Side Effects: None
//  * Coverage: Load IDT, IDT definition
//  * Files: x86_desc.h/S
//  */
// int idt_test(){
// 	TEST_HEADER;

// 	int i;
// 	int result = PASS;
// 	for (i = 0; i < 10; ++i){
// 		if ((idt[i].offset_15_00 == NULL) && 
// 			(idt[i].offset_31_16 == NULL)){
// 			assertion_failure();
// 			result = FAIL;
// 		}
// 	}

// 	return result;
// }

// /*
// * handler_DE test
// * 
// * Test the functionality of divide_zero exception handler
// * Inputs: None
// * Outputs: PASS/FAIL
// * Side Effects: None
// * Coverage: divide_zero exception handler
// * Files: idt.c/h
// */
// int handler_DE_test(){
// 	TEST_HEADER;
// 	int a=1;
// 	int b=0;
// 	int c=a/b;		//divide by zero
// 	c++;
// 	return FAIL;	//should not reach here if exceptin happens if exceptin happens
// }

// /*
// * handler_UD test
// * test the functionality of undefined opcode exception handler
// * Inputs: None
// * Outputs: PASS/FAIL
// * Side Effects: None
// * Coverage: undefined opcode exception handler
// * Files: idt.c/h
// */
// int handler_UD_test(){
// 	TEST_HEADER;
// 	asm volatile("ud2"); //undefined opcode
// 	return FAIL; //should not reach here if exceptin happens if exceptin happens
// }


// // test cases for paging
// // input: test_mode, 0 for normal test, 1 for null pointer test, 2 for far pointer test
// //        3,4,5,6 for boundary test of video memory and kernel memory
// // output: PASS/FAIL
// // side effect: print out the result of the test
// int paging_test(int test_mode){
// 	TEST_HEADER;
// 	uint32_t counter;
// 	uint8_t* video_mem = (uint8_t*)VIDEO_PAGE_BEGIN; // 0xB8000 is the beginning of video memory in virtual memory
// 	uint8_t* kernel_mem = (uint8_t*)KERNEL_PAGE_BEGIN; // 0x400000 is the beginning of kernel memory in virtual memory
// 	uint32_t video_mem_len = 0x1000; // 4KB
// 	uint32_t kernel_mem_len = 0x400000; // 4MB
// 	uint8_t thing;
// 	uint8_t* nullptr = NULL;
// 	uint8_t* far_ptr = (uint8_t*)0x888888; // far beyond the range of setting page
// 	switch(test_mode){
// 		case 0:
// 			printf("Paging video memory, should print last hex num.\n");
// 			for(counter = 0; counter < video_mem_len; counter+=0x80){ // test every 128B
// 				thing = *(video_mem + counter);
// 			}
// 			printf("MEM ADDR FROM %d AND LAST %x: %x\n", counter, (video_mem + counter - 1), thing); // print the address and the value of last time

// 			printf("Paging kernel memory, should print lst hex num.\n");
// 			for(counter = 0; counter < kernel_mem_len; counter+=0x20000){ // test every 128KB
// 				thing = *(kernel_mem + counter);
// 			}
// 			printf("MEM ADDR FROM %d AND LAST %x: %x\n", counter, (kernel_mem + counter - 1), thing); // print the address and the value of last time
// 			break;
// 		case 1:
// 			printf("Null pointer, should cause page fault and print exception.\n");
// 			thing = *nullptr; // should cause a page fault because of accessing null and print out the error message
// 			return FAIL;
// 		case 2:
// 			printf("Far pointer, should cause page fault and print exception.\n");
// 			thing = *far_ptr; // should cause a page fault because of going beyond and print out the error message
// 			return FAIL;
// 		case 3:
// 			printf("Test B8000-1, should cause page fault:");
// 			thing = *(video_mem - 1); // test left boundary of video mem
// 			return FAIL;
// 		case 4:
// 			printf("Test B9000, should cause page fault:");
// 			thing = *(video_mem + video_mem_len); // test right boundary of video mem
// 			return FAIL;
// 		case 5:
// 			printf("Test 0x400000-1, should cause page fault:");
// 			thing = *(kernel_mem - 1); // test left boundary of kernel mem
// 			return FAIL;
// 		case 6:
// 			printf("Test 0x800000, should cause page fault:");
// 			thing = *(kernel_mem + kernel_mem_len); //test right boundary of kernel mem
// 			return FAIL;
// 	}
// 	return PASS;
// }

// // add more tests here

// /* Checkpoint 2 tests */
// // test cases for terminal
// // input: None
// // output: PASS/FAIL
// // side effect: print out the result of the test
// // pass if the terminal can read and write correctly and return the correct number of bytes
// int test_terminal_rw(void){
// 	TEST_HEADER;
// 	int32_t i;
// 	uint8_t test_buf[128];
// 	int32_t test_read;
// 	int32_t test_write;
// 	int sign = 1;
// 	printf("---------------Test for read and write----------------\n");
// 	while(sign){
// 		printf("Please enter a string: ");
// 		test_read = test_read_term(0, test_buf, buffer_size);
// 		printf("Output should be the same: ");
// 		test_write = test_write_term(0, test_buf, buffer_size);
// 		printf("test_read number: %d\n", test_read);
// 		printf("test_write number: %d\n", test_write);
// 		printf("test_buffer content: \n");
// 		for(i = 0; i < test_read; i++){
// 			printf("%c", test_buf[i]);
// 		}
// 		printf("\n");
// 		if (test_read != test_write) printf("Test failed.\n");
// 		else printf("This message appears if test successfully. \n");
// 		printf("Enter 0 to exit for next test, else to continue: ");
// 		test_read = test_read_term(0, test_buf, buffer_size);
// 		sign = test_buf[0] - '0';
// 	}
// 	printf("----------------Test for read and write end----------------\n");
// 	// next test for buffer newline
// 	// test for newline
// 	printf("\n");
// 	printf("------------------Test for newline-----------------\n");
// 	printf("It should print out 128 1s with new line changed: \n");
// 	for(i = 0; i < 128; i++){
// 		test_buf[i] = '1';
// 	}
// 	init_term();
// 	i = test_write_term(0, test_buf, buffer_size); // should print out 128 1s with new line changed
// 	printf("\n");
// 	i++; // to because test_write is used for keyboard and it accepts one enter, so minus 1 occur in function
// 		 // and we have to add 1 back here
// 	if(i == 128) printf("Test successfully.\n");
// 	else printf("Fail with printing %d\n", i);
// 	printf("----------------Test for newline end----------------\n");
// 	return PASS;
// }
// // test cases for rtc open and close
// // input: none
// // output: PASS/FAIL
// // side effect: none
// int rtc_driver_openc_close(){
// 	TEST_HEADER;
// 	printf("----------------Test for rtc valid oepn and write----------------------------------\n");
// 	if(rtc_open(NULL)!=0)return FAIL;
// 	rtc_read(0,NULL,0);				//check if the freq is 2 as required in doc
// 	printf("open_freq");
// 	rtc_read(0,NULL,0);
// 	printf("open_freq");
// 	rtc_read(0,NULL,0);
// 	printf("open_freq");
// 	rtc_read(0,NULL,0);
// 	printf("open_freq");
// 	rtc_read(0,NULL,0);
// 	printf("open_freq");
// 	rtc_read(0,NULL,0);
// 	printf("open_freq");
// 	if(rtc_close(0)!=0)return FAIL;
// 	printf("----------------rtc open and close end-----------------------------\n\n");
// 	return PASS;
// }
// // test cases for rtc valid write
// // input: none
// // output: PASS/FAIL
// // side effect: none
// int rtc_driver_valid_freqencies(){
// 	TEST_HEADER;
// 	uint32_t freqency;
// 	int i;
// 	printf("----------------Test for rtc valid write----------------------------------\n");
// 	printf("It should print out numbers from 2 to 1024 at the freqency of the number\n\n");
// 	for(freqency=2;freqency<=1024;freqency=freqency<<1){
// 		if(rtc_write(0,&freqency,4)==-1){		//can not write a valid frequency
// 			return FAIL;	
// 		}
// 		for(i=0;i<2*freqency;i++){	//print 2 secs
// 			rtc_read(0,NULL,0);
// 			printf("%u",freqency);
// 		}
// 		printf("\n");
// 	}
// 	printf("Test successfully.\n");
// 	printf("----------------Test for rtc valid write end-----------------------------\n\n");
// 	return PASS;
// }
// // test cases for rtc invalid write
// // input: none
// // output: PASS/FAIL
// // side effect: none
// int rtc_driver_test_invalid_frequencies(){
// 	TEST_HEADER;
// 	printf("----------------Test for rtc invalid write---------------------------------\n");
// 	int result = PASS;
// 	uint32_t freqency = 555;	//not a power of 2
// 	uint16_t freqency1 = 2048;	// a power of 2 but exceed 1024
// 	if(rtc_write(0,&freqency,4)!=-1 || rtc_write(0,&freqency1,4)!=-1){
// 		result = FAIL;
// 	}
// 	printf("Test successfully.\n");
// 	printf("----------------Test for rtc invalid write---------------------------------\n");
// 	return result;
// }

// // test cases for file system
// // input: filename
// // output: PASS/FAIL
// // side effect: none
// int test_file_ocrw(uint8_t* filename){
// 	TEST_HEADER;
// 	// first test for open file
// 	int32_t counter;
// 	int32_t bytelen;
// 	uint8_t buffer[50000]; // get a large buffer to test
// 	dentry_t test_dentry;
// 	printf("------------------------Test for open file---------------------------------\n");
// 	if(file_open(filename) == -1){
// 		printf("Fail to open file.\n");
// 	}
// 	else{
// 		printf("Open file successfully.\n");
// 	}
// 	printf("------------------------Test for open file end-----------------------------\n");
// 	sleep(1);
// 	// next test for close file
// 	printf("------------------------Test for close file--------------------------------\n");
// 	if(file_close(0) == -1){
// 		printf("Fail to close file.\n");
// 	}
// 	else{
// 		printf("Close file successfully.\n");
// 	}
// 	printf("------------------------Test for close file end----------------------------\n");
// 	sleep(1);
// 	// next test for read file
// 	printf("------------------------Test for read file---------------------------------\n");
// 	if(file_read(0, NULL, 0) == -1){
// 		printf("Fail to read file.\n");
// 	}
// 	else{
// 		printf("Read file successfully.\n");
// 	}
// 	printf("------------------------Test for read file end-----------------------------\n");
// 	sleep(1);
// 	// next test for write file
// 	printf("------------------------Test for write file--------------------------------\n");
// 	if(file_write(0, NULL, 0) == -1){
// 		printf("Fail to write file.\n");
// 	}
// 	else{
// 		printf("Write file successfully.\n");
// 	}
// 	printf("------------------------Test for write file end----------------------------\n");
// 	sleep(1);
// 	// next for listing filenames
// 	printf("------------------------Test for listing filenames--------------------------\n");
// 	//buffer[32] = '\0'; // set the last byte to be null, the filename is longest 32 bytes long
// 	printf("Files in this dir: \n");
// 	list_all_dir_ent();
// 	printf("\n");
// 	printf("------------------------Test for listing filenames end----------------------\n");
// 	sleep(1);
// 	// next for read file counter by name
// 	printf("------------------------Test for read file counter by name------------------\n");
// 	read_dentry_by_name(filename, &test_dentry);
// 	bytelen = read_data(test_dentry.inode_index, 0, buffer, 50000); // read from text
// 	for(counter = 0; counter < bytelen; counter++){
// 		putc(buffer[counter]);
// 	}
// 	printf("\n");
// 	sleep(1);
// 	read_dentry_by_name((uint8_t*)"frame1.txt", &test_dentry);
// 	bytelen = read_data(test_dentry.inode_index, 0, buffer, 50000); // read the file pingpong
// 	for(counter = 0; counter < bytelen; counter++){
// 		putc(buffer[counter]);
// 	}
// 	printf("\n");
// 	sleep(1);
// 	read_dentry_by_name((uint8_t*)"grep", &test_dentry);
// 	bytelen = read_data(test_dentry.inode_index, 0, buffer, 50000); // read the file pingpong
// 	for(counter = 0; counter < bytelen; counter++){
// 		putc(buffer[counter]);
// 	}
// 	printf("\n");
// 	sleep(1);
// 	read_dentry_by_name((uint8_t*)"ls", &test_dentry);
// 	bytelen = read_data(test_dentry.inode_index, 0, buffer,50000); // read the file pingpong
// 	for(counter = 0; counter < bytelen; counter++){
// 		putc(buffer[counter]);
// 	}
// 	printf("\n");
// 	sleep(1);
// 	read_dentry_by_name((uint8_t*)"verylargetextwithverylongname.txt", &test_dentry);
// 	bytelen = read_data(test_dentry.inode_index, 0, buffer, 50000); // read the file pingpong
// 	for(counter = 0; counter < bytelen; counter++){
// 		putc(buffer[counter]);
// 	}
// 	printf("\n");
// 	sleep(1);
// 	read_dentry_by_name((uint8_t*)"fish", &test_dentry);
// 	bytelen = read_data(test_dentry.inode_index, 0, buffer, 50000); // read the file pingpong
// 	for(counter = 0; counter < bytelen; counter++){
// 		putc(buffer[counter]);
// 	}
// 	printf("\n");
// 	printf("------------------------Test for read file counter by name end--------------\n");
// 	return PASS;
// }




/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	/* Checkpoint 1 tests */
	//TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	//TEST_OUTPUT("divide_zero exception", handler_DE_test());
	//TEST_OUTPUT("undefined opcode_test", handler_UD_test());
	//TEST_OUTPUT("paging_test_normal", paging_test(0));
	//TEST_OUTPUT("paging_test_null", paging_test(1));
	//TEST_OUTPUT("paging_test_beyond", paging_test(2));
	//TEST_OUTPUT("paging_test_left_boundary_of_video", paging_test(3));
	//TEST_OUTPUT("paging_test_right_boundary_of_video", paging_test(4));
	//TEST_OUTPUT("paging_test_left_boundary_of_kernel", paging_test(5));
	//TEST_OUTPUT("paging_test_right_boundary_of_kernel", paging_test(6));

	/* Checkpoint 2 tests */
	//TEST_OUTPUT("rtc open and close", rtc_driver_openc_close())
	//sleep(2);
	//TEST_OUTPUT("test valid rtc input",rtc_driver_valid_freqencies());
	//sleep(2);
	//TEST_OUTPUT("test invalid rtc input",rtc_driver_test_invalid_frequencies());
	//sleep(2);
	//TEST_OUTPUT("terminal_rw_test", test_terminal_rw());
	//sleep(2);
	//TEST_OUTPUT("test_file_ocrw", test_file_ocrw((uint8_t*)"frame0.txt"));
}

