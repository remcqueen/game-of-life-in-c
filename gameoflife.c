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
    if (argv[counter][0] == '-')
    {
      // Switch case for each flag given
      switch (argv[counter][1])
      {
      case 'i':

        // checks if -i was given as final argument
        if (counter + 2 > argc)
        {
          fprintf(stderr, "Error: Input file not specified\n");
          exit(128);
        }

        // if -i flag was already given, checks if the filename was the same
        if (i != NULL)
        {
          int difference = strcmp(i, argv[counter + 1]);

          if (difference)
          {
            fprintf(stderr, "Error: Switch -i has two inconsistant entries: %s and %s\n", i, argv[counter + 1]);
            exit(128);
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
          exit(128);
        }

        // increase counter to skip checking next argument
        counter++;

        break;

      case 'o':

        // checks if -i was given as final argument
        if (counter + 2 > argc)
        {
          fprintf(stderr, "\nError: Output file not specified\n");
          exit(128);
        }

        // change to check if new -o is the same file
        if (o != NULL)
        {
          int difference = strcmp(o, argv[counter + 1]);

          if (difference)
          {
            fprintf(stderr, "Error: Switch -o has two inconsistant entries: %s and %s\n", o, argv[counter + 1]);
            exit(128);
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
          fprintf(stderr, "Error: Number of generations not specified\nExiting...\n");
          exit(128);
        }

        // checks if -g switch has been given before
        // checks if previous value of g matches new value
        if (g != -1)
        {
          int same = g == atoi(argv[counter + 1]);

          if (!same)
          {
            fprintf(stderr, "Error: Switch -g has two inconsistant entries: %d and %s\n", g, argv[counter + 1]);
            exit(128);
          }
          else if (same)
          {
            break;
          }
        }

        // // Checks string length will not overflow lengthOfString
        // // Checks for zero if length exceeds long storage and causes strtol to return 0
        // if (strtol(strlen(argv[counter + 1]), NULL, 0) > INT_MAX || strtol(strlen(argv[counter + 1]), NULL, 0) == 0)
        // {
        //   fprintf(stderr, "Error: Length of generations given exceeds max integer storage\n");
        //   exit(1);
        // }

        int lengthOfString = strlen(argv[counter + 1]);

        // checks each charecter in argument for if it lies it is a number
        for (int j = 0; j < lengthOfString; j++)
        {
          int integerConvert = (int)argv[counter + 1][j];
          if (integerConvert < 48 || integerConvert > 57)
          {
            fprintf(stderr, "Error: Number of generations must be an integer\n");
            exit(128);
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
        exit(128);
      }
    }

    // error check for unknown commands given as input
    else
    {
      fprintf(stderr, "Error: Invalid command line switch given\n");
      exit(128);
    }
  }

  // checks if stdin has stored a file
  fseek(stdin, 0, SEEK_END);
  int len = ftell(stdin);
  rewind(stdin);
  // checks if -i and stdin both were given
  if (i && (len != 0))
  {
    fprintf(stderr, "Error: stdin and -i switch both given\n");
    exit(128);
  }

  // checks if stdout has stored a file
  fseek(stdout, 0, SEEK_END);
  len = ftell(stdout);
  rewind(stdout);
  // checks if -o and stdout were both given
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

  // itterates through g and evolves
  if (g == -1)
  {
    g = 5;
  }

  printf("Performing evolves\n");

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

  printf("Performing writeout\n");

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
