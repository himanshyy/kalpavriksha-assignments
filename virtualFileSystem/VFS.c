#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define NUM_BLOCK 1024

typedef struct freeBlock
{
    int blockIndex;
    struct freeBlock *prev;
    struct freeBlock *next;
} freeBlock;

typedef struct fileNode
{
    char name[51];
    int type; // 0 = FILE 1 = Directory
    struct fileNode *parent;
    struct fileNode *child;
    struct fileNode *next;
    struct fileNode *prev;
    int fileSize;
    int *blockPointer;

} fileNode;

int initializeFreeBlock(int numBlock, freeBlock **head, freeBlock **tail);
void freeList(freeBlock *head);
fileNode *createNode(char *name, int type);
void addChild(fileNode *parent, fileNode *newnode);
void childList(fileNode *directory);
fileNode *changeDirectory(fileNode *cwd, char *name);
void printWorkingDir(fileNode *cwd);
fileNode *findChild(fileNode *parent, char *name);
int allocateBlock(freeBlock **head, freeBlock **tail);
int writeFile(fileNode *file, unsigned char (*disk)[BLOCK_SIZE], freeBlock **freeHead, freeBlock **freeTail, char *data);
void readFile(fileNode *file, unsigned char (*disk)[BLOCK_SIZE]);
int addFreeBlock(freeBlock **head, freeBlock **tail, int *blockPointer, int count);
void removeChild(fileNode *parent, fileNode *target);
void deleteFile(fileNode *file, freeBlock **freeHead, freeBlock **freeTail);
void removeDirectory(fileNode *directory);
void diskUsage(freeBlock *freeHead);
void freeDirectoryTree(fileNode *directory);

