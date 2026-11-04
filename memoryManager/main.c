#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct memoryBlock 
{
    int start;
    int size;
    int allocatedId;
    int status; // 0 for free, 1 for allocated
};

void allocateMemory(struct memoryBlock **blocks, int *blockCount, int *capacity, int totalMemorySize, int *nextId, int strategy);
void releaseMemory(struct memoryBlock **blocks, int *blockCount);
void resizeAllocation(unsigned char  *memoryPool, struct memoryBlock **blocks, int *blockCount, int *capacity, int totalMemorySize, int strategy);
bool insertFreeBlock(struct memoryBlock **blocks, int *blockCount, int *capacity, int start, int size, int insertIndex);
void memoryMapDisplay(struct memoryBlock *blocks, int blockCount);
void displayMemoryStatics(struct memoryBlock *blocks, int blockCount, int totalMemorySize);
void changeAllocationStrategy(int *strategy);
void mergeAdjacentBlocks(struct memoryBlock *blocks, int *blockCount);
void calculateFreeMemory(struct memoryBlock *blocks, int blockCount, int *totalFreeMemory, int *largestFreeBlock);
int findSuitableBlock(struct memoryBlock *blocks, int blockCount, int size, int strategy, int excludeIndex);

int main()
{
    printf("----------------Smart Memory Manager-------------------\n");
    // Prompt the user for the total memory size
    int totalMemorySize;
    printf("Enter the total memory size: \n");

    if(scanf("%d", &totalMemorySize) != 1)
    {
        printf("Invalid input. Please enter a valid integer for memory size. \n");
        return 1;
    }

    if(totalMemorySize < 1 || totalMemorySize > 1000000)
    {
        printf("Please enter a valid memory size between 1 and 1000000. \n");
        return 1;
    }

    // Allocate the memory pool based on user - defined size
    unsigned char *memoryPool = malloc(totalMemorySize * sizeof(unsigned char));

    if(memoryPool == NULL)
    {
        printf("Memory allocation failed. Exiting program. \n");
        return 1;
    }

    // metadata for memory blocks
    int nextId = 1; // unique identifier for each allocated block
    int blockCount = 1; // start with one block representing the entire memory pool
    int capacity = 1; // Initial capacity for the blocks array 
    int strategy = 1; 

    struct memoryBlock *blocks = malloc(capacity * sizeof(struct memoryBlock));

    if(blocks == NULL)
    {
        printf("Memory allocation for blocks failed. Exiting Program. \n");
        free(memoryPool);
        return 1;
    }
    // Initialize the first block to represent the entire memory pool as free
    blocks[0].start = 0;
    blocks[0].size = totalMemorySize;
    blocks[0].allocatedId = -1; // -1 indicates free block
    blocks[0].status = 0; // 0 indicates free block

    printf("Select an allocation strategy (1. First Fit, 2.Best Fit, 3. Worst Fit) \n");
    scanf("%d", &strategy);
    if(strategy < 1 || strategy > 3)
    {
        printf("Invalid strategy selection. Defaulting to First Fit. \n");
        strategy = 1;
    }
    printf("Strategy selected: %s \n", strategy == 1 ? "First Fit" : (strategy == 2 ? "Best Fit" : "Worst Fit"));
    
    while(1)
    {
        printf("\n\n1. Allocate Memory\n");
        printf("2. Release Memory\n");
        printf("3. Resize an allocation\n");
        printf("4. Display Memory Map\n");
        printf("5. Display Memory Statistics\n");
        printf("6. Change Allocation Strategy\n");
        printf("7. Exit\n");

        int choice;
        printf("Enter your choice: ");

        if(scanf("%d", &choice) != 1)
        {
            printf("Invalid input. please enter a number between 1 and 7. \n");
            continue;
        }

        if(choice < 1 || choice > 7)
        {
            printf("Invalid choice. Please enter a number between 1 and 7. \n");
            continue;
        }

        switch(choice)
        {
            case 1:
                //Call the function to allocate memory
                allocateMemory(&blocks, &blockCount, &capacity, totalMemorySize, &nextId, strategy);
                break;
            case 2:
                releaseMemory(&blocks, &blockCount);
                break;    
            case 3:
                // call the function to resize an allocation
                resizeAllocation(memoryPool, &blocks, &blockCount, &capacity, totalMemorySize, strategy);
                break; 
            case 4:
                memoryMapDisplay(blocks, blockCount);   
                break; 
            case 5:
                displayMemoryStatics(blocks, blockCount, totalMemorySize);   
                break; 
            case 6:
                changeAllocationStrategy(&strategy); 
                break;   
            case 7:
                // Exit the program
                free(blocks);
                free(memoryPool);
                printf("Exiting program. \n");
               return 0;    
            default:
               break;    
        }
    }
    return 0;
}

