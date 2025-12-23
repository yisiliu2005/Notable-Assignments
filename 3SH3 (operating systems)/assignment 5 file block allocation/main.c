/*• Initialize the file system.
• Create multiple files of different sizes.
• Delete some files and verify that blocks are reclaimed.
• Display the directory and free block list after each operation.*/

#include <stdio.h>
#include "fs_indexed.h" 

int main() {
    initFS();

    createFile("alpha.txt", 3072);
    createFile("beta.txt", 5120);

    listFiles();
    printFreeBlocks();

    deleteFile("alpha.txt");

    listFiles();
    printFreeBlocks();

    createFile("gamma.txt", 4096);
    createFile("delta.txt", 8192);

    listFiles();
    printFreeBlocks();

    deleteFile("beta.txt");
    deleteFile("gamma.txt");
    deleteFile("delta.txt");

    listFiles();
    printFreeBlocks();

    return 0;
}  

