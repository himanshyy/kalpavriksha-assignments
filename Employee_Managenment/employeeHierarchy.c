#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct employee
{
    int id;
    char name[100];
    char designation[100];
    struct employee *manager;
    struct employee *next;
    struct employee *report;
    struct employee *nextReport;
};

struct employee *head;

void addEmployee(int id, char *name, char *designation, struct employee *manager);
struct employee *findEmployeeById(int id);
bool validEmployee(int id, struct employee *manager);
void displayHierarchy(struct employee *employee, int level);
int employeeCount(struct employee *employee);
void displayCompleteHierarchy(void);
void removeEmployee(int id);
int countDirectReport(struct employee *employee);

int main()
{
    int option;
    int id;
    char name[100];
    char designation[100];
    int managerId;

    while(1)
    {
        printf("-----Employee Hierarchy Managenment----------\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");
        scanf("%d", &option);

        switch(option)
        {
        case 1:
        {
            printf("Enter employee ID: \n");
            scanf("%d", &id);

            printf("Enter employee name: \n");
            scanf("%99s", name);

            printf("Enter Designation: \n");
            scanf("%99s", designation);

            printf("Enter ID of Reporting Manager (Enter 0 if dont have a reporting manager)\n");
            scanf("%d", &managerId);

            struct employee *manager = NULL;
        
            if(managerId != 0)
            {
                manager = findEmployeeById(managerId);
                if(managerId != 0 && manager == NULL)
                {
                    printf("Reporting manager doest not exit.\n");
                    break;
                }
            }
            addEmployee(id, name, designation, manager);
            break;
        }    
        case 2:
        {
            int employeeId;

            printf("Enter employee ID to remove: ");
            scanf("%d", &employeeId);

            removeEmployee(employeeId);
            break;
        }
        case 3:
        {
            int employeeId;
            printf("Enter employee ID: ");
            scanf("%d", &employeeId);
            
            struct employee *selectedEmployee = findEmployeeById(employeeId);
            
            if (selectedEmployee == NULL) 
            {
                printf("Employee not found.\n");
                break;
            }
            
            displayHierarchy(selectedEmployee, 0);
            printf("Total employee under this employee: %d\n", employeeCount(selectedEmployee));

            break;
        }
        case 4:
            displayCompleteHierarchy();
            break;

        case 5:
            return 0;

        default:
           printf("Invalid Choice\n");
           break;    
        }
    }
}

void addEmployee(int id, char *name, char *designation, struct employee *manager)
{
    if(!validEmployee(id, manager)) 
    {
        return;
    }

    struct employee *newUser = (struct employee*)malloc(sizeof(struct employee));
    if(newUser == NULL)
    {
        printf("Memory allocation failed\n");
        return;
    }

    newUser->id = id;
    strcpy(newUser->name, name);
    strcpy(newUser->designation, designation);

    newUser->manager = manager;
    newUser->next = NULL;
    newUser->report = NULL;
    newUser->nextReport = NULL;

    if(head == NULL)
    {
        head = newUser;
    }
    else
    {
        struct employee *current = head;
        while(current->next != NULL)
        {
            current = current->next;
        }
        current->next = newUser;
    }

    if(manager != NULL)
    {
        if(manager->report == NULL)
        {
            manager->report = newUser;
        }
        else
        {
            struct employee *currentReport = manager->report; 
            while(currentReport->nextReport != NULL)
            {
                currentReport = currentReport->nextReport;
            }
            currentReport->nextReport = newUser;
        }
    }
    printf("Employee added successfully\n\n");
}

