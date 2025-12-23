#include "fs_indexed.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FileSystem fs;

void initFS() {
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        fs.VCB.free_blocks[i].blockNumber = i; //assigning block numbers from 0 to 63
    }
    FreeBlockNode** free_nodes = (FreeBlockNode**)malloc(sizeof(FreeBlockNode)*TOTAL_BLOCKS);
    free_nodes[TOTAL_BLOCKS-1] = (FreeBlockNode*)malloc(sizeof(FreeBlockNode));
    free_nodes[TOTAL_BLOCKS-1]->block = &(fs.VCB.free_blocks[TOTAL_BLOCKS-1]);
    free_nodes[TOTAL_BLOCKS-1]->next = NULL;
    for (int i = TOTAL_BLOCKS-2; i >= 0; i--) {
        free_nodes[i] = (FreeBlockNode*)malloc(sizeof(FreeBlockNode));
        free_nodes[i]->block = &(fs.VCB.free_blocks[i]);
        free_nodes[i]->next = free_nodes[i+1];//linking all free blocks
    }
    fs.VCB.freeBlockList.head = free_nodes[0];
    fs.VCB.freeBlockList.tail = free_nodes[TOTAL_BLOCKS-1];
    fs.VCB.freeBlockList.count = TOTAL_BLOCKS;

    for (int i = 0; i < MAX_FILES; i++) {
        fs.VCB.FIBidStatus[i] = 0; //initially, all available

        //currently, no files in the system, therefore files is an array of null pointers
        fs.VCB.files[i] = NULL;
    }

    fs.VCB.file_count = 0;

    printf("Filesystem initialized with 64 blocks of 1024 bytes each.\n");

    return;
}

FIB* getFileInformationBlockID() {
    int i = 0;
    while (fs.VCB.FIBidStatus[i]) { //find first 0 (available)
        i++;
    }
    FIB* fib = (FIB*)malloc(sizeof(FIB));
    fib->ID = i;
    fs.VCB.FIBidStatus[i] = 1; //this id is now occupied
    fs.VCB.files[fs.VCB.file_count] = fib; //add to list of files
    return fib;
}

Block* allocateFreeBlock() {
    Block* block = fs.VCB.freeBlockList.head->block;
    FreeBlockNode* ex_head = fs.VCB.freeBlockList.head;
    fs.VCB.freeBlockList.head = fs.VCB.freeBlockList.head->next;
    free(ex_head);
    fs.VCB.freeBlockList.count--;
    //printf("block allocated\n");
    return block;
}

void createFile(const char* filename, int size) {
    if (fs.VCB.file_count >= MAX_FILES) { //check file count
        printf("Max file count reached, cannot create new file.\n");
        return;
    }

    int required_block_count = size/1024; //check block count
    if ((required_block_count + 1) > fs.VCB.freeBlockList.count) { //plus one index block
        printf("Not enough space, cannot create new file.\n");
        return;
    }
    Block* index_block = allocateFreeBlock(); //allocateFreeBlock updates free block list

    for (int i = 0; i < required_block_count; i++) {
        Block* data_block_pointer = allocateFreeBlock();
        //printf("data block successfully allocated\n");
        memcpy(index_block->data + i*sizeof(data_block_pointer), &data_block_pointer, sizeof(data_block_pointer)); //store pointers to data blocks in order in index block
    }

    FIB* fib = getFileInformationBlockID(); //getFileInformationBlockID updates FIB* files, FIBidStatus
    strcpy(fib->name, filename);
    fib->size = size;
    fib->block_count = required_block_count + 1;
    fib->index_block = index_block;

    fs.VCB.file_count++; //increment file count

    printf("File '%s' created with %i data blocks + 1 index block.\n", filename, required_block_count);

    return;
}

void returnFreeBlock(Block* Block) {
    FreeBlockNode* tail = (FreeBlockNode*)malloc(sizeof(FreeBlockNode));
    tail->block = Block;
    tail->next = NULL;

    fs.VCB.freeBlockList.tail->next = tail;
    fs.VCB.freeBlockList.tail = tail;
    fs.VCB.freeBlockList.count++;
    return;
}

void deleteFile(const char* filename) {
    // Find the File
    int fileIndex = -1;
    for (int i = 0; i < fs.VCB.file_count; i++) {
        // Check if slot is used AND name matches
        if (fs.VCB.files[i] != NULL && strcmp(fs.VCB.files[i]->name, filename) == 0) {
            fileIndex = i;
            break;
        }
    }

    if (fileIndex == -1) {
        printf("File not found\n");
        return;
    }

    FIB* fib = fs.VCB.files[fileIndex];

    // Retrieve the Index Block
    Block* index_block = fib->index_block;

    // Retrieve and Return Data Blocks
    int data_block_count = fib->block_count - 1; // Subtract 1 for the index block itself

    for (int i = 0; i < data_block_count; i++) {
        Block* data_block_ptr;


        // i * sizeof(Block*) : offset
        memcpy(&data_block_ptr, index_block->data + (i * sizeof(Block*)), sizeof(Block*));


        returnFreeBlock(data_block_ptr); // Return the block to the free list
    }

    returnFreeBlock(index_block); // Return the Index Block itself

    // Cleanup FIB and System Status
    fs.VCB.FIBidStatus[fib->ID] = 0; // Set ID back to 0 (available)
    free(fib); // Free the FIB memory

    // Remove from file list (Swap with last file to fill NULL)
    if (fileIndex != fs.VCB.file_count - 1) {
        fs.VCB.files[fileIndex] = fs.VCB.files[fs.VCB.file_count - 1];
    }
    fs.VCB.files[fs.VCB.file_count - 1] = NULL;

    fs.VCB.file_count--;

    printf("File '%s' deleted.\n", filename);
    return;
}


void printFreeBlocks() {
    printf("Free Blocks (%d): ", fs.VCB.freeBlockList.count); // Print the header with the current free block count

    FreeBlockNode* currentNode = fs.VCB.freeBlockList.head; // Start at the head of the list

    while (currentNode != NULL) {
        // Print the current block number followed by an arrow
        printf("[%d] -> ", currentNode->block->blockNumber);

        currentNode = currentNode->next;  // Move to the next node
    }

    printf("NULL\n\n"); //end of the list
}

/*Root Directory Listing (2 files):
alpha.txt | 3072 bytes | 3 data blocks | FIBID=0
beta.txt | 5120 bytes | 5 data blocks | FIBID=1*/

void listFiles() {
    printf("Root Directory Listing (%d files): \n", fs.VCB.file_count);
    for (int i = 0; i < fs.VCB.file_count; i++) {
        FIB* fib = fs.VCB.files[i];
        int size = fib->size;
        int ID = fib->ID;
        printf("%s | %d bytes | %d data blocks | FIBID=%d\n", fib->name, size, size/1024, ID);
    }
    printf("\n");
    return;
}

