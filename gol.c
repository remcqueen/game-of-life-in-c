#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "gol.h"

// Function for returning the signed modulous
static int mod(int number, int base)
{
    return (number < 0 ? ((number % base) + base) % base : number % base);
}

void read_in_file(FILE *infile, struct universe *u)
{
    // checks if the file pointer exists
    if (infile == NULL)
    {
        fprintf(stderr, "Error: File pointer does not exist");
        exit(1);
    }

    // sets the file pointer to the end of the input stream
    fseek(infile, 0, SEEK_END);
    // gets the position of the current pointer which will give the size of the stream
    int len = ftell(infile);
    // resets the position of the pointer
    rewind(infile);

    int row_length = 0;
    int column_length = 0;
    char charecter = '\0';

    // tempColCounter counts the number of columns on each new row
    int tempColCounter = 0;
    // prevColCount is compared to tempColCounter to ensure column lengths are equal
    int prevColCount = 0;

    // Stream pointer will point to a string that will contain the users input as they type it
    // We be dynamically allocated and reallocated as they type
    char *stream_pointer = NULL;

    // if length is 0 the file pointer is not a file, or points to an empty file
    if (!(len > 0))
    {
        // gets the character of the current position in the input stream
        charecter = getc(infile);
        // if the character is EOF it means the infile pointer is pointing to an empty file
        if (charecter == EOF)
        {
            fprintf(stderr, "Error: Input file must not be empty\n");
            exit(1);
        }
        // else means it is pointing to an empty stdin
        else
        {
            // returns the character that was taken from getc
            ungetc(charecter, infile);
            charecter = '\0';
        }

        // Allocates a 512 stream pointer initially to accept up to 512 columns
        stream_pointer = malloc(512);
        if (stream_pointer == NULL)
        {
            fprintf(stderr, "Error: Failed to allocate memory\n");
            exit(1);
        }

        printf("Assume user input\n");

        // While loop scans each character user inputs
        while (scanf("%c", &charecter) != EOF)
        {

            // Checks if charecter is valid
            if (charecter != '*' && charecter != '.' && charecter != '\n')
            {
                fprintf(stderr, "Error: Invalid charecter given\n");
                exit(1);
            }
            // If valid charecter, increases column counter and adds charecter to streampointer string
            else if (charecter == '*' || charecter == '.')
            {
                stream_pointer[column_length] = charecter;
                column_length++;
                tempColCounter++;
            }

            // If user presses enter it checks if row is in valid range
            else if (charecter == '\n')
            {
                if (row_length != 0 && tempColCounter != prevColCount)
                {
                    printf("Tempcolcount: %d\nprevColCount: %d\n", tempColCounter, prevColCount);
                    fprintf(stderr, "Error: Column lengths were not the same\n");
                    exit(1);
                }
                if (tempColCounter < 1 || tempColCounter > 512)
                {
                    fprintf(stderr, "Error: Invalid row length\n");
                    exit(1);
                }
                row_length++;

                prevColCount = tempColCounter;
                tempColCounter = 0;
                // Increases the size of stream_pointer to accept the next row
                if (row_length > 0)
                {
                    stream_pointer = realloc(stream_pointer, (row_length + 1) * prevColCount);
                    if (stream_pointer == NULL)
                    {
                        fprintf(stderr, "Error: Failed to reallocate memory\n");
                        exit(1);
                    }
                }
            }
        }

        printf("EOF Detected\n");

        // Reduces the size of stream pointer by one row as the user terminated on this row
        stream_pointer = realloc(stream_pointer, row_length * prevColCount);
        if (stream_pointer == NULL)
        {
            fprintf(stderr, "Error: Failed to reallocate memory\n");
            exit(1);
        }
    }
    // if infile points to a valid file location
    else

    {
        printf("Assume file input\n");
        prevColCount = -1;

        row_length = 1;

        column_length = 1;

        charecter = fgetc(infile);

        // boolean for if the last char was a blank file
        int lastChar = 0;

        // uses getc to move accross each character to validate and find number of rows and columns
        while (charecter != EOF)
        {
            if (charecter == '*' || charecter == '.')
            {
                column_length++;
                tempColCounter++;
                lastChar = 0;
            }
            // checks if the last charecter was an end of line
            else if (charecter == '\n' && !lastChar)
            {
                if (row_length != 0 && prevColCount != -1 && tempColCounter != prevColCount)
                {
                    fprintf(stderr, "Error: Column lengths were not the same in inputfile\n");
                    exit(1);
                }
                if (tempColCounter < 1 || tempColCounter > 512)
                {
                    fprintf(stderr, "Error: Invalid row length in inputfile\n");
                    exit(1);
                }
                prevColCount = tempColCounter;
                tempColCounter = 0;
                row_length++;
                lastChar = 1;
            }
            else if (charecter == '\n' && lastChar)
            {
                fprintf(stderr, "Error: Invalid file format\n");
                exit(1);
            }

            else if (charecter != '\0' && charecter != EOF && charecter != '\n')
            {
                printf("%c\n", charecter);
                fprintf(stderr, "Error: Invalid character detected in input file\n");
                exit(1);
            }

            charecter = fgetc(infile);
        }

        // If final charecter was an end of line, removes that row
        if (lastChar)
        {
            row_length--;
        }
        // If final charecter was not an end of line, causes error
        else
        {
            fprintf(stderr, "Error: Input file must end in a newline\n");
            exit(1);
        }
    }

    // calculates number of columns
    column_length /= row_length;

    u->row_length = row_length;
    u->column_length = column_length;

    // Dynamically allocates memory for 2d array
    u->current_matrix = malloc(sizeof(u->current_matrix) * row_length);
    u->next_matrix = malloc(sizeof(u->next_matrix) * row_length);
    if (u->current_matrix == NULL || u->next_matrix == NULL)
    {
        fprintf(stderr, "Error: Failed to allocate memory\n");
        exit(1);
    }

    // Dynamically allocates memory for each column of 2d array
    for (int i = 0; i < row_length; i++)
    {
        u->current_matrix[i] = malloc(sizeof(u->current_matrix) * column_length);
        u->next_matrix[i] = malloc(sizeof(u->next_matrix) * column_length);
        if (u->current_matrix[i] == NULL || u->next_matrix[i] == NULL)
        {
            fprintf(stderr, "Error: Failed to allocate memory\n");
            exit(1);
        }
    }

    u->total_alive = 0;
    u->current_alive = 0;
    u->generations = 0;

    if (len != 0)
    {
        rewind(infile);
    }

    int counter = 0;

    // Stores the file or user input into the 2d array
    for (int row = 0; row < row_length; row++)
    {
        for (int column = 0; column < column_length; column++)
        {
            // if infile is a valid file pointer, get next charecter
            if (len != 0)
            {
                charecter = fgetc(infile);
            }
            // Else get the next charecter from stream pointer string
            else
            {
                charecter = stream_pointer[counter];
                counter++;
            }

            // As input file contains new lines, only append * and . to universe
            while (charecter != '*' && charecter != '.')
            {
                charecter = fgetc(infile);
            }

            // Increase current alive and total alive
            if (charecter == '*')
            {
                u->current_matrix[row][column] = charecter;
                u->current_alive += 1;
                u->total_alive += 1;
            }
            else if (charecter == '.')
            {
                u->current_matrix[row][column] = charecter;
            }
        }
    }

    // if valid file was given
    if (len != 0)
    {
        fclose(infile);
    }
    // Else free the memory pointer used to store user input
    else
    {
        free(stream_pointer);
    }

    return;
}

