#include "filesystem.h"
#include "x86_desc.h"
#include "lib.h"
#include "types.h"
#include "system_calls.h"

inode_t* inode_ptr;
dentry_t* dentry_ptr;
boot_block_t* boot_block_ptr;
data_block_t* data_block_ptr;

int32_t dir_counter;
/* 
 * init_filesystem
 *   DESCRIPTION: initialize the filesystem
 *   INPUTS: fs_start_addr -- the start address of the filesystem
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void 
init_filesystem(uint32_t fs_start_addr)
{   
    /* the pointer type conversion
     * is based on the same size of 
     * different blocks             */ 
    boot_block_ptr = (boot_block_t*) fs_start_addr;
    inode_ptr = (inode_t*) (boot_block_ptr + 1); // inode starts at the second block
    data_block_ptr = (data_block_t*)(inode_ptr+boot_block_ptr->inodes_num);
    dentry_ptr = boot_block_ptr->dentries; // dentry starts at the third block
    dir_counter = 0;
}

/* 
 * read_dentry_by_name
 *   DESCRIPTION: read the dentry by name
 *   INPUTS: fname -- the file name
 *           dentry -- the dentry to be filled
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t 
read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
{
    if(fname==NULL) return -1; // invalid filename
    // dentry_t* dentry_ptr_local;
    int i;
    int len = strlen((int8_t*)fname);
    int shortstrlen;
    if (len > FILENAME_LENGTH) len = FILENAME_LENGTH; // truncate the filename if it is too long
    // search the dentry by name
    for (i = 0; i < boot_block_ptr->dir_entries_num; i++) {
        shortstrlen = strlen((int8_t*)(dentry_ptr[i].filename));
        if(shortstrlen>32) shortstrlen=32;
        if(len!=shortstrlen) continue;
        if (strncmp((int8_t*)fname, (int8_t*)(dentry_ptr[i].filename), len) == 0) { // found
            strncpy((int8_t*)(dentry->filename), (int8_t*)(dentry_ptr[i].filename), len); // copy the dentry
            dentry->filename[len]='\0';
            dentry->file_type = dentry_ptr[i].file_type;
            dentry->inode_index = dentry_ptr[i].inode_index;
            return 0;
        }
    }
    return -1;
}

/*
* read_dentry_by_index
*   DESCRIPTION: read the dentry by index
*   INPUTS: index -- the index of the dentry
*           dentry -- the dentry to be filled
*   OUTPUTS: none
*   RETURN VALUE: 0 on success, -1 on failure
*   SIDE EFFECTS: none
*/
int32_t
read_dentry_by_index (uint32_t index, dentry_t* dentry)
{
    if (index >= boot_block_ptr->dir_entries_num) return -1; // index out of bound
    if (index >= FILE_NUMBER) return -1; // index out of bound (should not happen
    // copy the dentry based on the index
    strncpy((int8_t*)(dentry->filename), (int8_t*)((dentry_ptr[index]).filename), FILENAME_LENGTH);
    dentry->filename[FILENAME_LENGTH]='\0';
    dentry->file_type = dentry_ptr[index].file_type;
    dentry->inode_index = dentry_ptr[index].inode_index;
    return 0;
}

