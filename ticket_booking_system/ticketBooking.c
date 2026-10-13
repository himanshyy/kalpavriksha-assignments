#include <stdio.h>
#include <string.h>

int isValidCategory(int choice);
int isValidTicketCount(int numberOfTickets);
void printOutput(char name[], int numberOfTickets, char category[], float subTotal, float discount, float finalPrice);

int main()
{
    char name[100];
    char category[50];

    int numberOfTickets;
    int choice;
    int ticketPrice;

    const int standardPrice = 150;
    const int premiumPrice = 250;
    const int reclinerPrice = 400;

    printf("-----------Movie Ticket Booking System------------\n");

    // Get customer details

    printf("Enter Customer Name: \n");
    scanf("%99s", name);

    while (1)
    {
        printf("Enter number of tickets (1-20): \n");
        scanf("%d", &numberOfTickets);

        int checkTicket = isValidTicketCount(numberOfTickets);
        if (checkTicket == 1)
        {
            break;
        }
    }

    printf("Available Ticket Category: \n");
    printf("1. Standard - Rs. 150\n");
    printf("2. Premium - Rs. 250\n");
    printf("3. Recliner - Rs. 400\n");

    while (1)
    {
        printf("Enter your choice (1-3): \n");
        scanf("%d", &choice);

        int checkCategory = isValidCategory(choice);
        if (checkCategory == 1)
        {
            break;
        }
    }

    // calculate ticket price based on category

    switch (choice)
    {
        case 1:
            strcpy(category, "Standard");
            ticketPrice = standardPrice;
            break;
        case 2:
            strcpy(category, "Premium");
            ticketPrice = premiumPrice;
            break;
        case 3:
            strcpy(category, "Recliner");
            ticketPrice = reclinerPrice;
            break;
    }

    float subTotal = numberOfTickets * ticketPrice;

    float discount = 0;

    if (numberOfTickets >= 5)
    {
        discount = subTotal * 0.1;
    }
    else
    {
        discount = 0;
    }

    float finalPrice = subTotal - discount;

    printOutput(name, numberOfTickets, category, subTotal, discount, finalPrice);

    return 0;
}

// function to validate category choice

int isValidCategory(int choice)
{
    int valid = 1;

    if (choice < 1 || choice > 3)
    {
        printf("Invalid category choice! Please enter a number between 1 and 3. \n");
        valid = 0;
    }
    return valid;
}

//function to validate ticket count

int isValidTicketCount(int numberOfTickets)
{
    int valid = 1;
    if (numberOfTickets < 1 || numberOfTickets > 20)
    {
        printf("Invalid ticket count! Please enter a number between 1 and 20. \n");
        valid = 0;
    }
    return valid;
}

// function to print the output 

void printOutput(char name[], int numberOfTickets, char category[], float subTotal, float discount, float finalPrice)
{
    printf("---------Ticket details are:------------ \n");
    printf("Customer: %s\n", name);
    printf("Ticket Category: %s\n", category);
    printf("Tickets: %d\n", numberOfTickets);
    printf("Subtotal: %.2f\n", subTotal);
    printf("Discount: %.2f\n", discount);
    printf("Final Bill: %.2f\n", finalPrice);
}