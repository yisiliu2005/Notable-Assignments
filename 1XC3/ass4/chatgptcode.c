#include <stdio.h>
#include <string.h>

void deleteChar(char *str, char ch) {
    int i, j;
    int len = strlen(str);

    // Iterate through the string
    for (i = 0, j = 0; i < len; i++) {
        // If the current character is not the character to be deleted
        if (str[i] != ch) {
            // Move the character to the left
            str[j++] = str[i];
        }
    }
    // Null-terminate the string after removing the character
    str[j] = '\0';
}

int main() {
    char str[] = "hello";
    char ch = 'h';

    printf("Before deletion: %s\n", str);

    deleteChar(str, ch);

    printf("After deletion: %s\n", str);

    return 0;
}

