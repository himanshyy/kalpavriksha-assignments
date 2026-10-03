#include<stdio.h>

int checkExpression(char *expression);
void calculate(char *expression);
void reduce(int *number, char *operator, int *intCount, int *operatorCount, int i);

int main()
{
    char expression[100];
    printf("write a expression to calculate : ");
    fgets(expression, sizeof(expression), stdin);
    if(checkExpression(expression) == 1) 
    {
        calculate(expression);
    }
    return 0;
}
void calculate(char *expression)
{
    char operator[50];
    int num = 0;
    int number[50];
    int intCount = 0;
    int operatorCount = 0;

    for(int i = 0; expression[i] != '\0'; i++)
    {
        if(expression[i] == ' ')
        {
            continue;
        }
            //number check
        if(expression[i] >= '0' && expression[i] <= '9')
        {
            while(expression[i] >= '0' && expression[i] <= '9')
            {
                num = num * 10 + (expression[i] - '0');
                i++;
            }
            number[intCount] = num;
            intCount++;
            i--;
        }
        else if(expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/')
        {
            operator[operatorCount] = expression[i];
            operatorCount++;
            num = 0;
        }
           
    }
    //firstly we will calculate the multiplication and division        
    for(int i = 0; i < operatorCount; i++)
    {
        if(operator[i] == '*')
        {
            int result = number[i] * number[i+1];
            number[i] = result;
            reduce(number, operator, &intCount, &operatorCount, i);
            i--;
        }
        else if(operator[i] == '/')
        {
            if(number[i+1] == 0)
            {
                printf("Error: Division by zero.\n");
                return;
            }
            int result = number[i] / number[i+1];
            number[i] = result;
            reduce(number, operator, &intCount, &operatorCount, i);
            i--;
        }
    }
    //then we will calculate the addition and subtraction
    for(int i = 0; i < operatorCount; i++)
    {
        if(operator[i] == '+')
        {
            int result = number[i] + number[i+1];
            number[i] = result;
            reduce(number, operator, &intCount, &operatorCount, i);
            i--;
        }
        else if(operator[i] == '-')
        {
            int result = number[i] - number[i+1];
            number[i] = result;
            reduce(number, operator, &intCount, &operatorCount, i);
            i--;
        }
    }
    printf("Result: %d\n",number[0]); 
}

void reduce(int *number, char *operator, int *intCount, int *operatorCount, int i)
{
    //  reducing the expression
        for(int j=i+1; j < *intCount-1; j++)
        {
            number[j] = number[j+1];
        }
        (*intCount)--;
        for(int j=i; j < *operatorCount-1; j++)
        {
            operator[j] = operator[j+1];
        }
        (*operatorCount)--;
}

int checkExpression(char *expression)
{
    int valid = 1;
    int seenNumber = 0;
    int spaces = 0;

    for(int i = 0; expression[i] != '\0'; i++)
    {
        if(!(expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/'|| expression[i] == '\n' ||expression[i] == ' '|| (expression[i] >= '0' && expression[i] <= '9')))
        {
            valid = 0;
            break;
        }
        // check if the expression starts with an operator
        if(expression[i] >= '0' && expression[i] <= '9')
        {
            if(spaces == 1)
            {
                valid = 0;
                break;
            } 
            seenNumber = 1;
        }
        else if(expression[i] == ' ')
        {
            if(seenNumber == 1)
            {
                spaces = 1;
            }
        }
        else if(expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/')
        {
            if(seenNumber == 0)
            {
                valid = 0;
                break;
            }
            seenNumber = 0;
            spaces = 0;
        }
    }
    if(seenNumber == 0)
    {
        valid = 0;
    }
    if(valid == 0)
    {
        printf("Error: Invalid expression.\n");
        return 0;
    }
    else
    {
        return 1;
    }
}