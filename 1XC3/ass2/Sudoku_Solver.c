// Code: Here include your necessary library(s)
#include <stdio.h>

// Code: Write your global variables here , like :
#define N 9
int count = 0;

/*Code : write your functions here, or the declaration of the function /
For example write the recursive function solveSudoku(), like :*/

void print(int grid[N][N]);

int valid(int grid[N][N], int i, int j, int num)
{
    for (int ind = 0; ind < N; ind++)
    {
        if (grid[ind][j] == num || grid[i][ind] == num)
        {
            return 0;
        }
    }
    /*for (int row = 0; row < 9; row++)
    {
        if (row == i)
            continue;

        if (grid[row][j] == num)
        {
            return 0;
        }
    }

    for (int col = 0; col < 9; col++)
    {
        if (col == j)
            continue;

        if (grid[i][col] == num)
        {
            return 0;
        }
    }*/

    int r1, r2, c1, c2;
    switch (i % 3)
    {
    case 0:
        r1 = i + 1;
        r2 = i + 2;
        break;
    case 1:
        r1 = i - 1;
        r2 = i + 1;
        break;
    case 2:
        r1 = i - 2;
        r2 = i - 1;
        break;
    }
    switch (j % 3)
    {
    case 0:
        c1 = j + 1;
        c2 = j + 2;
        break;
    case 1:
        c1 = j - 1;
        c2 = j + 1;
        break;
    case 2:
        c1 = j - 2;
        c2 = j - 1;
        break;
    }

    if (grid[r1][c1] == num || grid[r2][c1] == num || grid[r1][c2] == num || grid[r2][c2] == num)
    {
        return 0;
    }

    return 1;
}

int solveSudoku(int grid[N][N])
{
    count++;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (grid[i][j] != 0)
            {
                continue;
            }

            for (int ans = 1; ans <= 9; ans++)
            {
                if (valid(grid, i, j, ans))
                {
                    grid[i][j] = ans;
                    
                    if (solveSudoku(grid) == 0)
                    {
                        grid[i][j] = 0;
                    }
                    /*if (solveSudoku(grid))
                    {
                        break;
                    } else
                    {
                        grid[i][j] = 0;
                    }*/
                }
            }

            if (grid[i][j] == 0) //if still zero, ie no numbers fit
            {
                return 0;
            }
        }
    }
    return 1;
}

int main()
{
    // This is hard coding to receive the ”grid”
    int grid[N][N] = {
        {1, 0, 0, 4, 8, 9, 0, 0, 6},
        {7, 3, 0, 0, 5, 0, 0, 4, 0},
        {4, 6, 0, 0, 0, 1, 2, 9, 5},
        {3, 8, 7, 1, 2, 0, 6, 0, 0},
        {5, 0, 1, 7, 0, 3, 0, 0, 8},
        {0, 4, 6, 0, 9, 5, 7, 1, 0},
        {9, 1, 4, 6, 0, 0, 0, 8, 0},
        {0, 2, 0, 0, 4, 0, 0, 3, 7},
        {8, 0, 3, 5, 1, 2, 0, 0, 4}};

    // For more samples to check your program, google for solved samples, or
    // check https://sandiway.arizona.edu/sudoku/examples.html

    printf("The input Sudoku puzzle :\n");
    // ”print” is a function we define to print the ”grid”
    print(grid);

    if (solveSudoku(grid))
    {
        // If the puzzle is solved then:
        printf("Solution found after % d iterations :\n", count);
        print(grid);
    }
    else
    {
        printf("No solution exists. %d\n", count);
        print(grid);
    }
    return 0;
}

/*Code : If you have functions that are declared but not implemented they,
here write the implementation. */

void print(int grid[N][N])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%d  ", grid[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}


/*{
    count += 1;
    // Code: count+1, the number of times the function was called.
    // Code: here write the implementation of solveSudoku

//base cases
    if (i > 8)
    {
        return 1;
    }

    if (i < 0)
    {
        return 0;
    }

//fill the square at (i, j)
    do
    {
        if (grid[i][j] < 9)
        {
            grid[i][j] += 1;
        }
        else
        {
            grid[i][j] = 0;
        }
    } while (valid(grid, grid[i][j]) == 0 && grid[i][j] != 0);

//change squares
    if (valid(grid, grid[i][j]))
    {
        do
        {
            if (j < 8)
            {
                j += 1;
            }
            else // j == 8
            {
                i += 1;
                j = 0;
            }
        } while (grid[i][j] != 0);
    }
    else if (grid[i][j] == 0)
    {
        do
        {
            if (j > 0)
            {
                j -= 1;
            } else // j == 0
            {
                i -= 1;
                j = 8;
            }
        } while (grid[i][j] != 0); //oh no can't differentiate between puzzle and entry
    }

    if (solveSudoku(grid))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}*/