/*
* read_data
*   DESCRIPTION: read the data from the file(dentry)
*   INPUTS: inode -- the inode of the file
*           offset -- the offset of the file
*           buf -- the buffer to be filled
*           length -- the length of the data to be read
*   OUTPUTS: none
*   RETURN VALUE: the number of bytes read
*   SIDE EFFECTS: none
*/
int32_t
read_data(uint32_t inode, uint32_t offset, uint8_t *buffer, uint32_t length)
{   //printf("%x\n",buffer);
    inode_t* inode_ptr_local = &inode_ptr[inode];
    uint32_t i; //the index of the data block
    uint32_t byte_count=0; // byte_count is the number of bytes read
    uint32_t begin_block_idx;
    uint32_t begin_block_offset;
    uint32_t end_bnlock_idx;
    uint32_t end_block_offset;
    data_block_t* curr_block_ptr;
    if(length == 0) return 0; // reach the end of the file
    if(inode >= boot_block_ptr->inodes_num) return -1; // index out of bound
    if(offset >= inode_ptr_local->file_length) return 0; // offset out of bound
    if(length+offset > inode_ptr_local->file_length){ 
        length = inode_ptr_local->file_length - offset;} // truncate the length if it is too long
    
    begin_block_idx = offset / BLOCK_SIZE;
    begin_block_offset = offset % BLOCK_SIZE;
    end_bnlock_idx = (offset + length -1) / BLOCK_SIZE;
    end_block_offset = (length - 1 + offset) % BLOCK_SIZE;
    if(begin_block_idx==end_bnlock_idx){  // the data is in the same block
        curr_block_ptr = &(data_block_ptr[(inode_ptr_local->data_blocks_idxs)[begin_block_idx]]);
        memcpy(buffer, &(curr_block_ptr->data_blocks[begin_block_offset]), length);
        byte_count = fill_file_buffer(&buffer, byte_count,length);
    }
    else{
        i=begin_block_idx;
        while(i<=end_bnlock_idx){
            curr_block_ptr = &(data_block_ptr[(inode_ptr_local->data_blocks_idxs)[i]]);
            if(i==begin_block_idx){ // the data is in the first block
                memcpy(buffer, &(curr_block_ptr->data_blocks[begin_block_offset]), BLOCK_SIZE-begin_block_offset);
                byte_count = fill_file_buffer(&buffer, byte_count,BLOCK_SIZE-begin_block_offset);
            }
            else if(i==end_bnlock_idx){ // the data is in the last block
                memcpy(buffer, curr_block_ptr->data_blocks, end_block_offset+1);
                byte_count = fill_file_buffer(&buffer, byte_count,end_block_offset+1);
            }
            else{ // the data is in the middle block
                memcpy(buffer, curr_block_ptr->data_blocks, BLOCK_SIZE);
                byte_count = fill_file_buffer(&buffer, byte_count, BLOCK_SIZE);
            }
            i++;
    }
    }
    //printf("%x\n",buffer);
    return byte_count;
}

/*
* fill_file_buffer
*   DESCRIPTION: fill the file buffer
*   INPUTS: buffer_ptr -- the pointer to the buffer
*           byte_count -- the number of bytes read
*           offset -- the offset of the file
*   OUTPUTS: none
*   RETURN VALUE: the number of bytes read
*   SIDE EFFECTS: none
*/
uint32_t
fill_file_buffer(uint8_t** buffer_ptr, uint32_t byte_count, uint32_t offset)
{
    *buffer_ptr += offset; // move the buffer pointer
    return byte_count+offset; // return the number of bytes read
}

/*
* file_open
*   DESCRIPTION: open the file
*   INPUTS: filename -- the name of the file
*   OUTPUTS: none
*   RETURN VALUE: 0 on success, -1 on failure
*   SIDE EFFECTS: none
*/
int32_t
file_open(const uint8_t *filename)
{
    dentry_t dentry;
    if(read_dentry_by_name(filename, &dentry) == -1) return -1; // file not found
    return 0;
}

/*
* file_close
*   DESCRIPTION: close the file
*   INPUTS: fd -- the file descriptor
*   OUTPUTS: none
*   RETURN VALUE: 0 on success, -1 on failure
*   SIDE EFFECTS: none
*/
int32_t
file_close(int32_t fd)
{
    return 0;
}