void write_out_file(FILE *outfile, struct universe *u)
{
    int numColumns = u->column_length;
    int numRows = u->row_length;

    // error check input
    if (outfile == NULL)
    {
        fprintf(stderr, "Error: Output file not found\n");
        exit(1);
    }

    // go through each index of 2d array and print to infile
    for (int rowCounter = 0; rowCounter < numRows; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < numColumns; columnCounter++)
        {
            fprintf(outfile, "%c", u->current_matrix[rowCounter][columnCounter]);
        }
        fprintf(outfile, "\n");
    }
}

int is_alive(struct universe *u, int column, int row)
{

    // Error checks input parameters
    if (column < 0 || column > u->column_length || row < 0 || row > u->row_length)
    {
        fprintf(stderr, "Error: Invalid column or row parameter given in is_alive\n");
        exit(1);
    }

    if (u->current_matrix[row][column] == '*')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int will_be_alive(struct universe *u, int column, int row)
{

    if (column < 0 || column > u->column_length || row < 0 || row > u->row_length)
    {
        fprintf(stderr, "Error: Invalid column or row parameter given in is_alive\n");
        exit(1);
    }

    int alive_neighbours = 0;

    // check top
    if (row != 0)
    {
        // printf("Checked Top\n");
        alive_neighbours += is_alive(u, column, row - 1);

        // check diagonal left top
        if (column != 0)
        {
            // printf("Checked Diagonal left Top\n");
            alive_neighbours += is_alive(u, column - 1, row - 1);
        }
        // check diagonal right top
        if (u->column_length - 1 != column)
        {
            // printf("Checked Diagonal right Top\n");
            alive_neighbours += is_alive(u, column + 1, row - 1);
        }
    }

    // check left
    if (column != 0)
    {
        // printf("Checked left\n");
        alive_neighbours += is_alive(u, column - 1, row);
    }
    // check right
    if (u->column_length - 1 != column)
    {
        // printf("Checked right\n");
        alive_neighbours += is_alive(u, column + 1, row);
    }

    // check bottom
    if (u->row_length - 1 != row)
    {
        // printf("Checked bottom\n");
        alive_neighbours += is_alive(u, column, row + 1);

        // check diagonal left bottom
        if (column != 0)
        {
            // printf("Checked Diagonal left bottom\n");
            alive_neighbours += is_alive(u, column - 1, row + 1);
        }
        // check diagonal right bottom
        if (u->column_length - 1 != column)
        {
            // printf("Checked Diagonal right bottom\n");
            alive_neighbours += is_alive(u, column + 1, row + 1);
        }
    }

    int current_cell = is_alive(u, column, row);

    // checks conditions for if cell will be alive or dead
    if (current_cell && (alive_neighbours == 2 || alive_neighbours == 3))
    {
        return 1;
    }
    else if (!current_cell && alive_neighbours == 3)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int will_be_alive_torus(struct universe *u, int column, int row)
{

    // Error check column and row parameters are in range
    if (column < 0 || column >= u->column_length || row < 0 || row >= u->row_length)
    {
        fprintf(stderr, "Error: Invalid column or row parameter given in is_alive\n");
        exit(1);
    }

    int alive_neighbours = 0;
    int row_base = u->row_length;
    int col_base = u->column_length;

    // check top
    alive_neighbours += is_alive(u, column, mod(row - 1, row_base));

    // check diagonal left top
    alive_neighbours += is_alive(u, mod(column - 1, col_base), mod(row - 1, row_base));

    // check diagonal right top
    alive_neighbours += is_alive(u, mod(column + 1, col_base), mod(row - 1, row_base));

    // check left
    alive_neighbours += is_alive(u, mod(column - 1, col_base), row);

    // check right
    alive_neighbours += is_alive(u, mod(column + 1, col_base), row);

    // check bottom
    alive_neighbours += is_alive(u, column, mod(row + 1, row_base));

    // check diagonal left bottom
    alive_neighbours += is_alive(u, mod(column - 1, col_base), mod(row + 1, row_base));

    // check diagonal right bottom
    alive_neighbours += is_alive(u, mod(column + 1, col_base), mod(row + 1, row_base));

    int current_cell = is_alive(u, column, row);

    if (current_cell && (alive_neighbours == 2 || alive_neighbours == 3))
    {
        return 1;
    }
    else if (!current_cell && alive_neighbours == 3)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row))
{

    // set current alive cells to 0
    u->current_alive = 0;
    u->generations++;

    for (int r = 0; r < u->row_length; r++)
    {
        for (int c = 0; c < u->column_length; c++)
        {
            if ((*rule)(u, c, r))
            {
                u->next_matrix[r][c] = '*';

                // Error check to prevent int overflow
                if (u->total_alive == INT_MAX)
                {
                    fprintf(stderr, "Error: total alive cells will exceed maximum integer storage\n");
                    exit(1);
                }

                u->current_alive += 1;
                u->total_alive += 1;
            }
            else
            {
                u->next_matrix[r][c] = '.';
            }
        }
    }

    for (int r = 0; r < u->row_length; r++)
    {
        for (int c = 0; c < u->column_length; c++)
        {
            u->current_matrix[r][c] = u->next_matrix[r][c];
        }
    }

    return;
}

void print_statistics(struct universe *u)
{
    int total_cells = u->column_length * u->row_length;
    float current_alive_percentage = ((float)u->current_alive / (float)total_cells) * 100.000;
    float total_alive;

    // Add 1 to generations to account for initial input
    total_alive = ((float)u->total_alive * 100.000) / ((float)total_cells * ((float)u->generations + 1.000));

    printf("%.3f%c of cells currently alive\n%.3f%c of cells alive on average\n", current_alive_percentage, '%', total_alive, '%');
    return;
}
