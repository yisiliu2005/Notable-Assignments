#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct words
{
    char word[30];//sorry to change it but this is just so much more convenient and i dont think there's a word longer than 30 letters so i think it's fine
    float score;
    float SD;
    int SIS_array[10];
};

// put what i got from fscanf into struct words
void store(struct words *worddata, char *word, float score, float SD, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10);

void deleteChar(char *str, char ch); // for sanitizing, from chatgpt

int main(int argc, char *argv[]) // first will be vader_lexicon, second will be validation
{

    // loop through file and store each line into an array of struct words

    // first make array
    struct words *arr = malloc(sizeof(struct words)); // can't use realloc before using malloc so here
    if (arr == NULL)                                  // check
    {
        printf("mem alloc failed. \n");
        return 1;
    }
    // then fill array
    // first open file
    FILE *lexicon = fopen(argv[1], "r");
    if (lexicon == NULL) // check
    {
        printf("could not open vader_lexicon.txt\n");
        return 1;
    }

    // now read file
    // variables for fscanf
    char word[30];
    float score;
    float SD;
    int n1, n2, n3, n4, n5, n6, n7, n8, n9, n10;

    int linenumber = 0;

    while (fscanf(lexicon, "%s	%f	%f	[%d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n", word, &score, &SD, &n1, &n2, &n3, &n4, &n5, &n6, &n7, &n8, &n9, &n10) != -1) // the emojis/phrases with spaces screw me up so i'll just ignore them ahaha...
    // for debugging printf("%s	%f	%f	[%d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n", word, score, SD, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10);
    {
        linenumber++;
        arr = realloc(arr, linenumber * sizeof(struct words)); // get space each time so it's flexy to sizey of filey
        store(arr + linenumber - 1, word, score, SD, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10);
    }

    fclose(lexicon); // close vader_lexicon
    // also for debugging printf("%s\n", (arr+linenumber -1)->word);

    // loop through array of structs words to find score for each word in a sentence, calculate average for sentence

    // first, create array of all sentence scores for storage
    float *scores = malloc(sizeof(float)); // REMEMBER TO CHANGE TO DYM AND ADD REALLOC BELOW
    int linedex = -1;

    // next, create reusable array to keep track of all the scores of words in a sentence before averaging
    float tosum[50]; // human sentences are not very long plus this is for reusing, so i chose stack
    // initialize; zero unless found:
    for (int i = 0; i < 50; i++)
    {
        tosum[i] = 0;
    }
    int index = -1;

    // next, go through file
    // first, open validation to read
    FILE *validation = fopen(argv[2], "r");

    /*
    while (fscanf(validation, "%s", word) != -1)
    {
        printf("%s", word);
        if(fgetc(validation)=='\n')
        {
            printf("\n");
        }
    }*///tested structure in test.c!

    char valword[30];

    while (fscanf(validation, "%s", valword) != -1)
    {
        index++; // go to next element in array for each word
        // printf("word: %s \n", valword);
        // printf("struct: %s\n", (arr + 137)->word);
        int unmatched = 1;
        for (int i = 0; i < linenumber; i++)
        {
            if (strcmp((arr + i)->word, valword) == 0)
            {
                tosum[index] = (arr + i)->score;
                unmatched = 0;
                // from debugging printf("matched first %s\n", valword);
                break;
            }
        }
        if (unmatched) //if unmatched, scrub and try again (probably should've put the rest in some functions honestly but eh)
        {
            int l = strlen(valword);//for using in loops
            for (int i = 0; i < l; i++)
            {
                if (isupper(valword[i])) // no capitals
                {
                    valword[i] = tolower(valword[i]);
                }
            }
            for (int i = 0; i < l; i++)
            {
                if (isalpha(valword[i]) == 0) // no punctuation
                {
                    deleteChar(valword, valword[i]);
                    l = strlen(valword);
                }
            }

            // check again
            for (int i = 0; i < linenumber; i++)
            {
                if (strcmp((arr + i)->word, valword) == 0)
                {
                    tosum[index] = (arr + i)->score;
                    // from debugging printf("matched clean %s\n", valword);
                    break;
                }
            }
        }

        if (fgetc(validation) == '\n') // when gotten to the end of one line:
        {
            // a thing in scores = sum of tosum
            linedex++;
            scores = realloc(scores, (linedex + 1) * sizeof(float)); // get space each time so it's flexy to sizey of filey again
            
            float sum = 0;
            for (int i = 0; i <= index; i++)
            {
                sum += tosum[i];
            }// from debugging printf("sum: %f, average: %f\n", sum, sum / (index + 1));

            scores[linedex] = (sum / (index + 1));

            // reset for reuse
            for (int i = 0; i < 50; i++)
            {
                tosum[i] = 0;
            }
            index = -1;
        }
    }

    fclose(validation); // close file

    //okay wait i still need to print all the sentences but i don't wanna touch any code above here so ill just reopen and reclose validation

    FILE *val = fopen(argv[2], "r");

    int i = -1;
    char pad[100];

    while (fgets(pad, sizeof(pad), val) != NULL)
    {
        i++;
        deleteChar(pad, '\n');
        printf("%-100s", pad);
        printf("%7.2f\n", scores[i]);
    }

    fclose(val);


    // free scores
    free(scores);

    // free dynamically allocated array of struct words
    free(arr);

    return 0;
}

//function implementations:

void store(struct words *worddata, char *word, float score, float SD, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10)
{
    worddata->score = score;
    worddata->SD = SD;
    worddata->SIS_array[0] = n1; // is there a way to do this that's not manually stating each thing
    worddata->SIS_array[1] = n2;
    worddata->SIS_array[2] = n3;
    worddata->SIS_array[3] = n4;
    worddata->SIS_array[4] = n5;
    worddata->SIS_array[5] = n6;
    worddata->SIS_array[6] = n7;
    worddata->SIS_array[7] = n8;
    worddata->SIS_array[8] = n9;
    worddata->SIS_array[9] = n10;
    strcpy(worddata->word, word);
}

void deleteChar(char *str, char ch) //credit: chatgpt
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