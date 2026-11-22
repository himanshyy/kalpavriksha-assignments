#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Players_data.h"
#include <stdbool.h>

typedef struct myPlayer
{
    int playerId;
    char name[100];
    char teamName[100];
    char role[100];
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
    struct myPlayer *next;
} myPlayer;

typedef struct teamDetail
{
    int teamId;
    char teamName[100];
    int totalPlayers;
    float averageStrikeRate;

    struct myPlayer *heads[3];

} teamDetail;

typedef struct ResultNode
{
    myPlayer *player;
    struct ResultNode *next;
} ResultNode;

int findTeamIndex(teamDetail *teamList, int teamId);
void loadInitialPlayers(teamDetail *teamList);
void initializeTeams(teamDetail *teamList, const char **teams);
float calculatePI(myPlayer *player);
void insertPlayer(int index, myPlayer *player, teamDetail *team);
void calculateTeamAverage(teamDetail *team);
int findingTeamId(teamDetail *team, const char *teamName);
bool playerExists(int playerID, teamDetail *team);
void addPlayerToTeam(teamDetail *teamList);
void displayTeamPlayers(teamDetail *teamList);
void displayTeamSorted(teamDetail *teamList);
void displayTopKPlayers(teamDetail *teamList);
int getRoleIndex(char *role);
void displayAllPlayersByRole(teamDetail *teamList);
ResultNode *mergeResultLists(ResultNode *list1, ResultNode *list2);
void freeAllPlayers(teamDetail *teamList);
void clearInputBuffer();

int main()
{
    teamDetail teamList[10];
    int choice;

    initializeTeams(teamList, teams);
    loadInitialPlayers(teamList);

    for (int i = 0; i < 10; i++)
    {
        calculateTeamAverage(&teamList[i]);
    }
    // menu
    printf("\n========== ICC ODI PLAYER PERFORMANCE ANALYZER ==========\n");

    while (1)
    {
        printf("1. Add Player to Team\n");
        printf("2. Display All Players of a Team\n");
        printf("3. Display Teams Sorted by Average Strike Rate\n");
        printf("4. Display Top K Players of a Team by Role\n");
        printf("5. Display All Players of a Selected Role\n");
        printf("6. Exit\n");

        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input! Please enter a number from 1 to 6.\n");
            clearInputBuffer();
            continue;
        }

        clearInputBuffer();

        switch (choice)
        {
        case 1:
            addPlayerToTeam(teamList);
            break;
        case 2:
            displayTeamPlayers(teamList);
            break;
        case 3:
            displayTeamSorted(teamList);
            break;
        case 4:
            displayTopKPlayers(teamList);
            break;
        case 5:
            displayAllPlayersByRole(teamList);
            break;
        case 6:
            freeAllPlayers(teamList);
            printf("Exiting Program\n");
            return 0;
        default:
            printf("Invalid choice. Please select 1 to 6.\n");
            break;
        }
    }
    return 0;
}

void initializeTeams(teamDetail *teamList, const char **teams)
{
    for (int i = 0; i < 10; i++)
    {
        teamList[i].teamId = i + 1;
        strcpy(teamList[i].teamName, teams[i]);
        teamList[i].totalPlayers = 0;
        teamList[i].averageStrikeRate = 0;

        for (int j = 0; j < 3; j++)
        {
            teamList[i].heads[j] = NULL;
        }
    }
}

void loadInitialPlayers(teamDetail *teamList)
{
    for (int i = 0; i < playerCount; i++)
    {
        myPlayer *player = malloc(sizeof(myPlayer));

        if (player == NULL)
        {
            printf("Memory allocation failed\n");
            return;
        }

        player->playerId = players[i].id;
        strcpy(player->name, players[i].name);
        strcpy(player->teamName, players[i].team);
        strcpy(player->role, players[i].role);
        player->totalRuns = players[i].totalRuns;
        player->battingAverage = players[i].battingAverage;
        player->strikeRate = players[i].strikeRate;
        player->wickets = players[i].wickets;
        player->economyRate = players[i].economyRate;
        player->next = NULL;

        player->performanceIndex = calculatePI(player);

        int teamId = findingTeamId(teamList, player->teamName);

        if (teamId == -1)
        {
            printf("Team not found\n");
            free(player);
            continue;
        }

        int index = findTeamIndex(teamList, teamId);

        if (index == -1)
        {
            free(player);
            continue;
        }
        insertPlayer(index, player, teamList);
    }
}

int findTeamIndex(teamDetail *teamList, int teamId)
{
    int low = 0;
    int high = 9;

    while (low <= high)
    {
        int mid = (low + high) / 2;
        if (teamList[mid].teamId == teamId)
        {
            return mid;
        }
        else if (teamList[mid].teamId < teamId)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }
    return -1;
}