void allocateMemory(struct memoryBlock **blocks, int *blockCount, int *capacity, int totalMemorySize, int *nextId, int strategy)
{
    // ask the user for the size of memory to allocate 
    int size;
    printf("Enter the size of memory to allocate: ");

    if(scanf("%d", &size) != 1)
    {
        printf("Invalid input. Please enter a valid integer for memory size. \n");
        return;
    }

    if(size < 1 || size > totalMemorySize)
    {
        printf("Please enter a valid memory size between 1 and %d, \n", totalMemorySize);
        return;
    }

    int selectedBlockIndex = findSuitableBlock(*blocks, *blockCount, size, strategy, -1);

    if(selectedBlockIndex == -1)
    {
        int totalFreeMemory = 0;
        int largestFreeBlock = 0;
        // Calculate total free memory and largest free block
        calculateFreeMemory(*blocks, *blockCount, &totalFreeMemory, &largestFreeBlock);

        printf("No suitable block found for allocation. \n");

        if(totalFreeMemory >= size && largestFreeBlock < size)
        {
            printf("External Fragmentation detected!\n");
            printf("Total free memory: %d bytes\n", totalFreeMemory);
            printf("Largest free block: %d bytes\n", largestFreeBlock);
            printf("Requested memory: %d bytes\n", size);

            printf("Individual free block sizes:\n");

            for(int k = 0; k < *blockCount; k++)
            {
                if((*blocks)[k].status == 0)
                {
                    printf("Free Block: %d bytes\n", (*blocks)[k].size);
                }
            }
        }
        else
        {
            printf("Insufficient free memory.\n");
        }
        return;
    }
    int i = selectedBlockIndex;
    int oldStart = (*blocks)[i].start;
    int remainingSize = (*blocks)[i].size - size;
    
    // if there is remaining size, we need to create a new free block for the remaining memory
    if(remainingSize > 0)
    {
        if(!insertFreeBlock(blocks, blockCount, capacity, oldStart + size, remainingSize, i + 1))
        {
            printf("Failed to insert the remaining free block. allocation cancelled. \n");
            return;
        }
    }
    //update the blocks pointer and capacity
    (*blocks)[i].size = size;
    (*blocks)[i].allocatedId = *nextId;
    (*blocks)[i].status = 1;
    (*nextId)++;
    printf("Memory allocated successfully. Allocation ID: %d: \n", *nextId - 1);
    return;
}

void releaseMemory(struct memoryBlock **blocks, int *blockCount)
{
    int id;
    int found = 0;
    printf("Enter the allocation ID to release: ");
    if(scanf("%d" , &id) != 1)
    {
        printf("Invalid input. Please enter a valid integer for allocation ID. \n");
        return;
    }
    

    for(int i = 0; i < *blockCount; i++)
    {
        if((*blocks)[i].allocatedId == id && (*blocks)[i].status == 1)
        {
            found = 1;
            (*blocks)[i].status = 0; // mark the block as free
            (*blocks)[i].allocatedId = -1; // reset the allocated ID
            printf("Memory block with ID %d released successfully. \n", id);
            break;
        }
        else if((*blocks)[i].allocatedId == id && (*blocks)[i].status == 0)
        {
            printf("Memory block with ID %d is already free. \n", id);
            return;
        }
    }
    if(found == 0)
    {
        printf("No allocated block found with ID %d. \n", id);
        return;
    }
    //merging adjacent free blocks afterreleasing memory    
    mergeAdjacentBlocks(*blocks, blockCount);
}

bool insertFreeBlock(struct memoryBlock **blocks, int *blockCount, int *capacity, int start, int size, int insertIndex)
{
    if(insertIndex < 0 || insertIndex > *blockCount)
    {
        printf("Invalid insert index. Insertion cancelled. \n");
        return false;
    }

    if(*blockCount >= *capacity)
    {
        int newCapacity = (*capacity) * 2;
        struct memoryBlock *newBlocks = realloc(*blocks, newCapacity * sizeof(struct memoryBlock));

        if(newBlocks == NULL)
        {
            printf("Memory allocation for blocks failed. Insertion cancelled. \n");
            return false;
        }

        *blocks = newBlocks;
        *capacity = newCapacity;
    }

    for(int j = *blockCount; j > insertIndex; j--)
        {
            (*blocks)[j] = (*blocks)[j - 1];
        }
        (*blocks)[insertIndex].start = start;
        (*blocks)[insertIndex].size = size;
        (*blocks)[insertIndex].allocatedId = -1; // -1 indicates free block
        (*blocks)[insertIndex].status = 0;
        (*blockCount)++;

        return true;
}