/*
* file_read
*   DESCRIPTION: read the file
*   INPUTS: fd -- the file descriptor
*           buf -- the buffer to be filled
*           nbytes -- the number of bytes to be read
*   OUTPUTS: none
*   RETURN VALUE: the number of bytes read
*   SIDE EFFECTS: none
*/
int32_t
file_read(int32_t fd, void *buf, int32_t nbytes)
{
    file_descriptor_t file_descriptor;
    uint32_t inode, offset;
    PCB_t* pcb_ptr;
    pcb_ptr=get_current_pcb();
    file_descriptor = pcb_ptr->file_array[fd]; // get the file descriptor
    inode = file_descriptor.inode; // get the inode
    offset = file_descriptor.file_position; // get the offset  
    return read_data(inode, offset, buf, nbytes);
}

/*
* file_write
*   DESCRIPTION: write the file
*   INPUTS: fd -- the file descriptor
*           buf -- the buffer to be written
*           nbytes -- the number of bytes to be written
*   OUTPUTS: none
*   RETURN VALUE: -1
*   SIDE EFFECTS: none
*/
int32_t
file_write(int32_t fd, const void *buf, int32_t nbytes)
{
    return -1; // no pcb implemented
}

/*
* dir_read
*   DESCRIPTION: read the directory
*   INPUTS: fd -- the file descriptor
*           buf -- the buffer to be filled
*           index -- the maximumbytes of a direc entry
*   OUTPUTS: none
*   RETURN VALUE: 0 on success, or lens for existing filename
*   SIDE EFFECTS: none
*/
int32_t
dir_read(int32_t fd, void *buf, int32_t bytes)
{
    dentry_t dentry=dentry_ptr[dir_counter];
    uint32_t filename_len = 0;
    uint32_t i;
    int8_t temp[33];
    temp[32] = '\0';
    if(dir_counter >= boot_block_ptr->dir_entries_num){
        dir_counter = 0;
        return 0; // index out of bound
    }
    filename_len = strlen(((int8_t*)&(dentry.filename)));
    if(filename_len > 32){
        filename_len = 32;
        for(i = 0; i<32; i++){
            temp[i] = dentry.filename[i];
        }
        memcpy(buf, &temp, FILENAME_LENGTH+1);
        dir_counter++;
        return filename_len;
    }
    memcpy(buf, &dentry.filename, FILENAME_LENGTH);
    dir_counter++;
    return filename_len;
}

// list_all_dir_ent
// input: none
// output: none
// sideeffect: print all filename in current directory.
// void
// list_all_dir_ent(void){
//     int32_t counter;
//     char buffer[33]; // maximum size of filename including '0\'
//     buffer[32] = '\0';
//     for(counter = 0; counter < boot_block_ptr->dir_entries_num; counter++){
//         if(dir_read(0, buffer, counter)==0){
//             printf((char*)buffer);
//             printf("  ");
//         }
//     }
// }


/*
* dir_write
*   DESCRIPTION: write the directory
*   INPUTS: fd -- the file descriptor
*           buf -- the buffer to be written
*           nbytes -- the number of bytes to be written
*   OUTPUTS: none
*   RETURN VALUE: -1
*   SIDE EFFECTS: none
*/
int32_t
dir_write(int32_t fd, const void *buf, int32_t nbytes)
{
    return -1; // no pcb implemented
}

/*
* dir_open
*   DESCRIPTION: open the directory
*   INPUTS: filename -- the name of the directory
*   OUTPUTS: none
*   RETURN VALUE: 0 on success, -1 on failure
*   SIDE EFFECTS: none
*/
int32_t
dir_open(const uint8_t *filename)
{
    return -1;
}

/*
* dir_close
*   DESCRIPTION: close the directory
*   INPUTS: fd -- the file descriptor
*   OUTPUTS: none
*   RETURN VALUE: 0 on success, -1 on failure
*   SIDE EFFECTS: none
*/
int32_t
dir_close(int32_t fd)
{
    return 0;
}
/*
* get_num_bytes
*   DESCRIPTION: return the number of bytes of the inode
*   INPUTS: inode-index
*   OUTPUTS: none
*   RETURN VALUE: number of bytes
*   SIDE EFFECTS: none
*/
uint32_t
get_num_bytes(int32_t inode_index){
    return inode_ptr[inode_index].file_length;
}
