#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "page.h"
#include "rtc.h"
#include "filesystem.h"

#define PASS 1
#define FAIL 0

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


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/*
* handler_DE test
* 
* Test the functionality of divide_zero exception handler
* Inputs: None
* Outputs: PASS/FAIL
* Side Effects: None
* Coverage: divide_zero exception handler
* Files: idt.c/h
*/
int handler_DE_test(){
	TEST_HEADER;
	int a=1;
	int b=0;
	int c=a/b;		//divide by zero
	c++;
	return FAIL;	//should not reach here if exceptin happens if exceptin happens
}

/*
* handler_UD test
* test the functionality of undefined opcode exception handler
* Inputs: None
* Outputs: PASS/FAIL
* Side Effects: None
* Coverage: undefined opcode exception handler
* Files: idt.c/h
*/
int handler_UD_test(){
	TEST_HEADER;
	asm volatile("ud2"); //undefined opcode
	return FAIL; //should not reach here if exceptin happens if exceptin happens
}


// test cases for paging
// input: test_mode, 0 for normal test, 1 for null pointer test, 2 for far pointer test
//        3,4,5,6 for boundary test of video memory and kernel memory
// output: PASS/FAIL
// side effect: print out the result of the test
int paging_test(int test_mode){
	TEST_HEADER;
	uint32_t counter;
	uint8_t* video_mem = (uint8_t*)VIDEO_PAGE_BEGIN; // 0xB8000 is the beginning of video memory in virtual memory
	uint8_t* kernel_mem = (uint8_t*)KERNEL_PAGE_BEGIN; // 0x400000 is the beginning of kernel memory in virtual memory
	uint32_t video_mem_len = 0x1000; // 4KB
	uint32_t kernel_mem_len = 0x400000; // 4MB
	uint8_t thing;
	uint8_t* nullptr = NULL;
	uint8_t* far_ptr = (uint8_t*)0x888888; // far beyond the range of setting page
	switch(test_mode){
		case 0:
			printf("Paging video memory, should print last hex num.\n");
			for(counter = 0; counter < video_mem_len; counter+=0x80){ // test every 128B
				thing = *(video_mem + counter);
			}
			printf("MEM ADDR FROM %d AND LAST %x: %x\n", counter, (video_mem + counter - 1), thing); // print the address and the value of last time

			printf("Paging kernel memory, should print lst hex num.\n");
			for(counter = 0; counter < kernel_mem_len; counter+=0x20000){ // test every 128KB
				thing = *(kernel_mem + counter);
			}
			printf("MEM ADDR FROM %d AND LAST %x: %x\n", counter, (kernel_mem + counter - 1), thing); // print the address and the value of last time
			break;
		case 1:
			printf("Null pointer, should cause page fault and print exception.\n");
			thing = *nullptr; // should cause a page fault because of accessing null and print out the error message
			return FAIL;
		case 2:
			printf("Far pointer, should cause page fault and print exception.\n");
			thing = *far_ptr; // should cause a page fault because of going beyond and print out the error message
			return FAIL;
		case 3:
			printf("Test B8000-1, should cause page fault:");
			thing = *(video_mem - 1); // test left boundary of video mem
			return FAIL;
		case 4:
			printf("Test B9000, should cause page fault:");
			thing = *(video_mem + video_mem_len); // test right boundary of video mem
			return FAIL;
		case 5:
			printf("Test 0x400000-1, should cause page fault:");
			thing = *(kernel_mem - 1); // test left boundary of kernel mem
			return FAIL;
		case 6:
			printf("Test 0x800000, should cause page fault:");
			thing = *(kernel_mem + kernel_mem_len); //test right boundary of kernel mem
			return FAIL;
	}
	return PASS;
}

// add more tests here

/* Checkpoint 2 tests */
// test cases for rtc valid write
// input: none
// output: PASS/FAIL
// side effect: none
int rtc_driver_valid_freqencies(){
	TEST_HEADER;
	uint32_t freqency;
	int i;
	for(freqency=2;freqency<=1024;freqency=freqency<<1){
		if(rtc_write(0,&freqency,4)==-1){		//can not write a valid frequency
			return FAIL;	
		}
		for(i=0;i<=2*freqency;i++){
			rtc_read(0,NULL,0);
			printf("%u",freqency);
		}
	}
	return PASS;
}
// test cases for rtc invalid write
// input: none
// output: PASS/FAIL
// side effect: none
int rtc_driver_test_invalid_frequencies(){
	TEST_HEADER;
	int result = PASS;
	uint32_t freqency = 555;	//not a power of 2
	uint16_t freqency1 = 2048;	// a power of 2 but exceed 1024
	if(rtc_write(0,&freqency,4)!=-1 || rtc_write(0,&freqency1,4)!=-1){
		result = FAIL;
	}
	return result;
}

