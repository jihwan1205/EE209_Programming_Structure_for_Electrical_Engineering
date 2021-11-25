/*
Name: Jihwan Shin
Student ID: 20200355
Assignment Number: 2
File Name: str.c
*/

#include <assert.h> /* to use assert() */
#include <stdio.h>
#include <stdlib.h> /* for strtol() */
#include <string.h>
#include<ctype.h> /* for strtoLong() */
#include "str.h"

/* Your task is: 
   1. Rewrite the body of "Part 1" functions - remove the current
      body that simply calls the corresponding C standard library
      function.
   2. Write appropriate comment per each function
*/

/* Part 1 */
/*------------------------------------------------------------------------*/
size_t StrGetLength(const char* pcSrc)
{
  const char *pcEnd;
  assert(pcSrc); /* NULL address, 0, and FALSE are identical. */
  pcEnd = pcSrc;
	
  while (*pcEnd) /* null character and FALSE are identical. */
    pcEnd++;

  return (size_t)(pcEnd - pcSrc);
}

/*------------------------------------------------------------------------*/
char *StrCopy(char *pcDest, const char* pcSrc)
{
  /* TODO: fill this function */
  assert(pcDest); //error if pcDest address is NULL
  assert(pcSrc);  //error if pcSrc address is NULL
  size_t i;
  //copy each character of pcSrc to pcDest
  for(i=0;*(pcSrc+i);i++){  
    *(pcDest+i) = *(pcSrc+i);
  }
  *(pcDest+i)='\0';   //add NULL character to last
  return pcDest;
}

/*------------------------------------------------------------------------*/
int StrCompare(const char* pcS1, const char* pcS2)
{
  /* TODO: fill this function */
  assert(pcS1); //error if pcS1 address is NULL
  assert(pcS2); //error if pcS2 address is NULL
  const char *pcC1 = pcS1;
  const char *pcC2 = pcS2;
  while(1){
    if(*pcC1 > *pcC2)  //when character from S1 is larger
      return 1;
    else if(*pcC1 < *pcC2)  //when character from S1 is smaller
      return -1;
    else if(!*pcC1 && !*pcC2) //when character is NULL for both characters
      break;
    else{    //when character is same but not NULL
      pcC1++;
      pcC2++;
    }
  }
  return 0; 
}
/*------------------------------------------------------------------------*/
char *StrFindChr(const char* pcHaystack, int c)
{
  /* TODO: fill this function */
  assert(pcHaystack);   //error if address is NULL

  if(c=='\0')   //when c is NULL character
    return (char*)(pcHaystack+StrGetLength(pcHaystack)); 
  //compare each charcter of pcHayStack with c
  for(size_t i=0;*(pcHaystack+i);i++){   
    if(*(pcHaystack+i)==c)
      return (char*)(pcHaystack+i);
  }
  return NULL;
}
/*------------------------------------------------------------------------*/
char *StrFindStr(const char* pcHaystack, const char *pcNeedle)
{
  /* TODO: fill this function */
  assert(pcHaystack);   //error if address is NULL
  assert(pcNeedle);     //error if address is NULL
  
  if(*pcNeedle=='\0') //when pcNeedle is empty string
    return (char*)pcHaystack;
  for(size_t i=0;*(pcHaystack+i);i++){
    //when starting character of pcNeedle is found
    for(size_t j=0;*(pcHaystack+i+j)==*(pcNeedle+j);j++){
      if(!*(pcNeedle+j+1)) 
        return (char*)(pcHaystack+i); 
    } //no return means string pcNeedle is not found
    //compare following char of pcHayStack with start of pcNeedle
  }
  return NULL;
}
/*------------------------------------------------------------------------*/
char *StrConcat(char *pcDest, const char* pcSrc)
{
  /* TODO: fill this function */
  assert(pcDest);   //error if address is NULL
  assert(pcSrc);    //error if address is NULL
  //copy pcSrc to the first NULL character is pcDest
  StrCopy(pcDest+StrGetLength(pcDest),pcSrc); 
  return pcDest;
}

/*------------------------------------------------------------------------*/
long int StrToLong(const char *nptr, char **endptr, int base)
{
  /* handle only when base is 10 */
  if (base != 10) return 0;

  /* TODO: fill this function */
  assert(nptr);
  long int returnVal = 0; 
  char *nptr2 = (char*)nptr;
  int neg = 0;    //identify whether returnVal is negative

  //skip whitespace
  while(*nptr2 && isspace(*nptr2)){
    nptr2++;
  }

  //check for sign
  if(*nptr2 == '+')
    nptr2++;
  else if(*nptr2 == '-'){
    neg = 1;
    nptr2++;
  }
  //skip '0'
  while(*nptr2 && *nptr2=='0'){
    nptr2++;
  }

  //check and evaluate the digits
  while(*nptr2 && isdigit(*nptr2)){
    int temp = (int)*nptr2-'0';
    if(!neg && (LONG_MAX-temp)/10<returnVal-1) 
      return LONG_MAX; //OVERFLOW
    else if(neg && (LONG_MIN+temp)/10>-returnVal) 
      return LONG_MIN;  //UNDERFLOW
    returnVal = 10*returnVal + temp;
    nptr2++;
  }

  if(endptr) *endptr = nptr2;   //nptr is not NULL
  
  if(neg) returnVal = -returnVal;

  return returnVal;
}