void insertPlayer(int index, myPlayer *player, teamDetail *team)
{
    player->next = NULL;

    int roleIndex = getRoleIndex(player->role);

    if (roleIndex == -1)
    {
        free(player);
        return;
    }

    myPlayer **head = &team[index].heads[roleIndex];

    if (*head == NULL || player->performanceIndex > (*head)->performanceIndex)
    {
        player->next = *head;
        *head = player;
    }
    else
    {
        myPlayer *temp = *head;

        while (temp->next != NULL && temp->next->performanceIndex >= player->performanceIndex)
        {
            temp = temp->next;
        }

        player->next = temp->next;
        temp->next = player;
    }

    team[index].totalPlayers++;
}

float calculatePI(myPlayer *player)
{
    float PI = 0;
    if (strcmp(player->role, "Batsman") == 0)
    {
        PI = (player->battingAverage * player->strikeRate) / 100;
    }
    else if (strcmp(player->role, "Bowler") == 0)
    {
        PI = (player->wickets * 2) + (100 - player->economyRate);
    }
    else if (strcmp(player->role, "All-rounder") == 0)
    {
        PI = ((player->battingAverage * player->strikeRate) / 100) + (player->wickets * 2);
    }
    else
    {
        printf("Invalid role\n");
        return -1;
    }
    return PI;
}

int findingTeamId(teamDetail *team, const char *teamName)
{
    for (int i = 0; i < 10; i++)
    {
        if (strcmp(team[i].teamName, teamName) == 0)
        {
            return team[i].teamId;
        }
    }
    return -1;
}

void calculateTeamAverage(teamDetail *team)
{
    float sum = 0;
    int count = 0;

    // Batsman list (heads[0])
    myPlayer *temp = team->heads[0];

    while (temp != NULL)
    {
        sum += temp->strikeRate;
        count++;
        temp = temp->next;
    }

    // All-rounder list (heads[2])
    temp = team->heads[2];

    while (temp != NULL)
    {
        sum += temp->strikeRate;
        count++;
        temp = temp->next;
    }

    if (count > 0)
    {
        team->averageStrikeRate = sum / count;
    }
    else
    {
        team->averageStrikeRate = 0;
    }
}

bool playerExists(int playerID, teamDetail *team)
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            myPlayer *temp = team[i].heads[j];

            while (temp != NULL)
            {
                if (temp->playerId == playerID)
                {
                    return true;
                }

                temp = temp->next;
            }
        }
    }
    return false;
}

