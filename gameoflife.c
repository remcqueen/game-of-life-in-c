#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include "gol.h"
int main(int argc, char *argv[])
{
  // s and t are booleans for if switch was given
  // g initially -1 to check if multiple -g flags given
  int g = -1, s = 0, t = 0;
  // i and o are pointers to contain the file names if given
  char *i = NULL, *o = NULL;
  FILE *inputFile = NULL;
  FILE *outputFile = NULL;

  // for loop through each argument given
  for (int counter = 1; counter < argc; counter++)
  {
    // checks if argument is a flag
    if (argv[counter][0] == '-' && strlen(argv[counter]) == 2)
    {
      // Switch case for each flag given
      switch (argv[counter][1])
      {
      case 'i':

        // checks if -i was given as final argument
        if (counter + 2 > argc)
        {
          fprintf(stderr, "Error: Input file not specified\n");
          exit(1);
        }

        // if -i flag was already given, checks if the filename was the same
        if (i != NULL)
        {
          int difference = strcmp(i, argv[counter + 1]);

          if (difference)
          {
            fprintf(stderr, "Error: Switch -i has two inconsistant entries: %s and %s\n", i, argv[counter + 1]);
            exit(1);
          }
          else if (!difference)
          {
            break;
          }
        }

        // stores name of file in i
        i = argv[counter + 1];

        // Checks if file exists and can be opened
        if ((inputFile = fopen(i, "r")) == NULL)
        {
          fprintf(stderr, "Error: Could not open input file with name %s\n", i);
          exit(1);
        }

        // increase counter to skip checking next argument
        counter++;

        break;

      case 'o':

        // checks if -i was given as final argument
        if (counter + 2 > argc)
        {
          fprintf(stderr, "\nError: Output file not specified\n");
          exit(1);
        }

        // change to check if new -o is the same file
        if (o != NULL)
        {
          int difference = strcmp(o, argv[counter + 1]);

          if (difference)
          {
            fprintf(stderr, "Error: Switch -o has two inconsistant entries: %s and %s\n", o, argv[counter + 1]);
            exit(1);
          }
          else if (!difference)
          {
            break;
          }
        }

        // Stores output file name
        o = argv[counter + 1];

        // increase counter to skip checking next argument
        counter++;

        break;

      case 'g':

        // checks if -g was given as final argument
        if (counter + 2 > argc)
        {
          fprintf(stderr, "Error: Number of generations not specified\n");
          exit(1);
        }

        // checks if -g switch has been given before
        // checks if previous value of g matches new value
        if (g != -1)
        {
          int same = g == atoi(argv[counter + 1]);

          if (!same)
          {
            fprintf(stderr, "Error: Switch -g has two inconsistant entries: %d and %s\n", g, argv[counter + 1]);
            exit(1);
          }
          else if (same)
          {
            break;
          }
        }

        int lengthOfString = strlen(argv[counter + 1]);

        // checks each charecter in argument for if it lies it is a number
        for (int j = 0; j < lengthOfString; j++)
        {
          int integerConvert = (int)argv[counter + 1][j];
          if (integerConvert < 48 || integerConvert > 57)
          {
            fprintf(stderr, "Error: Number of generations must be an integer\n");
            exit(1);
          }
        }

        // Ensures that the user did not input a generation size larger than max int value which would cause overflow
        if (strtol(argv[counter + 1], NULL, 0) > INT_MAX || (strtol(argv[counter + 1], NULL, 0) == 0 && lengthOfString != 1))
        {
          fprintf(stderr, "Error: generations given will exceed maximum integer storage\n");
          exit(1);
        }

        // increase counter to skip checking next argument
        g = strtol(argv[counter + 1], NULL, 0);
        counter++;
        break;

      case 's':

        s = 1;
        break;

      case 't':

        t = 1;
        break;

      // default case for invalid/unknown switches given
      default:
        fprintf(stderr, "Error: Invalid command line switch given\n");
        exit(1);
      }
    }

    // error check for unknown commands given as input
    else
    {
      fprintf(stderr, "Error: Invalid command line switch given\n");
      exit(1);
    }
  }

  struct universe v;

  if (i)
  {
    read_in_file(inputFile, &v);
  }
  else
  {
    read_in_file(stdin, &v);
  }

  // itterates through g and evolves
  if (g == -1)
  {
    g = 5;
  }

  for (int iteration = 0; iteration < g; iteration++)
  {
    if (t)
    {
      evolve(&v, will_be_alive_torus);
    }
    else
    {
      evolve(&v, will_be_alive);
    }
  }

  if (o)
  {
    outputFile = fopen(o, "w");
    write_out_file(outputFile, &v);
  }
  else
  {
    write_out_file(stdout, &v);
  }

  if (s)
  {
    print_statistics(&v);
  }

  return 0;
}
