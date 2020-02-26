#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gol.h"
int main(int argc, char *argv[])
{
  int g = -1, s = 0, t = 0, lengthOfFile;
  char *i = NULL, *o = NULL;
  FILE *inputFile;
  FILE *outputFile;

  for (int counter = 0; counter < argc; counter++)
  {
    if (argv[counter][0] == '-')
    {
      switch (argv[counter][1])
      {
      case 'i':

        if (counter + 2 > argc)
        {
          fprintf(stderr, "Error - input file not specified\n");
          exit(128);
        }

        if (i != NULL)
        {
          int difference = strcmp(i, strcat(argv[counter + 1], ".txt")) ? strcmp(i, strcat(argv[counter + 1], ".txt")) : strcmp(i, argv[counter + 1]);

          if (difference)
          {
            fprintf(stderr, "Error - switch -i has two inconsistant entries: %s and %s\n", i, argv[counter + 1]);
            exit(128);
          }
          else if (!difference)
          {
            break;
          }
        }

        lengthOfFile = strlen(argv[counter + 1]);

        i = calloc(lengthOfFile, sizeof(char));

        if (i == NULL)
        {
          fprintf(stderr, "Error - memory for input file not allocated\n");
          exit(128);
        }

        strcpy(i, argv[counter + 1]);

        if ((inputFile = fopen(i, "r")) == NULL)
        {
          fprintf(stderr, "Error - could not open input file with name %s\n", i);
          exit(128);
        }

        break;

      case 'o':

        if (counter + 2 > argc)
        {
          fprintf(stderr, "\nError - output file not specified\n");
          exit(128);
        }

        // change to check if new -o is the same file
        if (o != NULL)
        {
          int difference = strcmp(o, argv[counter + 1]);

          if (difference)
          {
            fprintf(stderr, "Error - switch -o has two inconsistant entries: %s and %s\n", o, argv[counter + 1]);
            exit(128);
          }
          else if (!difference)
          {
            break;
          }
        }

        lengthOfFile = strlen(argv[counter + 1]);

        o = calloc(lengthOfFile, sizeof(char));

        if (o == NULL)
        {
          fprintf(stderr, "\nError - memory for output file not allocated\nExiting...\n");
          exit(128);
        }

        strcpy(o, argv[counter + 1]);
        // o[lengthOfFile] = '\0';
        break;

      case 'g':

        if (counter + 2 > argc)
        {
          fprintf(stderr, "\nError - number of generations not specified\nExiting...\n");
          exit(128);
        }

        if (g != -1)
        {
          int same = g == atoi(argv[counter + 1]);

          if (!same)
          {
            fprintf(stderr, "Error - switch -g has two inconsistant entries: %d and %s\n", g, argv[counter + 1]);
            exit(128);
          }
          else if (same)
          {
            break;
          }
        }

        int lengthOfString = strlen(argv[counter + 1]);

        for (int j = 0; j < lengthOfString; j++)
        {
          int integerConvert = (int)argv[counter + 1][j];
          if (integerConvert < 48 || integerConvert > 57)
          {
            fprintf(stderr, "\nError - number of generations must be an integer\nExiting...\n");
            exit(128);
          }
        }

        g = atoi(argv[counter + 1]);
        break;

      case 's':
        // if (s != 0)
        // {
        //   fprintf(stderr, "\nError - multiple instances of -s switch\nExiting...\n");
        //   exit(128);
        // }

        s = 1;
        break;

      case 't':
        // if (t != 0)
        // {
        //   fprintf(stderr, "\nError - multiple instances of -t switch\nExiting...\n");
        //   exit(128);
        // }

        t = 1;
        break;
      }
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

  if (s)
  {
    print_statistics(&v);
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

  return 0;
}