int main()
{
    freeBlock *freeHead = NULL;
    freeBlock *freeTail = NULL;
    fileNode *cwd = NULL;
    char command[20];
    char name[51];

    printf("\n============== VIRTUAL FILE SYSTEM ===============\n");

    unsigned char (*disk)[BLOCK_SIZE] = malloc(NUM_BLOCK * sizeof(*disk));

    if (disk == NULL)
    {
        printf("Memory allocation failed");
        return 1;
    }
    else
    {
        printf("VFS initialized successfully!");
    }

    if (initializeFreeBlock(NUM_BLOCK, &freeHead, &freeTail) != 1)
    {
        printf("Initialization failed\n");
        freeList(freeHead);
        free(disk);
        return 1;
    }

    fileNode *root = createNode("/", 1);
    if (root == NULL)
    {
        freeList(freeHead);
        free(disk);
        return 1;
    }
    cwd = root;

    printf("\n============== AVAILABLE COMMANDS ==============\n\n");
    printf("mkdir name               Create a directory\n");
    printf("create name              Create a file\n");
    printf("write name \"content\"   Write data into a file\n");
    printf("read name                Read file contents\n");
    printf("delete name              Delete a file\n");
    printf("rmdir name               Remove an empty directory\n");
    printf("ls                       List files and directories\n");
    printf("cd name                  Change directory\n");
    printf("cd ..                    Move to parent directory\n");
    printf("cd /                     Move to root directory\n");
    printf("pwd                      Show current directory\n");
    printf("df                       Show disk usage\n");
    printf("exit                     Exit the program\n");

    printf("=================================================\n");
    while (1)
    {
        printf("\nVFS> ");
        if (scanf("%19s", command) != 1)
        {
            break;
        }
        if (strcmp(command, "exit") == 0)
        {
            printf("Exiting program\n");
            break;
        }
        if (strcmp(command, "mkdir") == 0)
        {
            if (scanf("%50s", name) != 1)
            {
                break;
            }
            if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 || strcmp(name, "/") == 0)
            {
                printf("Reserved name cannot be used.\n");
            }
            else if (findChild(cwd, name) != NULL)
            {
                printf("Name already exists\n");
            }
            else
            {
                fileNode *newnode = createNode(name, 1);
                if (newnode != NULL)
                {
                    addChild(cwd, newnode);
                    printf("Directory created successfully\n");
                }
            }
        }
        else if (strcmp(command, "create") == 0)
        {
            if (scanf("%50s", name) != 1)
            {
                break;
            }
            if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 || strcmp(name, "/") == 0)
            {
                printf("Reserved name cannot be used.\n");
            }
            else if (findChild(cwd, name) != NULL)
            {
                printf("Name already exists\n");
            }
            else
            {
                fileNode *newnode = createNode(name, 0);
                if (newnode != NULL)
                {
                    addChild(cwd, newnode);
                    printf("File created successfully\n");
                }
            }
        }
        else if (strcmp(command, "ls") == 0)
        {
            childList(cwd);
        }
        else if (strcmp(command, "cd") == 0)
        {
            if (scanf("%50s", name) != 1)
            {
                break;
            }
            fileNode *newdir = changeDirectory(cwd, name);
            if (newdir != NULL)
            {
                cwd = newdir;
            }
            else
            {
                printf("Directory not found\n");
            }
        }
        else if (strcmp(command, "pwd") == 0)
        {
            printWorkingDir(cwd);
        }
        else if (strcmp(command, "write") == 0)
        {
            char data[10000];
            if (scanf("%50s", name) != 1)
            {
                break;
            }
            int ch;

            while ((ch = getchar()) == ' ' || ch == '\t')
            {
            }

            if (ch != '"' && ch != '\'')
            {
                printf("Content must be inside quotes.\n");
                while (ch != '\n' && ch != EOF)
                {
                    ch = getchar();
                }
                continue;
            }

            char quote = ch;
            int i = 0;

            while ((ch = getchar()) != quote && ch != '\n' && ch != EOF)
            {
                if (i < (int)sizeof(data) - 1)
                {
                    data[i++] = (char)ch;
                }
                else
                {
                    printf("Input is too long.\n");
                    while (ch != '\n' && ch != EOF)
                    {
                        ch = getchar();
                    }
                    i = -1;
                    break;
                }
            }

            if (i == -1)
            {
                continue;
            }

            if (ch != quote)
            {
                printf("Missing closing quote.\n");
                continue;
            }

            data[i] = '\0';

            int extra = 0;

            while ((ch = getchar()) != '\n' && ch != EOF)
            {
                if (ch != ' ' && ch != '\t' && ch != '\r')
                {
                    extra = 1;
                }
            }

            if (extra)
            {
                printf("Invalid input after closing quote.\n");
                continue;
            }

            fileNode *temp = findChild(cwd, name);

            if (temp == NULL)
            {
                printf("File not found\n");
            }
            else if (temp->type != 0)
            {
                printf("Cannot write: It is a directory\n");
            }
            else
            {
                writeFile(temp, disk, &freeHead, &freeTail, data);
            }
        }
        else if (strcmp(command, "read") == 0)
        {
            if (scanf("%50s", name) != 1)
            {
                break;
            }
            fileNode *temp = findChild(cwd, name);

            if (temp == NULL)
            {
                printf("File not found\n");
            }
            else if (temp->type != 0)
            {
                printf("Cannot read: It is a directory\n");
            }
            else
            {
                readFile(temp, disk);
            }
        }
        else if (strcmp(command, "delete") == 0)
        {
            if (scanf("%50s", name) != 1)
            {
                break;
            }
            fileNode *temp = findChild(cwd, name);
            if (temp == NULL)
            {
                printf("File not found\n");
            }
            else if (temp->type != 0)
            {
                printf("Cannot delete: It is a directory. Use rmdir.\n");
            }
            else
            {
                deleteFile(temp, &freeHead, &freeTail);
            }
        }
        else if (strcmp(command, "rmdir") == 0)
        {
            if (scanf("%50s", name) != 1)
            {
                break;
            }
            fileNode *temp = findChild(cwd, name);
            if (temp == NULL)
            {
                printf("Directory not found\n");
            }
            else if (temp->type != 1)
            {
                printf("This is a file, not a directory\n");
            }
            else
            {
                removeDirectory(temp);
            }
        }
        else if (strcmp(command, "df") == 0)
        {
            diskUsage(freeHead);
        }
        else
        {
            printf("Invalid command \n");
        }
    }

    freeDirectoryTree(root);
    freeList(freeHead);
    free(disk);

    return 0;
}

