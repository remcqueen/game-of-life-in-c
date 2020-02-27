#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gol.h"
int main(int argc, char *argv[])
{
  int g = -1, s = 0, t = 0, lengthOfFile;
  char *i = NULL, *o = NULL;
  FILE *inputFile = NULL;
  FILE *outputFile = NULL;

  for (int counter = 0; counter < argc; counter++)
  {
    if (argv[counter][0] == '-')
    {
      switch (argv[counter][1])
      {
      case 'i':

        if (counter + 2 > argc)
        {
          fprintf(stderr, "Error: input file not specified\n");
          exit(128);
        }

        if (i != NULL)
        {
          int difference = strcmp(i, argv[counter + 1]);

          if (difference)
          {
            fprintf(stderr, "Error: switch -i has two inconsistant entries: %s and %s\n", i, argv[counter + 1]);
            exit(128);
          }
          else if (!difference)
          {
            break;
          }
        }

        i = argv[counter + 1];

        if ((inputFile = fopen(i, "r")) == NULL)
        {
          fprintf(stderr, "Error: could not open input file with name %s\n", i);
          exit(128);
        }

        break;

      case 'o':

        if (counter + 2 > argc)
        {
          fprintf(stderr, "\nError: output file not specified\n");
          exit(128);
        }

        // change to check if new -o is the same file
        if (o != NULL)
        {
          int difference = strcmp(o, argv[counter + 1]);

          if (difference)
          {
            fprintf(stderr, "Error: switch -o has two inconsistant entries: %s and %s\n", o, argv[counter + 1]);
            exit(128);
          }
          else if (!difference)
          {
            break;
          }
        }

        lengthOfFile = strlen(argv[counter + 1]);

        o = argv[counter + 1];

        // o[lengthOfFile] = '\0';
        break;

      case 'g':

        if (counter + 2 > argc)
        {
          fprintf(stderr, "Error: number of generations not specified\nExiting...\n");
          exit(128);
        }

        if (g != -1)
        {
          int same = g == atoi(argv[counter + 1]);

          if (!same)
          {
            fprintf(stderr, "Error: switch -g has two inconsistant entries: %d and %s\n", g, argv[counter + 1]);
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
            fprintf(stderr, "Error: number of generations must be an integer\n");
            exit(128);
          }
        }

        g = atoi(argv[counter + 1]);
        break;

      case 's':

        s = 1;
        break;

      case 't':

        t = 1;
        break;
      }
    }
  }

  fseek(stdin, 0, SEEK_END);
  int len = ftell(stdin);
  rewind(stdin);
  if (i && (len != 0))
  {
    fprintf(stderr, "Error: stdin and -i switch both given\n");
    exit(128);
  }

  fseek(stdout, 0, SEEK_END);
  len = ftell(stdout);
  rewind(stdout);
  if (o && (len != 0))
  {
    fprintf(stderr, "Error: stdout and -o switch both given\n");
    exit(128);
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
