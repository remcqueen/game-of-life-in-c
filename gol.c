#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gol.h"

static int mod(int number, int base)
{
    return (number < 0 ? ((number % base) + base) % base : number % base);
}

void read_in_file(FILE *infile, struct universe *u)
{
    if (infile == NULL)
    {
        fprintf(stderr, "Error - file pointer does not exist");
        exit(1);
    }

    fseek(infile, 0, SEEK_END);
    int len = ftell(infile);
    rewind(infile);
    int row_length = 0;
    int column_length = 0;
    char charecter = '\0';
    char *stream_pointer = NULL;
    if (len == 0)
    {
        int tempColCounter = 0;
        int prevColCount = 0;
        stream_pointer = malloc(512);
        if (stream_pointer == NULL)
        {
            fprintf(stderr, "Error - Failed to allocate memory\n");
            exit(1);
        }
        while (scanf("%c", &charecter) != EOF)
        {

            if (charecter != '*' && charecter != '.' && charecter != '\n')
            {
                fprintf(stderr, "Error - Invalid charecter given\n");
                exit(1);
            }
            else if (charecter == '*' || charecter == '.')
            {
                stream_pointer[column_length] = charecter;
                column_length++;
                tempColCounter++;
            }

            else if (charecter == '\n')
            {
                if (row_length != 0 && tempColCounter != prevColCount)
                {
                    fprintf(stderr, "Error - Column lengths were not the same\n");
                    exit(1);
                }
                if (tempColCounter < 1 || tempColCounter > 512)
                {
                    fprintf(stderr, "Error - Invalid row length\n");
                    exit(1);
                }
                row_length++;

                prevColCount = tempColCounter;
                tempColCounter = 0;
                if (row_length > 0)
                {
                    stream_pointer = realloc(stream_pointer, (row_length + 1) * prevColCount);
                    if (stream_pointer == NULL)
                    {
                        fprintf(stderr, "Error - Failed to reallocate memory\n");
                        exit(1);
                    }
                }
            }
        }

        column_length /= row_length;
        stream_pointer = realloc(stream_pointer, row_length * prevColCount);
        if (stream_pointer == NULL)
        {
            fprintf(stderr, "Error - Failed to reallocate memory\n");
            exit(1);
        }
    }
    else

    {
        row_length = 1;

        column_length = 1;

        charecter = fgetc(infile);

        // boolean for if the last char was a blank file
        int lastChar = 0;

        while (charecter != EOF)
        {
            if (charecter == '*' || charecter == '.')
            {
                column_length++;
                lastChar = 0;
            }
            // checks if the last charecter was an end of line
            else if (charecter == '\n' && !lastChar)
            {
                row_length++;
                lastChar = 1;
            }

            else if (charecter != '\0' || charecter != EOF || charecter != '\n')
            {
                fprintf(stderr, "Error - invalid character detected in input file\n");
                exit(1);
            }

            charecter = fgetc(infile);
        }

        // If final charecter was an end of line, removes that row
        if (lastChar)
        {
            row_length--;
        }
        else
        {
            fprintf(stderr, "Error - input file must end in a newline\n");
            exit(1);
        }

        // calculates number of columns
        column_length /= row_length;
    }

    u->row_length = row_length;
    u->column_length = column_length;

    u->current_matrix = malloc(sizeof(u->current_matrix) * row_length);
    if (u->current_matrix == NULL)
    {
        fprintf(stderr, "Error - Failed to allocate memory\n");
        exit(1);
    }
    for (int i = 0; i < row_length; i++)
    {
        u->current_matrix[i] = malloc(sizeof(u->current_matrix) * column_length);
        if (u->current_matrix[i] == NULL)
        {
            fprintf(stderr, "Error - Failed to allocate memory\n");
            exit(1);
        }
    }

    u->next_matrix = malloc(sizeof(u->next_matrix) * row_length);
    if (u->next_matrix == NULL)
    {
        fprintf(stderr, "Error - Failed to allocate memory\n");
        exit(1);
    }
    for (int i = 0; i < row_length; i++)
    {
        u->next_matrix[i] = malloc(sizeof(u->next_matrix) * column_length);
        if (u->next_matrix[i] == NULL)
        {
            fprintf(stderr, "Error - Failed to allocate memory\n");
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

    for (int row = 0; row < row_length; row++)
    {
        for (int column = 0; column < column_length; column++)
        {
            if (len != 0)
            {
                charecter = fgetc(infile);
            }
            else
            {
                charecter = stream_pointer[counter];
                counter++;
            }

            while (charecter != '*' && charecter != '.')
            {
                charecter = fgetc(infile);
            }

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

    if (len != 0)
    {
        fclose(infile);
    }
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

    if (outfile == NULL)
    {
        fprintf(stderr, "\nError - output file not found\nExiting...\n");
        exit(1);
    }

    for (int rowCounter = 0; rowCounter < numRows; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < numColumns; columnCounter++)
        {
            fprintf(outfile, "%c", u->current_matrix[rowCounter][columnCounter]);
        }
        fprintf(outfile, "\n");
    }
    fclose(outfile);
}

int is_alive(struct universe *u, int column, int row)
{
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
    // printf("Called: will be alive\n");
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
    // printf("Called: will be alive\n");
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
                u->current_alive += 1;
                u->total_alive += 1;
            }
            else
            {
                u->next_matrix[r][c] = '.';
            }
        }
    }

    u->current_matrix = u->next_matrix;

    return;
}

void print_statistics(struct universe *u)
{
    int total_cells = u->column_length * u->row_length;
    float current_alive_percentage = ((float)u->current_alive / (float)total_cells) * 100.000;
    float total_alive;

    if (!u->generations)
    {
        total_alive = ((float)u->total_alive * 100.000) / ((float)total_cells);
    }
    else
    {
        total_alive = ((float)u->total_alive * 100.000) / ((float)total_cells * (float)u->generations);
    }

    printf("%.3f%c of cells currently alive\n%.3f%c of cells alive on average\n", current_alive_percentage, '%', total_alive, '%');
    return;
}