void resizeAllocation(unsigned char  *memoryPool,struct memoryBlock **blocks, int *blockCount, int *capacity, int totalMemorySize, int strategy)
{
    int id, newSize;
    int found = 0;
    printf("Enter the allocation ID to resize: ");
    if(scanf("%d", &id) != 1)
    {
        printf("Invalid input. Please enter a valid integer for allocation ID.\n");
        return;
    }
    if(id <= 0)
    {
        printf("Invalid input, Please enter a valid ID\n");
        return;
    }

    printf("Enter the new size for the allocation: ");
    if(scanf("%d", &newSize) != 1)
    {
        printf("Invalid input. Please enter a valid integer for new size. \n");
        return;
    }

    if(newSize < 1 || newSize > totalMemorySize)
    {
        printf("Please enter a valid memory size between 1 and %d. \n", totalMemorySize);
        return;
    }

    for(int i = 0; i < *blockCount; i++)
    {
        if((*blocks)[i].allocatedId == id && (*blocks)[i].status == 1)
        {
            found = 1;
            int oldSize = (*blocks)[i].size;
            int oldStart = (*blocks)[i].start;

            if(newSize == oldSize)
            {
                printf("The new size is the same as the current size. No changes made. \n");
                return;
            }
            else if(newSize < oldSize)
            {
                int remainingSize = oldSize - newSize;
                if(!insertFreeBlock(blocks, blockCount, capacity, oldStart + newSize, remainingSize, i + 1))
                {
                    printf("Failed to insert the remaining free block. allocation cancelled. \n");
                    return;
                }
                (*blocks)[i].size = newSize;

                mergeAdjacentBlocks(*blocks, blockCount);

                printf("Allocation resize successfully. \n");
                return;
            }
            else
            {
                int extraSize = newSize - oldSize;
                if(i + 1 < *blockCount && (*blocks)[i+1].status == 0 && (*blocks)[i+1].size >= extraSize)
                {
                    (*blocks)[i].size = newSize;
                    (*blocks)[i + 1].size -= extraSize;
                    (*blocks)[i + 1].start += extraSize;

                    if((*blocks)[i + 1].size == 0)
                    { 
                        for(int j = i + 1; j < *blockCount - 1; j++)
                        {
                            (*blocks)[j] = (*blocks)[j + 1];
                        }
                        (*blockCount)--;
                    }
                    //merging
                    mergeAdjacentBlocks(*blocks, blockCount);

                    printf("Allocation resize successfully. \n");
                    return;
                }
                else
                {
                    printf("Expansion not possible in the adjacent free block. \n");

                    int destination = findSuitableBlock(*blocks, *blockCount, newSize, strategy, i);

                    if(destination == -1){
                        printf("No suitable free block available. \n");
                        return;
                    }
                    int oldStart = (*blocks)[i].start;
                    int oldSize = (*blocks)[i].size;
                    int newStart = (*blocks)[destination].start;

                    int remainingSize = (*blocks)[destination].size - newSize;
                    if(remainingSize > 0)
                    {
                        if(!insertFreeBlock(blocks, blockCount, capacity, newStart + newSize, remainingSize, destination + 1))
                        {
                            printf("Failed to insert the remaining free block. allocation cancelled. \n");
                            return;
                        }
                    }
                    
                    int oldIndex = -1;
                    for(int j = 0; j < *blockCount; j++)
                    {
                        if((*blocks)[j].allocatedId == id && (*blocks)[j].status == 1)
                        {
                            oldIndex = j;
                            break;
                        }
                    }
                    if(oldIndex == -1)
                    {
                        printf("Original allocation not found. resize cancelled. \n");
                        return;
                    }
                    memmove(memoryPool + newStart , memoryPool + oldStart, oldSize);

                    (*blocks)[destination].size = newSize;
                    (*blocks)[destination].allocatedId = id;
                    (*blocks)[destination].status = 1;
                    
                    (*blocks)[oldIndex].status = 0;
                    (*blocks)[oldIndex].allocatedId = -1; 

                    mergeAdjacentBlocks(*blocks, blockCount);
                    printf("Allocation ID %d resized successfully.\n", id);
                    return;
                }
            }
        }
    }
    if(found == 0)
    {
        printf("No allocated block found with ID %d. \n", id);
        return;
    }
}

