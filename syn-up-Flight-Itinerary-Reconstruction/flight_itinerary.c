#include <stdio.h>
#include <string.h>

int main()
{
    int n;
    scanf("%d",&n);

    char source[100000][101];
    char destination[100000][101];

    for(int i = 0; i < n ;i++)
    {
        scanf(" %100[^-]",source[i]);
        scanf(" -> %100[^\n]",destination[i]);

        source[i][strcspn(source[i], "\n")]='\0';
        destination[i][strcspn(destination[i],"\n")]='\0';

        char *s = source[i];
        while(s == ' ')
        {
            s++;
        }
        memmove(source[i], s, strlen(s)+1);

        int len = strlen(destination[i]);
        while(len > 0 && destination[i][len - 1] == ' ')
        {
            destination[i][--len] = '\0';
        }
    }
    int start = -1;
    for(int i=0; i < n; i++)
    {
        int found = 0;
        for(int j=0;j<n;j++){
            if(strcmp(source[i],destination[j])==0)
            {
                found = 1;
                break;
            }
        }
        if(found == 0){
            start = i;
            break;
        }
    }
    char current[101];
    strcpy(current,source[start]);
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            if(strcmp(current,source[j])==0)
            {
                printf("%s->%s",current,destination[j]);
            }
        }
    }

}