int initializeFreeBlock(int numBlock, freeBlock **head, freeBlock **tail)
{

    for (int i = 0; i < numBlock; i++)
    {
        freeBlock *newnode = malloc(sizeof(freeBlock));
        if (newnode == NULL)
        {
            printf("Node memory Allocation Failed");
            return 0;
        }
        newnode->blockIndex = i;
        if (*head == NULL)
        {
            newnode->prev = NULL;
            newnode->next = NULL;
            *head = newnode;
            *tail = newnode;
        }
        else
        {
            newnode->prev = *tail;
            (*tail)->next = newnode;
            newnode->next = NULL;
            *tail = newnode;
        }
    }
    return 1;
}
void freeList(freeBlock *head)
{
    freeBlock *temp = head;
    while (temp != NULL)
    {
        freeBlock *nextp = temp->next;
        free(temp);
        temp = nextp;
    }
}

fileNode *createNode(char *name, int type)
{
    fileNode *newnode = malloc(sizeof(fileNode));
    if (newnode == NULL)
    {
        printf("Node memory allocation failed");
        return NULL;
    }
    strncpy(newnode->name, name, sizeof(newnode->name) - 1);
    newnode->name[sizeof(newnode->name) - 1] = '\0';
    newnode->type = type;
    newnode->parent = NULL;
    newnode->child = NULL;
    newnode->next = NULL;
    newnode->prev = NULL;
    newnode->fileSize = 0;
    newnode->blockPointer = NULL;

    return newnode;
}

void addChild(fileNode *parent, fileNode *newnode)
{
    if (parent->child == NULL)
    {
        parent->child = newnode;
        newnode->parent = parent;
        newnode->next = newnode;
        newnode->prev = newnode;
    }
    else
    {
        fileNode *temp = parent->child;
        while (temp->next != parent->child)
        {
            temp = temp->next;
        }
        temp->next = newnode;
        newnode->parent = parent;
        newnode->next = parent->child;
        newnode->prev = temp;
        parent->child->prev = newnode;
    }
}

void childList(fileNode *directory)
{
    fileNode *current = directory->child;
    if (directory->child == NULL)
    {
        printf("Directory is empty");
        return;
    }
    do
    {
        if (current->type == 1)
        {
            printf("%s/\n", current->name);
        }
        else
        {
            printf("%s \n", current->name);
        }
        current = current->next;
    } while (current != directory->child);
}

fileNode *changeDirectory(fileNode *cwd, char *name)
{
    if (strcmp(name, "..") == 0)
    {
        if (cwd->parent != NULL)
        {
            cwd = cwd->parent;
        }
        return cwd;
    }
    else if (strcmp(name, "/") == 0)
    {
        fileNode *temp = cwd;
        while (temp->parent != NULL)
        {
            temp = temp->parent;
        }
        cwd = temp;
        return cwd;
    }
    else
    {
        fileNode *temp = cwd->child;
        if (cwd->child == NULL)
        {
            printf("Current directory is empty\n");
            return NULL;
        }
        do
        {
            if (strcmp(temp->name, name) == 0 && temp->type == 1)
            {
                return temp;
            }
            temp = temp->next;
        } while (temp != cwd->child);
        return NULL;
    }
}

void printWorkingDir(fileNode *cwd)
{
    if (cwd == NULL)
    {
        printf("Current Directory is not available \n");
        return;
    }
    fileNode *temp = cwd;
    char **path = malloc(10 * sizeof(char *));
    if (path == NULL)
    {
        printf("Memory allocation failed");
        return;
    }
    int capacity = 10;
    int count = 0;
    while (temp->parent != NULL)
    {

        if (count == capacity)
        {
            int newCapacity = capacity * 2;
            char **tempPath = realloc(path, newCapacity * sizeof(char *));
            if (tempPath == NULL)
            {
                printf("Memory allocation failed");
                free(path);
                return;
            }
            path = tempPath;
            capacity = newCapacity;
        }
        path[count] = temp->name;
        count++;
        temp = temp->parent;
    }
    printf("/");

    for (int i = count - 1; i >= 0; i--)
    {
        printf("%s", path[i]);
        if (i != 0)
        {
            printf("/");
        }
    }
    printf("\n");
    free(path);
}