/* Read by name test
 * 
 * Asserts that we can read dentry by name
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */
int read_by_name_test(uint8_t* filename) {
	TEST_HEADER;
	dentry_t test;
	printf("READ FILE TEST");
	printf(" \n");
	if (read_dentry_by_name(filename, &test) == -1)
		return FAIL;
	printf("The file's name is %s!\n",test.filename);

	if(strncmp((int8_t*)test.filename,(int8_t*)filename,FILENAME_LENGTH)!=0)
		return FAIL;
	return PASS;
}

/* Access data by name test
 * 
 * Asserts that we can read data from data block
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */
int read_data_test(uint8_t* filename) {
	// TEST_HEADER;
	dentry_t test;
	char buff[40000] = {'\0'};
	int i;
	int32_t bytes_read;
	read_dentry_by_name(filename,&test);
	bytes_read = read_data(test.inode_index,0,(uint8_t*)buff,100000);
	for(i=0; i <bytes_read; i++)
		putc(buff[i]);
	return PASS;
}



/* Read directory test
 * 
 * Asserts that we can read directory
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Read directory
 */
int read_directory_test(){
	TEST_HEADER;
	int32_t fd=0;
	int i;
	uint8_t buf[4096];
	buf[32] = '\0';
	// 63 is the max directory number in filesystem
	for (i = 0; i < 63; i++){
		if (dir_read(fd, buf, i) == -1)
			break;
		printf((int8_t*)buf);
		printf("\n");
	}
	return PASS;
}

/*
* File Open test
* Asserts we can open the file
* Inputs: None
* Outputs: PASS/FAIL
* Side Effects: None
* Coverage: Open the file
*/
int open_file_test(uint8_t* filename){
	TEST_HEADER;
	int32_t i;
	if (file_open(filename) == 0){
		printf("Valid Filename: ");
		for(i = 0; i<strlen((int8_t*)filename); i++){
			putc(filename[i]);
		}
		printf("\n");
		return PASS;
	}
	printf("Invalid Filename: ");
	for(i = 0; i<strlen((int8_t*)filename); i++){
		putc(filename[i]);
	}
	printf("\n");
	return PASS;
}

/*
* File Close test
* Asserts we can close the file
* Inputs: None
* Outputs: PASS/FAIL
* Side Effects: None
* Coverage: Close the file
*/
int close_file_test(){
	TEST_HEADER;
	int fd = 0;
	if (file_close(fd) == 0)
		return PASS;
	return FAIL;
}

/*
* File Write test
* Asserts we can write the file
* Inputs: None
* Outputs: PASS/FAIL
* Side Effects: None
* Coverage: Write the file
*/
int write_file_test(){
	TEST_HEADER;
	if (file_write(0, NULL, 0) == 0)
		return PASS;
	return FAIL;
}

/*
* File Read test
* Asserts we can read the file
* Inputs: None
* Outputs: PASS/FAIL
* Side Effects: None
* Coverage: Read the file
*/

int read_file_test(){
	TEST_HEADER;
	if (file_read(0, NULL, 0) == 0)
		return PASS;
	return FAIL;
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
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
	//checkpoint 2
	//TEST_OUTPUT("test valid rtc input",rtc_driver_valid_freqencies());
	//TEST_OUTPUT("test invalid rtc input",rtc_driver_test_invalid_frequencies());
	// TEST_OUTPUT("open_file_test_invalid", open_file_test((uint8_t*)"nonexistant_filename"));
	// TEST_OUTPUT("open_file_test_valid", open_file_test((uint8_t*)"frame1.txt"));
	// TEST_OUTPUT("read_file_test", read_file_test());
	// TEST_OUTPUT("write_file_test", write_file_test());
	// TEST_OUTPUT("read_file_name_test", read_by_name_test((uint8_t*)"pingpong"));
	TEST_OUTPUT("read_data_test", read_data_test((uint8_t*)"frame1.txt"));
}

