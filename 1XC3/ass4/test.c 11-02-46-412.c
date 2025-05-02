#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>

void deleteChar(char *str, char ch)
{
    int i, j;
    int len = strlen(str);

    // Iterate through the string
    for (i = 0, j = 0; i < len; i++)
    {
        // If the current character is not the character to be deleted
        if (str[i] != ch)
        {
            // Move the character to the left
            str[j++] = str[i];
        }
    }
    // Null-terminate the string after removing the character
    str[j] = '\0';
}

/*
int main()
{
    FILE *file = fopen("shortvader.txt", "r");
    if (file == NULL)
    {
        printf("file open failed\n");
        return 1;
    }

    char string[10];
    float score;
    int n1, n2, n3, n4, n5, n6, n7, n8, n9, n10;
    double SD;

    printf("%[\t] %f	%lf	[%d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n", string, score, SD, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10);

    fclose(file);

    return 0;
}*/

int main()
{
    FILE *file = fopen("validation.txt", "r");
    if (file == NULL)
    {
        printf("file open failed\n");
        return 1;
    }

    char word[30];

    while (fscanf(file, "%s", word) != -1)
    {
        printf("%s ", word);
        if (fgetc(file) == '\n')
        {
            printf("\n");
        }
    }

    fclose(file);

    return 0;
}