void addPlayerToTeam(teamDetail *teamList)
{
    int teamID;
    int playerID;
    char role[100];
    float battingAvg;
    int totalRuns;
    float strikeRate;
    int wickets;
    float economyRate;
    char playerName[100];

    printf("Enter team ID to add player: ");

    if (scanf("%d", &teamID) != 1)
    {
        printf("Invalid input! Please enter a numeric Team ID.\n");
        clearInputBuffer();
        return;
    }

    clearInputBuffer();

    int index = findTeamIndex(teamList, teamID);

    if (index == -1)
    {
        printf("Invalid Team ID\n");
        return;
    }

    if (teamList[index].totalPlayers >= 50)
    {
        printf("Team is full\n");
        return;
    }

    printf("\nEnter Player Details:");

    printf("\nPlayer ID: ");

    if (scanf("%d", &playerID) != 1)
    {
        printf("Invalid Player ID. Enter a number.\n");
        clearInputBuffer();
        return;
    }

    clearInputBuffer();

    if (playerID < 1 || playerID > 1000)
    {
        printf("Player ID must be between 1 and 1000\n");
        return;
    }

    if (playerExists(playerID, teamList))
    {
        printf("Player ID already exists\n");
        return;
    }

    printf("\nName: ");
    scanf(" %99[^\n]", playerName);

    printf("\nRole (Batsman, Bowler, All-rounder): ");
    scanf(" %99s", role);

    if (strcmp(role, "Batsman") != 0 && strcmp(role, "Bowler") != 0 && strcmp(role, "All-rounder") != 0)
    {
        printf("Invalid role\n");
        return;
    }

    printf("\nTotal Runs: ");

    if (scanf("%d", &totalRuns) != 1)
    {
        printf("Invalid Total Runs.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    printf("\nBatting Average: ");

    if (scanf("%f", &battingAvg) != 1)
    {
        printf("Invalid Batting Average.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    printf("\nStrike Rate: ");
    scanf("%f", &strikeRate);

    printf("\nWickets: ");
    scanf("%d", &wickets);

    printf("\nEconomy Rate: ");
    scanf("%f", &economyRate);

    myPlayer *player = malloc(sizeof(myPlayer));

    if (player == NULL)
    {
        printf("Memory allocation failed\n");
        return;
    }

    player->playerId = playerID;
    strcpy(player->name, playerName);
    strcpy(player->role, role);
    strcpy(player->teamName, teamList[index].teamName);

    player->totalRuns = totalRuns;
    player->battingAverage = battingAvg;
    player->strikeRate = strikeRate;
    player->wickets = wickets;
    player->economyRate = economyRate;

    player->performanceIndex = calculatePI(player);

    insertPlayer(index, player, teamList);

    calculateTeamAverage(&teamList[index]);

    printf("\nPlayer added successfully to Team %s!\n\n", teamList[index].teamName);
}

void displayTeamPlayers(teamDetail *teamList)
{
    int teamID;
    printf("Enter team ID: ");
    if (scanf("%d", &teamID) != 1)
    {
        printf("Invalid Team ID.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    int index = findTeamIndex(teamList, teamID);

    if (index == -1)
    {
        printf("Team ID does not exist\n");
        return;
    }

    printf("\nPlayers of Team %s:\n\n", teamList[index].teamName);

    printf("==========================================================================\n");
    printf("%-6s %-18s %-14s %-8s %-8s %-8s %-7s %-7s %-12s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf. Index");
    printf("==========================================================================\n");

    if (teamList[index].totalPlayers == 0)
    {
        printf("No players found in this team.\n");
        printf("Total Players: 0\n");
        printf("Average Batting Strike Rate: 0.00\n");
        return;
    }

    // Traverse all 3 role-wise linked lists
    for (int i = 0; i < 3; i++)
    {
        myPlayer *temp = teamList[index].heads[i];

        while (temp != NULL)
        {
            printf("%-6d %-18s %-14s %-8d %-8.1f %-8.1f %-7d %-7.1f %-12.2f\n", temp->playerId, temp->name, temp->role, temp->totalRuns, temp->battingAverage, temp->strikeRate, temp->wickets, temp->economyRate, temp->performanceIndex);
            temp = temp->next;
        }
    }

    printf("==========================================================================\n");

    printf("\nTotal Players: %d\n", teamList[index].totalPlayers);
    printf("\nAverage Batting Strike Rate: %.2f\n\n", teamList[index].averageStrikeRate);
}
void displayTeamSorted(teamDetail *teamList)
{
    teamDetail *sortedTeam[10];
    for (int i = 0; i < 10; i++)
    {
        sortedTeam[i] = &teamList[i];
    }
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9 - i; j++)
        {
            if (sortedTeam[j]->averageStrikeRate < sortedTeam[j + 1]->averageStrikeRate)
            {
                teamDetail *temp = sortedTeam[j];
                sortedTeam[j] = sortedTeam[j + 1];
                sortedTeam[j + 1] = temp;
            }
        }
    }
    printf("Teams Sorted by Average Batting Strike Rate\n\n");
    printf("\n=========================================================\n");
    printf("%-10s %-25s %-20s %-10s\n", "Team ID", "Team Name", "Average Strike Rate", "Total Player");
    printf("\n=========================================================\n");
    for (int i = 0; i < 10; i++)
    {
        printf("%-10d %-25s %-20.2f %-10d\n", sortedTeam[i]->teamId, sortedTeam[i]->teamName, sortedTeam[i]->averageStrikeRate, sortedTeam[i]->totalPlayers);
    }
}

void displayTopKPlayers(teamDetail *teamList)
{
    int teamID;
    printf("Enter Team ID: ");
    scanf("%d", &teamID);

    char playerRole[100];
    printf("Enter Player Role (Batsman/Bowler/All-rounder): ");
    scanf("%99s", playerRole);

    int noOfPlayer;
    printf("Enter number of players: ");
    scanf("%d", &noOfPlayer);

    int index = findTeamIndex(teamList, teamID);

    if (index == -1)
    {
        printf("Invalid Team ID\n");
        return;
    }

    if (noOfPlayer <= 0)
    {
        printf("Please enter a valid number of players.\n");
        return;
    }

    int roleIndex = getRoleIndex(playerRole);

    if (roleIndex == -1)
    {
        printf("Invalid Player Role\n");
        return;
    }

    printf("\nTop %d %s of Team %s:\n\n", noOfPlayer, playerRole, teamList[index].teamName);

    printf("============================================================\n");
    printf("%-8s %-25s %-15s %-15s\n", "ID", "Player Name", "Role", "Performance Index");
    printf("============================================================\n");

    myPlayer *temp = teamList[index].heads[roleIndex];

    if (temp == NULL)
    {
        printf("No players found for this role.\n");
        return;
    }

    int count = 0;

    while (temp != NULL && count < noOfPlayer)
    {
        printf("%-8d %-25s %-15s %-15.2f\n", temp->playerId, temp->name, temp->role, temp->performanceIndex);
        temp = temp->next;
        count++;
    }

    printf("\nTotal players displayed: %d\n", count);
}

int getRoleIndex(char *role)
{
    if (strcmp(role, "Batsman") == 0)
        return 0;

    if (strcmp(role, "Bowler") == 0)
        return 1;

    if (strcmp(role, "All-rounder") == 0)
        return 2;

    return -1;
}

void displayAllPlayersByRole(teamDetail *teamList)
{
    char role[100];

    printf("Enter role (Batsman/Bowler/All-rounder): ");
    if (scanf("%99s", role) != 1)
    {
        printf("Invalid role input.\n");
        clearInputBuffer();
        return;
    }

    int roleIndex = getRoleIndex(role);

    if (roleIndex == -1)
    {
        printf("Invalid role\n");
        return;
    }

    ResultNode *listHeads[10];

    for (int i = 0; i < 10; i++)
    {
        listHeads[i] = NULL;
    }

    for (int i = 0; i < 10; i++)
    {

        myPlayer *temp = teamList[i].heads[roleIndex];

        ResultNode *tail = NULL;

        while (temp != NULL)
        {
            ResultNode *newNode = malloc(sizeof(ResultNode));

            if (newNode == NULL)
            {
                printf("Memory allocation failed\n");

                for (int j = 0; j < 10; j++)
                {
                    ResultNode *current = listHeads[j];

                    while (current != NULL)
                    {
                        ResultNode *deleteNode = current;
                        current = current->next;
                        free(deleteNode);
                    }
                }

                return;
            }

            newNode->player = temp;
            newNode->next = NULL;

            if (listHeads[i] == NULL)
            {
                listHeads[i] = newNode;
            }
            else
            {
                tail->next = newNode;
            }

            tail = newNode;
            temp = temp->next;
        }
    }
    int count = 10;

    while (count > 1)
    {
        int j = 0;

        for (int i = 0; i < count; i += 2)
        {
            if (i + 1 < count)
            {
                listHeads[j] = mergeResultLists(listHeads[i], listHeads[i + 1]);
            }
            else
            {
                listHeads[j] = listHeads[i];
            }

            j++;
        }

        count = j;
    }
    if (listHeads[0] == NULL)
    {
        printf("No players found for this role.\n");
        return;
    }

    // Step 3: Final sorted list is listHeads[0]
    printf("\nAll %s Players Across Teams (Sorted by Performance Index):\n", role);

    printf("=========================================================================\n");
    printf("%-8s %-20s %-20s %-15s %-10s\n", "ID", "Name", "Team", "Role", "PI");
    printf("==========================================================================\n");

    ResultNode *temp = listHeads[0];

    int total = 0;

    while (temp != NULL)
    {
        myPlayer *player = temp->player;

        printf("%-8d %-20s %-20s %-15s %-10.2f\n", player->playerId, player->name, player->teamName, player->role, player->performanceIndex);
        temp = temp->next;
        total++;
    }

    printf("======================================================================\n");
    printf("Total Players: %d\n", total);
    temp = listHeads[0];

    while (temp != NULL)
    {
        ResultNode *deleteNode = temp;
        temp = temp->next;
        free(deleteNode);
    }
}

ResultNode *mergeResultLists(ResultNode *list1, ResultNode *list2)
{
    ResultNode *temp1 = list1;
    ResultNode *temp2 = list2;

    ResultNode dummy;
    dummy.next = NULL;

    ResultNode *tail = &dummy;

    while (temp1 != NULL && temp2 != NULL)
    {
        if (temp1->player->performanceIndex >= temp2->player->performanceIndex)
        {
            tail->next = temp1;
            temp1 = temp1->next;
        }
        else
        {
            tail->next = temp2;
            temp2 = temp2->next;
        }

        tail = tail->next;
    }

    if (temp1 != NULL)
        tail->next = temp1;
    else
        tail->next = temp2;

    return dummy.next;
}

void freeAllPlayers(teamDetail *teamList)
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            myPlayer *temp = teamList[i].heads[j];

            while (temp != NULL)
            {
                myPlayer *deleteNode = temp;

                temp = temp->next;

                free(deleteNode);
            }

            teamList[i].heads[j] = NULL;
        }

        teamList[i].totalPlayers = 0;
        teamList[i].averageStrikeRate = 0;
    }
}
void clearInputBuffer()
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        // discard invalid input
    }
}