void memoryMapDisplay(struct memoryBlock *blocks, int blockCount)
{
    printf("\n----------Memory Map Display------------\n");
    printf("total Blocks: %d\n", blockCount);

    for(int i = 0; i < blockCount; i++)
    {
        if(blocks[i].status == 1)
        { 
            printf("Allocated #%d: %d bytes, Start: %d\n",
            blocks[i].allocatedId,
            blocks[i].size,
            blocks[i].start);
        }
        else{
            printf("Free: %d bytes, start: %d\n", blocks[i].size, blocks[i].start);
        }
    }   
}

void displayMemoryStatics(struct memoryBlock *blocks, int blockCount, int totalMemorySize)
{
    int allocatedBytes = 0;
    int freeBytes = 0;

    int allocatedBlocks = 0;
    int freeBlocks = 0;

    int largeFreeBlock = 0;

    calculateFreeMemory(blocks, blockCount, &freeBytes, &largeFreeBlock);
    for(int i = 0; i < blockCount; i++)
    {
        if(blocks[i].status == 1)
        {
            allocatedBlocks++;
            allocatedBytes += blocks[i].size;
        }
        else
        {
            freeBlocks++;
        }
    }

    printf("\n----------Display Memory Statics------------\n");
    printf("Total Memory: %d bytes\n", totalMemorySize);
    printf("Allocated Memory: %d bytes\n", allocatedBytes);
    printf("Free Memory: %d bytes\n", freeBytes);
    printf("Allocated Blocks: %d\n", allocatedBlocks);
    printf("Free Blocks: %d\n", freeBlocks);
    printf("Largest Free Block: %d bytes\n", largeFreeBlock);
}

void changeAllocationStrategy(int *strategy)
{
    int newStrategy;
    printf("Select an New allocation strategy (1. First Fit, 2.Best Fit, 3. Worst Fit) \n");
    scanf("%d", &newStrategy);
    if(newStrategy < 1 || newStrategy > 3)
    {
        printf("Invalid strategy selection. \n");
        return;
    }

    if(newStrategy != *strategy){
        *strategy = newStrategy;
        printf("Strategy changed successfully.\n");
    }
    else{
        printf("This strategy is already selected.\n");
    }
    printf("Strategy selected: %s \n", newStrategy == 1 ? "First Fit" : (newStrategy == 2 ? "Best Fit" : "Worst Fit"));
}

void mergeAdjacentBlocks(struct memoryBlock *blocks, int *blockCount)
{
    for(int i = 0; i < *blockCount - 1; i++)
    {
        if(blocks[i].status == 0 && blocks[i + 1].status == 0)
        {
            blocks[i].size += blocks[i + 1].size;

            for(int j = i + 1; j < *blockCount - 1; j++)
            {
                blocks[j] = blocks[j + 1];
            }

            (*blockCount)--;
            i--;
        }
    }
}

void calculateFreeMemory(struct memoryBlock *blocks, int blockCount, int *totalFreeMemory, int *largestFreeBlock)
{
    *totalFreeMemory = 0;
    *largestFreeBlock = 0;

    for(int i = 0; i < blockCount; i++)
    {
        if(blocks[i].status == 0)
        {
            *totalFreeMemory += blocks[i].size;

            if(blocks[i].size > *largestFreeBlock)
            {
                *largestFreeBlock = blocks[i].size;
            }
        }
    }
}

int findSuitableBlock(struct memoryBlock *blocks, int blockCount, int size, int strategy, int excludeIndex)
{
    int selectedIndex = -1;

    int bestFitSize = -1;
    int worstFitSize = -1;

    for(int i = 0; i < blockCount; i++)
    {
        if(i == excludeIndex)
        {
            continue;
        }

        if(blocks[i].status == 0 && blocks[i].size >= size)
        {
            if(strategy == 1) // First Fit
            {
                return i;
            }
            else if(strategy == 2) // Best Fit
            {
                if(selectedIndex == -1 || blocks[i].size < bestFitSize)
                {
                    bestFitSize = blocks[i].size;
                    selectedIndex = i;
                }
            }
            else if(strategy == 3) // Worst Fit
            {
                if(blocks[i].size > worstFitSize)
                {
                    worstFitSize = blocks[i].size;
                    selectedIndex = i;
                }
            }
        }
    }

    return selectedIndex;
}