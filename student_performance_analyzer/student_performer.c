#include<stdio.h>
#include<stdlib.h>

struct student
{
    int rollNo;
    char name[500];
    int marks[3];
};

int calculateTotal(int marks[], int size);
float calculateAverage(int total, int size);
char calculateGrade(float average);
void performanceStars(char grade);
void printOutput(int rollNo, char name[], int total, float average, char grade);
void printRollno(int n);

int main()
{
    int n;
    printf("Enter the number of students: ");
    scanf("%d", &n);

    if(n <= 0 || n > 100)
    {
        printf("Invalid number of student. Please enter a number between 1 and 100 \n");
        return 0;
    }

    struct student *student = (struct student*) malloc(n*sizeof(struct student));

    if(student == NULL)
    {
        printf("Memory allocation failed \n");
        return 0;
    }

    for(int i = 0; i < n; i++)
    {
        printf("Enter the details of student %d (Roll Number, Name, Marks of 3 subjects): \n", i+1);
        scanf("%d %499s", &student[i].rollNo, student[i].name);

        for(int j = 0; j < 3; j++)
        {
            int result=scanf("%d", &student[i].marks[j]);
            if(result != 1)
            {
                printf("Invalid input. Please enter an integer value for marks: \n");
                while(getchar() != '\n');
                j--;
                continue;

            }
            while(student[i].marks[j] < 0 || student[i].marks[j] > 100)
            {
                printf("Invalid marks. please enter marks between 0 and 100: \n");
                scanf("%d", &student[i].marks[j]);
            }
        }
        
        int total = calculateTotal(student[i].marks, 3);

        float average = calculateAverage(total, 3);

        char grade = calculateGrade(average);

        printOutput(student[i].rollNo, student[i].name, total, average, grade);

        if(grade == 'F')
        {
           continue;
        }
        performanceStars(grade);
    }
    printf("list of Roll Number (via recursion): ");
    printRollno(n);

    free(student);
    return 0;
}

int calculateTotal(int marks[], int size)
{
    int total = 0;
    for(int i = 0; i < size; i++)
    {
        total += marks[i];
    }
    return total;
}

float calculateAverage(int total, int size)
{
    return (float)total / size;
}

char calculateGrade(float average)
{
    char grade;
    if(average >= 85)
    {
        grade = 'A';
    }
    else if(average >= 70)
    {
        grade = 'B';
    }
    else if(average >= 50)
    {
        grade = 'C';
    }
    else if(average >= 35)
    {
        grade = 'D';
    }
    else
    {
        grade = 'F';
    }
    return grade;    
}

void performanceStars(char grade)
{
    switch(grade)
    {
        case 'A':
            printf("Performance: *****\n");
            break;
        case 'B':
            printf("Performance: ****\n");
            break;
        case 'C':
            printf("Performance: ***\n");
            break;
        case 'D':
            printf("Performance: **\n");
            break;              
        }
}

void printOutput(int rollNo, char name[], int total, float average, char grade)
{
    printf("\n");
    printf("Roll: %d\n", rollNo);
    printf("Name: %s\n",  name);
    printf("Total: %d\n", total);
    printf("Average: %.2f\n", average);
    printf("Grade: %c\n", grade);
}

void printRollno(int n)
{
   if(n <= 0)
   {
      return;
   }

   printRollno(n-1);
   printf("%d ", n);

}