#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "types.h"

#define FILENAME_LENGTH 32
#define BLOCK_SIZE 4096 // 4KB
#define DENTRY_RESERVED 24
#define BOOT_BLOCK_RESERVED 52
#define FILE_NUMBER 63

typedef struct dentry_t {
    int8_t filename[FILENAME_LENGTH];
    int32_t file_type;
    int32_t inode_index;
    int8_t reserved[DENTRY_RESERVED];
} dentry_t;

typedef struct boot_block_t {
    int32_t dir_entries_num;
    int32_t inodes_num;
    int32_t data_blocks_num;
    int8_t reserved[BOOT_BLOCK_RESERVED];
    dentry_t dentries[FILE_NUMBER];
} boot_block_t;

typedef struct inode_t {
    int32_t file_length;
    int32_t data_blocks_idxs[BLOCK_SIZE / 4 - 1]; // 1023
} inode_t;

typedef struct data_block_t
{
    uint8_t data_blocks[BLOCK_SIZE];
} data_block_t;


int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t *dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length);

void init_filesystem(uint32_t fs_start_addr);

int32_t file_open(const uint8_t *filename);
int32_t file_close(int32_t fd);
int32_t file_read(int32_t fd, void *buf, int32_t nbytes);
int32_t file_write(int32_t fd, const void *buf, int32_t nbytes);

int32_t dir_open(const uint8_t *filename);
int32_t dir_close(int32_t fd);
int32_t dir_read(int32_t fd, void *buf, int32_t nbytes);
int32_t dir_write(int32_t fd, const void *buf, int32_t bytes);
uint32_t get_num_bytes(int32_t inode_index);
uint32_t fill_file_buffer(uint8_t **buffer, uint32_t byte_count, uint32_t offset);
void list_all_dir_ent(void);

#endif
