#ifndef FS_INDEXED_H
#define FS_INDEXED_H

#define BLOCK_SIZE 1024
#define TOTAL_BLOCKS 64
#define MAX_FILES 10

typedef struct Block{
    unsigned char data[BLOCK_SIZE];
    int blockNumber;
}Block;

typedef struct FIB{
    int ID;
    char name[32];
    int size;
    int block_count;
    Block* index_block;
}FIB;

typedef struct FreeBlockNode {
    Block* block;
    struct FreeBlockNode* next;
}FreeBlockNode;

typedef struct FreeBlockList {
    FreeBlockNode* head;
    FreeBlockNode* tail;
    int count;
}FreeBlockList;

typedef struct VCB{
    FreeBlockList freeBlockList;
    Block free_blocks[TOTAL_BLOCKS];
    int FIBidStatus[MAX_FILES];
    FIB* files[MAX_FILES];
    int file_count;
}VCB;

typedef struct FileSystem{
    VCB VCB;
}FileSystem;

extern FileSystem fs;

void initFS();
void createFile(const char* filename, int size);
void deleteFile(const char* filename);
void listFiles();
void printFreeBlocks();
Block* allocateFreeBlock();
void returnFreeBlock(Block* Block);
FIB* getFileInformationBlockID(); 


#endif

 
