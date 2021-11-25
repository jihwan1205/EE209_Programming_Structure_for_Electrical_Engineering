/*
Name: Jihwan Shin
Studnet ID: 20200355
Assignment Number: 2
File Name: sgrep.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for skeleton code */
#include <unistd.h> /* for getopt */
#include "str.h"

#define MAX_STR_LEN 1023

#define FALSE 0
#define TRUE  1

/*
 * Fill out your own functions here (If you need)
 */

/*--------------------------------------------------------------------*/
/* PrintUsage()
   print out the usage of the Simple Grep Program                     */
/*--------------------------------------------------------------------*/
void
PrintUsage(const char* argv0)
{
  const static char *fmt =
	  "Simple Grep (sgrep) Usage:\n"
	  "%s pattern [stdin]\n";

  printf(fmt, argv0);
}
/*-------------------------------------------------------------------*/
/* SearchPattern()
   Your task:
   1. Do argument validation
   - String or file argument length is no more than 1023
   - If you encounter a command-line argument that's too long,
   print out "Error: pattern is too long"

   2. Read the each line from standard input (stdin)
   - If you encounter a line larger than 1023 bytes,
   print out "Error: input line is too long"
   - Error message should be printed out to standard error (stderr)

   3. Check & print out the line contains a given string (search-string)

   Tips:
   - fgets() is an useful function to read characters from file. Note
   that the fget() reads until newline or the end-of-file is reached.
   - fprintf(sderr, ...) should be useful for printing out error
   message to standard error

   NOTE: If there is any problem, return FALSE; if not, return TRUE  */
/*-------------------------------------------------------------------*/
/* recursive function for pattern with star */
int SearchPattern2(char* buf, char* pattern){
  char *PATTERN = StrFindChr(pattern,'*');
  //case when pattern has no *
  if(!PATTERN){
    //search for pattern in buf
    if(StrFindStr(buf,pattern))
      return TRUE;
    else
      return FALSE;
  }
  //case when pattern has *
  else{
    //slice pattern based on *
    *PATTERN = '\0';
    PATTERN++;  //move address to sliced pattern(right)
    //search for sliced pattern(left) in buf
    char* BUF = StrFindStr(buf,pattern);
    if(!BUF)  //sliced pattern(left) not found
      return FALSE;
    //move buf address to end of sliced pattern(left)
    BUF+=StrGetLength(pattern);
    return SearchPattern2(BUF,PATTERN);
  }
}
int
SearchPattern(const char *pattern)
{
  char buf[MAX_STR_LEN + 2];

  /*
   *  TODO: check if pattern is too long
   */
  if(StrGetLength(pattern)>MAX_STR_LEN){
    fprintf(stderr,"Error: pattern is too long\n");
    return FALSE;
  }

  /* Read one line at a time from stdin, and process each line */
  while (fgets(buf, sizeof(buf), stdin)) {

    /* TODO: check the length of an input line */
    if(StrGetLength(buf)>MAX_STR_LEN){
      fprintf(stderr,"Error: input line is too long\n");
      return FALSE;
    }
    char* patterncopy = (char*)malloc(StrGetLength(pattern));
    StrCopy(patterncopy,pattern);
    //address of first * in pattern
    char* PATTERN = StrFindChr(patterncopy,'*');
    /* TODO: fill out this function */
    /*  
    case when pattern has no *
    contains both cases when pattern starts with *
    and cases when * is in middle of pattern  
    */
    if(!PATTERN){
      if(StrFindStr(buf,pattern))
        printf("%s",buf);
    }
    //case when pattern has *
    else{
      //slice the pattern based on *
      *PATTERN = '\0';
      PATTERN++;  //pattern points to sliced string(right)
      char* BUF = StrFindStr(buf,patterncopy);
      if(!BUF)    //sliced string(left) not found in buf
        continue;
      //  move pointer to last address of sliced pattern(left)
      BUF += StrGetLength(patterncopy);
      //recursively search for sliced pattern(right) 
      if(SearchPattern2(BUF,PATTERN))
        printf("%s",buf);
      else
        continue;
    }
  }

  return TRUE;
}
/*-------------------------------------------------------------------*/
int
main(const int argc, const char *argv[])
{
  /* Do argument check and parsing */
  if (argc < 2) {
	  fprintf(stderr, "Error: argument parsing error\n");
	  PrintUsage(argv[0]);
	  return (EXIT_FAILURE);
  }

  return SearchPattern(argv[1]) ? EXIT_SUCCESS:EXIT_FAILURE;
}