fileNode *findChild(fileNode *parent, char *name)
{
    if (parent->child == NULL)
    {
        return NULL;
    }
    fileNode *temp = parent->child;
    do
    {
        if (strcmp(temp->name, name) == 0)
        {
            return temp;
        }
        temp = temp->next;
    } while (temp != parent->child);
    return NULL;
}

int allocateBlock(freeBlock **head, freeBlock **tail)
{
    if (*head == NULL)
    {
        return -1;
    }

    freeBlock *temp = *head;
    int allocatedIndex = temp->blockIndex;

    *head = (*head)->next;

    if (*head == NULL)
    {
        *tail = NULL;
    }
    else
    {
        (*head)->prev = NULL;
    }
    free(temp);
    return allocatedIndex;
}

int writeFile(fileNode *file, unsigned char (*disk)[BLOCK_SIZE], freeBlock **freeHead, freeBlock **freeTail, char *data)
{

    int dataLen = strlen(data);

    int requiredBlock = (dataLen + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // Step 1: Count old blocks used by this file
    int oldBlockCount = (file->fileSize + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // Step 2: Count currently available blocks
    int availableBlock = 0;
    freeBlock *temp = *freeHead;

    while (temp != NULL)
    {
        availableBlock++;
        temp = temp->next;
    }

    // Old blocks will also become available after overwrite
    if (availableBlock + oldBlockCount < requiredBlock)
    {
        printf("Not enough free blocks\n");
        return 0;
    }

    // Step 3: Allocate new pointer array first
    int *newBlockPointer = NULL;

    if (requiredBlock > 0)
    {
        newBlockPointer = malloc(requiredBlock * sizeof(int));

        if (newBlockPointer == NULL)
        {
            printf("Memory allocation failed\n");
            return 0;
        }
    }

    // Step 4: Return old blocks to free-list tail
    if (!addFreeBlock(freeHead, freeTail, file->blockPointer, oldBlockCount))
    {
        free(newBlockPointer);
        return 0;
    }

    // Step 5: Free old pointer array
    free(file->blockPointer);

    // Step 6: Attach new pointer array
    file->blockPointer = newBlockPointer;

    int allocatedCount = 0;

    for (int i = 0; i < requiredBlock; i++)
    {
        int index = allocateBlock(freeHead, freeTail);

        if (index == -1)
        {
            printf("Block allocation failed\n");

            addFreeBlock(freeHead, freeTail, file->blockPointer, allocatedCount);

            free(file->blockPointer);
            file->blockPointer = NULL;
            file->fileSize = 0;

            return 0;
        }

        file->blockPointer[i] = index;
        allocatedCount++;
    }

    // Step 8: Copy data into allocated blocks
    for (int i = 0; i < requiredBlock; i++)
    {
        int offset = i * BLOCK_SIZE;
        int remaining = dataLen - offset;

        int chunkSize;

        if (remaining < BLOCK_SIZE)
            chunkSize = remaining;
        else
            chunkSize = BLOCK_SIZE;

        memcpy(disk[file->blockPointer[i]], data + offset, chunkSize);
    }

    file->fileSize = dataLen;

    printf("data length: %d\n", dataLen);
    printf("Required Blocks %d\n", requiredBlock);

    return 1;
}

void readFile(fileNode *file, unsigned char (*disk)[BLOCK_SIZE])
{
    if (file->fileSize == 0)
    {
        printf("File is empty\n");
        return;
    }
    int requiredBlock = (file->fileSize + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (int i = 0; i < requiredBlock; i++)
    {
        int offset = i * BLOCK_SIZE;
        int remaining = file->fileSize - offset;
        int chunkSize;

        if (remaining < BLOCK_SIZE)
        {
            chunkSize = remaining;
        }
        else
        {
            chunkSize = BLOCK_SIZE;
        }
        fwrite(disk[file->blockPointer[i]], 1, chunkSize, stdout);
    }
    printf("\n");
}

int addFreeBlock(freeBlock **head, freeBlock **tail,
                 int *blockPointer, int count)
{
    freeBlock *newHead = NULL;
    freeBlock *newTail = NULL;

    // Step 1: Allocate nodes for all blocks
    for (int i = 0; i < count; i++)
    {
        freeBlock *newnode = malloc(sizeof(freeBlock));

        if (newnode == NULL)
        {
            freeList(newHead);
            printf("Memory allocation failed\n");
            return 0;
        }

        newnode->blockIndex = blockPointer[i];
        newnode->prev = newTail;
        newnode->next = NULL;

        if (newHead == NULL)
        {
            newHead = newnode;
            newTail = newnode;
        }
        else
        {
            newTail->next = newnode;
            newTail = newnode;
        }
    }

    // Step 2: Attach all nodes at the free-list tail
    if (newHead != NULL)
    {
        if (*tail == NULL)
        {
            *head = newHead;
            *tail = newTail;
        }
        else
        {
            (*tail)->next = newHead;
            newHead->prev = *tail;
            *tail = newTail;
        }
    }

    return 1;
}
void removeChild(fileNode *parent, fileNode *target)
{
    // Only one child exists
    if (target->next == target)
    {
        parent->child = NULL;
    }
    else
    {
        target->prev->next = target->next;
        target->next->prev = target->prev;

        // If deleting the first child, update head
        if (parent->child == target)
        {
            parent->child = target->next;
        }
    }

    target->next = NULL;
    target->prev = NULL;
    target->parent = NULL;
}

void deleteFile(fileNode *file, freeBlock **freeHead, freeBlock **freeTail)
{
    fileNode *parent = file->parent;

    int requiredBlock = (file->fileSize + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // Return all allocated blocks to free list
    if (!addFreeBlock(freeHead, freeTail, file->blockPointer, requiredBlock))
    {
        return;
    }

    // Free the block pointer array
    free(file->blockPointer);

    // Remove file from parent's child list
    removeChild(parent, file);

    // Free the file node
    free(file);

    printf("File deleted successfully\n");
}

void removeDirectory(fileNode *directory)
{
    if (directory->child != NULL)
    {
        printf("Directory is not empty\n");
        return;
    }

    fileNode *parent = directory->parent;

    removeChild(parent, directory);

    free(directory);

    printf("Directory removed successfully\n");
}

void diskUsage(freeBlock *freeHead)
{
    int freeCount = 0;

    freeBlock *temp = freeHead;

    while (temp != NULL)
    {
        freeCount++;
        temp = temp->next;
    }

    int usedCount = NUM_BLOCK - freeCount;

    printf("Total blocks: %d\n", NUM_BLOCK);
    printf("Used blocks: %d\n", usedCount);
    printf("Free blocks: %d\n", freeCount);

    printf("Total disk space: %d bytes\n", NUM_BLOCK * BLOCK_SIZE);
    printf("Used disk space: %d bytes\n", usedCount * BLOCK_SIZE);
    printf("Free disk space: %d bytes\n", freeCount * BLOCK_SIZE);
    float usage = (usedCount * 100.0f) / NUM_BLOCK;
    printf("Disk Usage: %.2f%%\n", usage);
}

void freeDirectoryTree(fileNode *directory)
{
    if (directory == NULL)
    {
        return;
    }

    if (directory->child != NULL)
    {
        fileNode *first = directory->child;
        fileNode *current = first;

        while (current != NULL)
        {
            fileNode *nextNode = current->next;

            int isLast = (nextNode == first);

            if (current->type == 1)
            {
                freeDirectoryTree(current);
            }
            else
            {
                free(current->blockPointer);
                free(current);
            }

            if (isLast)
            {
                break;
            }

            current = nextNode;
        }
    }

    free(directory);
}