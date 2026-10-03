#include<stdio.h>
#include<string.h>

struct Data
{
    int id;
    char name[100];
    int age;
};
//function
void addUser();
void readUser();
void updateUser();
void deleteUser();

//Main Program
int main()
{
    while(1)
    {
        printf("choose an option:\n1. Add a new user\n2. Read the data\n3. Update the data\n4. Delete the data\n5. Exit\n");
        printf("Enter your choice: ");

        int choice;
        scanf("%d",&choice);
        
        if(choice == 1)
        {
        //create the file
           addUser();
        }
        //Read a file 
        else if(choice == 2)
        {
            readUser();
        }
        //update the file 
        else if(choice == 3)
        {
            updateUser();
        }
        //delete the file
        else if(choice == 4)
        {
            deleteUser();
        }
        else if(choice == 5)
        {
            break;
        }
        else
        {
            printf("Invalid choice. Please Try again.\n");
        }
}

    return 0;
}
void addUser()
{
    struct Data d1;
    int checkID;
    int found=0;
    FILE *fptr;

    fptr=fopen("users.txt","r");

    printf("Enter ID: ");
    scanf("%d", &checkID);

    if(fptr != NULL)
    {
        while(fscanf(fptr,"%d|%[^|]|%d", &d1.id, d1.name, &d1.age) == 3)
        {
            if(d1.id == checkID)
            {
            found = 1;
        }
    }
    fclose(fptr);
}
    if(found == 1)
    {
        printf("ID already exists\n");
    }
    if(found == 0)
    {
        fptr=fopen("users.txt", "a");
        if(fptr != NULL)
        {
            printf("Enter Name: ");
            getchar();
            fgets(d1.name, sizeof(d1.name), stdin);
            d1.name[strcspn(d1.name, "\n")] = '\0';
            
            printf("Enter Age: ");
            scanf("%d", &d1.age);
            
            fprintf(fptr,"%d|%s|%d\n", checkID, d1.name, d1.age);
            fclose(fptr);
            printf("User added Successfully\n");
        }
        else
        {
            printf("Error: Could not open file\n");
        }
    }


}
void readUser()
{
    struct Data d1;
    FILE *fptr;

    fptr = fopen("users.txt", "r");
    if(fptr != NULL)
    {
        while(fscanf(fptr, "%d|%[^|]|%d", &d1.id, d1.name, &d1.age) == 3)
        {
            printf("%d %s %d\n", d1.id, d1.name, d1.age);
        }
        fclose(fptr);
    }
    else
    {
        printf("Error: Could not open file\n");
    }

}
void updateUser()
{
    struct Data d1;
    int id;
    int found = 0;

    FILE *fptr;
    FILE *temp;

    fptr = fopen("users.txt", "r");
    temp = fopen("temp.txt", "w");
    printf("Enter ID to Update the record: ");
    scanf("%d", &id);

    if(fptr != NULL && temp != NULL)
    {
        while(fscanf(fptr, "%d|%[^|]|%d", &d1.id, d1.name, &d1.age) == 3)
        {
            if(d1.id == id)
            {
                found = 1;
                printf("New Name: ");
                getchar();
                fgets(d1.name, sizeof(d1.name), stdin);
                d1.name[strcspn(d1.name, "\n")] = '\0';
                
                printf("New Age: ");
                scanf("%d", &d1.age);
            }
            fprintf(temp, "%d|%s|%d\n", d1.id, d1.name, d1.age);
        }
        if(found == 1)
        {
            printf("Successfull\n");
            fclose(fptr);
            fclose(temp);
            remove("users.txt");
            rename("temp.txt", "users.txt");
        }
        else
        {
            printf("ID doesn't exists\n");
            fclose(fptr);
            fclose(temp);
            remove("temp.txt");
        }
    }
    else
    {
        printf("Error: Could not open file\n");
}

}
void deleteUser()
{

    struct Data d1;

    FILE *fptr;
    FILE *temp;

    fptr = fopen("users.txt", "r");
    temp = fopen("temp.txt", "w");

    int idCheck;
    int found = 0;

    printf("Enter ID you want to delete: ");
    scanf("%d", &idCheck);

    if(fptr != NULL && temp != NULL)
    {
        while(fscanf(fptr, "%d|%[^|]|%d", &d1.id, d1.name, &d1.age) == 3)
        {
            if(d1.id != idCheck)
            {
                fprintf(temp, "%d|%s|%d\n", d1.id, d1.name, d1.age);
            }
            else
            {
                found=1;
            }
        }
        if(found == 1)
        {
            printf("Successfull\n");
            fclose(fptr);
            fclose(temp);
            remove("users.txt");
            rename("temp.txt","users.txt");
        }
        else
        {
            printf("ID doesn't exists\n");
            fclose(fptr);
            fclose(temp);
            remove("temp.txt");
        }
    }
    else
    {
        printf("Error: Could not open file\n");
    }
}


