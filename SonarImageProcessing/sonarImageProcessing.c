#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int **generateMatrix(int size);
void freeMatrix(int **matrix, int cells);
void showMatrix(int **matrix, int size);
void rotateMatrix(int **matrix, int size);
void swapPointer(int **first, int **second);
int smoothing(int **matrix, int size);

int main()
{
    int size;
    printf("Enter Matrix Size (2-10):  ");
    if(scanf("%d", &size) != 1)
    {
        printf("Invalid input \n");
        return 1;
    }

    if(size < 2 || size > 10)
    {
        printf("Please enter a valid size ( 2-10)");
        return 1;
    }

    srand(time(NULL));
    int **matrix = generateMatrix(size);

    if(matrix == NULL)
    {
        printf("Memory allocation failed");
        return 1;
    }

    printf("\n-----------------------Original Matrix----------------------------\n\n");
    showMatrix(matrix, size);

    printf("\n-----------------------Rotated Matrix----------------------------\n\n");
    rotateMatrix(matrix, size);
    showMatrix(matrix, size);

    if(smoothing(matrix, size))
    {
        printf("\n-----------------------Smoothing Matrix----------------------------\n\n");
        showMatrix(matrix, size);
    }
    else
    {
        printf("Memory allocation failed during smoothing.\n");
    }

    freeMatrix(matrix , size * size);

    return 0;
}

// Allocates memory for the pointer table and generates random matrix values.
int **generateMatrix(int size)
{
    // Allocate memory for N*N pointer slots.
    int **matrix = malloc(size * size * sizeof(int*));

    if(matrix == NULL)
    {
        return NULL;
    }

    for(int i = 0; i < size * size; i++)
    {
        // Allocate memory for each individual matrix element.
        *(matrix + i) = malloc(sizeof(int));

        if(*(matrix + i) == NULL)
        {
            // Frees each allocated integer and then the pointer table.
            freeMatrix(matrix, i);
            return NULL;
        }

        // Generate a random intensity value between 0 and 255.
        **(matrix + i) = rand() % 256;
    }

    return matrix;
}

void freeMatrix(int **matrix, int cells)
{
    if(matrix == NULL)
    {
        return;
    }

    for(int i = 0; i < cells; i++)
    {
        free(*(matrix + i));
    }

    free(matrix);
}

// Displays the matrix in N rows and N columns.
void showMatrix(int **matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            // Calculate the pointer table index for the current row and column.
            int index = i * size + j;
            printf("%d  ", **(matrix + index)); 
        }
        printf("\n");
    }
}

// Swaps two pointer addresses without swapping the actual integer values.
void swapPointer(int **first, int **second)
{
    int *temp = *first;
    *first = *second;
    *second = temp;
}

// Transpose the matrix by swapping pointers across the diagonal.
void rotateMatrix(int **matrix, int size)
{
    //tranpose 
    for(int i = 0; i < size; i++)
    {
        for(int j = i + 1; j < size; j++)
        {
            int first = i * size + j;
            int second = j * size + i;
            swapPointer(matrix + first, matrix + second);
        }
    }

    //reverse
    for(int i = 0; i < size; i++)
    {
        int left = 0;
        int right = size - 1;
        while(left < right)
        {
            int first = i * size + left;
            int second = i * size + right;
            swapPointer(matrix + first, matrix + second);
            left++;
            right--;
        }
    }
}

int smoothing(int **matrix, int size)
{
    // Allocate three row buffers to preserve original values during smoothing.
    int *previousRow = malloc(size * sizeof(int));
    int *currentRow = malloc(size * sizeof(int));
    int *nextRow = malloc(size * sizeof(int));

    if(previousRow == NULL || currentRow == NULL || nextRow == NULL){
        free(previousRow);
        free(currentRow);
        free(nextRow);
        return 0;
    }

    for(int col = 0; col < size; col++)
    {
        // Load the first two rows into the current and next row buffers.
        *(currentRow + col) = **(matrix + col);
        *(nextRow + col) = **(matrix + size + col);
    }

    // Calculate the sum and count of valid neighboring cells in the 3x3 window.
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            int sum = 0; 
            int count = 0;

            for(int col = j - 1; col <= j + 1; col++)
                {
                    if(col >= 0 && col < size)
                    {
                        if(i > 0)
                        {
                            sum += *(previousRow + col);
                            count++;
                        }
                        sum += *(currentRow + col);
                        count++;

                        if(i + 1 < size){
                            sum += *(nextRow + col);
                            count++;
                        }
                    }
                }
            int position = i * size + j;
            int average = sum / count;
            **(matrix + position) = average; 
        }
        // Load the following original row into the next row buffer, if available.
        if(i + 1 < size)
        {
            for(int col = 0; col < size; col++)
            {
                *(previousRow + col) = *(currentRow + col);
                *(currentRow + col) = *(nextRow + col);
            }
            if(i + 2 < size)
            {
                for(int col = 0; col < size; col++){
                    int position = (i + 2) * size + col;
                    *(nextRow + col) = **(matrix + position);
                }
            }
        }
    }
    
    free(previousRow);
    free(currentRow);
    free(nextRow);

    return 1;
}