#include <sys/mman.h> /*For mmap() function*/
#include <string.h> /*For memcpy function*/
#include <fcntl.h> /*For file descriptors*/
#include <stdlib.h> /*For file descriptors*/
#include <stdio.h>
#include <unistd.h> // for close()

/*the logical address space (2^16 = 65536 bytes) is larger than
 * the physical address space (2^15 = 32768 bytes), and
 * the page size is 2^8 = 256 bytes.
 * The maximum no. of entries in the TLB = 16. */

//15 bits for the physical address
//16 bits for the logical address - 2 bytes
//2^8 = 256 pages
//2^7 = 128 frames
//8 bit offset
//7 bit frame number - signed char page table
//8 bit page number

#define LOGICAL_MEM_SIZE 65536
#define PHYS_MEM_SIZE 32768
#define OFFSET_MASK 255 //eight 1's in binary
#define PAGES 256
#define OFFSET_BITS 8
#define PAGE_SIZE 256 //2^8 bytes
#define BUFFER_SIZE 16


//**ADDED for TLB and statistics**
// New TLB Definition and variables 
//*************************************************** */
// Initialization of TLB
#define TLB_SIZE 16

typedef struct {
    int page_number;
    int frame_number;
} TLBEntry;

TLBEntry tlb[TLB_SIZE];
int tlb_rear = 0; // FIFO pointer for TLB
int tlb_size = 0; // Current size of TLB

// New Statistics variables 
int total_tlb_hits = 0;
int total_page_faults = 0;
int total_addresses = 0;

//*************************************************** */




signed char page_table[PAGES];

signed char *backing_store_ptr; //a signed character pointer to store the starting address of the memory mapped file.

typedef struct { //implemented as a circular array, implementation references internet
    int capacity;
    signed char data[PHYS_MEM_SIZE]; // 2^15 bytes
    int rear;    // index of next insertion position
    int size;    // number of elements currently stored
} PhysicalMemory;

PhysicalMemory ram = { .capacity = PHYS_MEM_SIZE, .rear = 0, .size = 0 };





// New TLB functions 
//*************************************************** */

int search_TLB(int page_n) { //Iterate through TLB to find page number
    for (int i = 0; i < tlb_size; i++) {
        if (tlb[i].page_number == page_n) {
            total_tlb_hits++;
            return tlb[i].frame_number; // TLB Hit
        }
    }
    return -1; // page is not found - TLB Miss
}

void TLB_Add(int page_n, int frame_n) {
    // adds a new mapping to the TLB using FIFO 
    tlb[tlb_rear].page_number = page_n;
    tlb[tlb_rear].frame_number = frame_n;

    tlb_rear = (tlb_rear + 1) % TLB_SIZE; //ensures it wraps back to 0 when it reaches 16. 
    
    if (tlb_size < TLB_SIZE) {
        tlb_size++;
    }
}

void TLB_Update(int victim_page_n, int new_page_n, int new_frame_n) {
    // is called when a page is replaced in RAM due to FIFO.
    //finds the victim entry and replaces it with the new page's info.
    for (int i = 0; i < tlb_size; i++) {
        if (tlb[i].page_number == victim_page_n) {
            tlb[i].page_number = new_page_n;
            tlb[i].frame_number = new_frame_n;
            return; 
        }
    }
} 

//*************************************************** */ 


//Page_in function modified to update TLB and page fault count
void page_in(int page_n) {
    total_page_faults++; // page_in is called. Increment page fault count
    int victim_page_n = -1; //not found yet
    int target_frame_n = ram.rear >> OFFSET_BITS; // get the frame number where the new page will be loaded. (the oldest one)

    if (ram.size >= ram.capacity) {
        // Memory is full, must replace a page (FIFO)
        for (int i = 0; i < PAGES; i++){
            if (page_table[i] == target_frame_n) {
                page_table[i] = -1; // victim's page table entry is swapped out
                victim_page_n = i; // Get the victim page number
                break;
            }
        }
    } else {
        ram.size += PAGE_SIZE;
    }
    
    memcpy(ram.data + ram.rear, backing_store_ptr + (page_n << OFFSET_BITS), PAGE_SIZE); // Copy page from backing store 
    page_table[page_n] = target_frame_n; // Update Page Table

    // Update TLB if a victim was found
    if (victim_page_n != -1) {
        TLB_Update(victim_page_n, page_n, target_frame_n);
    }
    
    ram.rear = (ram.rear + PAGE_SIZE) % ram.capacity;
    return;
}


//main function modified to include TLB search and addition
int main() {
    //initialization
    for (int i = 0; i < PAGES; i++) {
        page_table[i] = -1;
    }
    
    // Initialize TLB  
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i].page_number = -1;
        tlb[i].frame_number = -1;
    }

    int mmapfile_fd = open("BACKING_STORE.bin", O_RDONLY);
    if (mmapfile_fd == -1) { perror("open"); exit(1); }
    backing_store_ptr = mmap(0, LOGICAL_MEM_SIZE, PROT_READ, MAP_PRIVATE, mmapfile_fd, 0);
    if (backing_store_ptr == MAP_FAILED) { perror("mmap"); exit(1); }


    FILE *fptr = fopen("addresses.txt", "r");
    char buff[BUFFER_SIZE];
    int logical_addr, page_n, frame_n, offset, physical_addr, value;
    int tlb_hit_frame_n; // Variable to store frame number from TLB

    //simulating program running and accessing different logical addresses
    while(fgets(buff, BUFFER_SIZE, fptr) != NULL){
        total_addresses++; // Increment total addresses counter
        
        logical_addr = atoi(buff);
        page_n = logical_addr >> OFFSET_BITS;
        offset = logical_addr & OFFSET_MASK;

        //retrieving frame number
        
        // 1. Check TLB
        tlb_hit_frame_n = search_TLB(page_n);

        if (tlb_hit_frame_n != -1) {
            // TLB Hit! use the frame number from the TLB.
            frame_n = tlb_hit_frame_n;
        } else {
            // TLB miss, check page table
            if (page_table[page_n] == -1) {
                page_in(page_n); //Load page into RAM (updates page_table) 
            }
            
            frame_n = page_table[page_n]; // Frame number retrieved from the Page Table. 
            TLB_Add(page_n, frame_n); // Add the new Page Table entry to the TLB 
        }
        
        //frame number retrieved, compute physical address
        physical_addr = (frame_n << OFFSET_BITS) | offset;

        //access value at physical address
        value = ram.data[physical_addr];

        //output
        printf("Virtual address: %d, Physical address = %d, Value = %d\n", logical_addr, physical_addr, value);
    }
    
    // Output Statistics  
    printf("Total addresses = %d\n", total_addresses);
    printf("Page_faults = %d\n", total_page_faults);
    printf("TLB Hits = %d\n", total_tlb_hits);

    //closing files
    fclose(fptr);
    munmap(backing_store_ptr, LOGICAL_MEM_SIZE);
    close(mmapfile_fd);
    return 0;
} 