struct employee *findEmployeeById(int id)
{
    struct employee *current = head;
    while(current != NULL)
    {
        if(current->id == id)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

bool validEmployee(int id, struct employee *manager){
    if(findEmployeeById(id) != NULL)
    {
        printf("Employee ID already exists\n\n");
        return false;
    }
    if(manager != NULL)
    {
        if(manager->id == id)
        {
            printf("Self reporting is not allowed\n");
            return false;
        }
    }
    return true;
}

void displayHierarchy(struct employee *employee, int level)
{
    if(employee == NULL) 
    {
        return;
    }

    for(int i = 0; i < level; i++)
    {
        printf("   ");
    }
    printf("ID: %d, Name: %s, designation: %s\n", employee->id, employee->name, employee->designation);

    struct employee *current = employee->report;

    while(current != NULL)
    {
        displayHierarchy(current, level + 1);
        current = current->nextReport;
    }
}

int employeeCount(struct employee *employee)
{
    if(employee == NULL)
    {
        return 0;
    }

    int count = 0;
    struct employee *current = employee->report;

    while (current != NULL) 
    {
        count += 1 + employeeCount(current);
        current = current->nextReport;
    }
    return count;
}

void displayCompleteHierarchy(void)
{
    struct employee *current=head;
    int total = 0;

    if(head == NULL)
    {
        printf("No employees found \n");
        return;
    }

    while(current != NULL)
    {
        if(current->manager == NULL)
        {
            displayHierarchy(current, 0);

            total += 1 + employeeCount(current);
        }
        current = current->next;
    }
    printf("Total employees: %d\n\n",total);
}

void removeEmployee(int id)
{
    struct employee *target = findEmployeeById(id);

    if(target == NULL)
    {
        printf("Employee not found.\n");
        return;
    }
    if(target->manager == NULL)
    {
        printf("Top-level employee cannot be removed.\n\n");
        return;
    }

    struct employee *manager = target->manager;
    struct employee *targetNextReport = target->nextReport;
    struct employee *previous = NULL;
    struct employee *currentReport = manager->report;
    
    while(currentReport != NULL && currentReport != target) 
    {
        previous = currentReport;
        currentReport = currentReport->nextReport;
    }
    if(currentReport == target) 
    {
        if(previous == NULL) 
        {
            manager->report = targetNextReport;
        } 
        else 
        {
            previous->nextReport = targetNextReport;
        }
    }
    if(target->report != NULL)
    {
        struct employee *current = manager->report;
        struct employee *minReportEmployee = NULL;

        while(current != NULL)
        {
                int currentcount = countDirectReport(current);
                if(minReportEmployee == NULL || currentcount < countDirectReport(minReportEmployee))
                {
                    minReportEmployee = current;
                }
            current = current->nextReport;
        }

        struct employee *newManager;

        if(minReportEmployee != NULL)
        {
            newManager = minReportEmployee;
        }
        else
        {
            newManager = manager;
        }

        struct employee *firstReport = target->report;
        struct employee *lastReport = firstReport;
        while(lastReport->nextReport != NULL) 
        {
            lastReport = lastReport->nextReport;
        }
        if(newManager->report == NULL) 
        {
            newManager->report = firstReport;
        } 
        else
        {
            struct employee *currentReport = newManager->report;
            
            while(currentReport->nextReport != NULL) 
            {
                currentReport = currentReport->nextReport;
            }
            currentReport->nextReport = firstReport;
        }
        lastReport->nextReport = NULL;

        struct employee *child = target->report;
        while(child != NULL)
        {
            child->manager = newManager;
            child = child->nextReport;
        }
    }
    struct employee *previousEmployee = NULL;
    struct employee *currentEmployee = head;
    
    while(currentEmployee != NULL && currentEmployee != target) 
    {
        previousEmployee = currentEmployee;
        currentEmployee = currentEmployee->next;
    }
    
    if(currentEmployee == target) 
    {
        if (previousEmployee == NULL) 
        {
            head = target->next;
        } 
        else 
        {
            previousEmployee->next = target->next;
        }
    }
    free(target);
    printf("Employee removed successfully.\n\n");
}

int countDirectReport(struct employee *employee)
{
    if(employee == NULL)
    {
        return 0;
    }
    int count = 0;
    struct employee *current = employee->report;
    while(current != NULL)
    {
        count++;
        current = current->nextReport;
    }
    return count;
}