/* Jihwan Shin / Assignemtn3 / customer_manager1.c */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"



#define UNIT_ARRAY_SIZE 1024

// UserInfo stores customer data
struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
};

// DB stores array data
struct DB {
  struct UserInfo *pArray;   // pointer to the array
  int curArrSize;            // current array size (max # of elements)
  int numItems;              // # of stored items, needed to determine
			     // # whether the array should be expanded
			     // # or not
};

/*--------------------------------------------------------------------*/
DB_T
CreateCustomerDB(void)
{
  DB_T d;
  
  d = (DB_T) calloc(1, sizeof(struct DB));
  // Allocation Fail
  if (d == NULL) {
    // print to stderr
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }
  d->curArrSize = UNIT_ARRAY_SIZE; // start with 1024 elements
  d->pArray = (struct UserInfo *)calloc(d->curArrSize,
               sizeof(struct UserInfo));
  // Allocation Fail
  if (d->pArray == NULL) {
    // print to stderr
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->curArrSize);   
    free(d);
    d=NULL;
    return NULL;
  }
  return d;
}
/*--------------------------------------------------------------------*/
void
DestroyCustomerDB(DB_T d)
{
  /* fill out this function */
  assert(d!=NULL);  // error if d is NULL
  if(d->pArray){    // if array is not NULL
    for(int i=0;i<d->curArrSize;i++){
      // free customer data in each element of the array
      if(d->pArray[i].id!=NULL){
        free(d->pArray[i].id);
        d->pArray[i].id = NULL;
        free(d->pArray[i].name);
        d->pArray[i].name = NULL;
        d->pArray[i].purchase = 0;
      }
    }
    // free elements in d
    free(d->pArray);
    d->pArray = NULL;
    d->curArrSize = 0;
    d->numItems = 0;
  }
  // free d itself
  free(d);
  d = NULL;
  return;
}
/*--------------------------------------------------------------------*/
int
RegisterCustomer(DB_T d, const char *id,
		 const char *name, const int purchase)
{
  /* fill out this function */
  // return -1 for failures
  if(d==NULL || id==NULL || name==NULL || purchase<=0)  
    return -1;

  // return -1 if id or name already exists
  for(int i=0;i<d->curArrSize;i++){
    if(d->pArray[i].name == NULL) continue;
    if(strcmp(id,d->pArray[i].id)==0) return -1;
    if(strcmp(name,d->pArray[i].name)==0) return -1;
  }
  //expand memory if there is no empty space
  if(d->curArrSize == d->numItems){
    d->curArrSize = d->curArrSize + UNIT_ARRAY_SIZE;
    d->pArray = (struct UserInfo *) realloc(d->pArray,
                      d->curArrSize * sizeof(struct UserInfo));
    }
  // register element to an empty space
  for(int i=0;i<d->curArrSize;i++){
    if(d->pArray[i].name==NULL){
      d->pArray[i].id = strdup(id);
      d->pArray[i].name = strdup(name);
      d->pArray[i].purchase = purchase;
      d->numItems++;
      return 0;
    }
  }
  return -1;
}
/*--------------------------------------------------------------------*/
int
UnregisterCustomerByID(DB_T d, const char *id)
{
  /* fill out this function */
  // error: return -1
  if( d==NULL || id==NULL ) return -1;
  // search for customer by id
  for(int i=0;i<d->curArrSize;i++){
    if(d->pArray[i].id==NULL) continue;
    if(strcmp(id,d->pArray[i].id)==0){  // search success
      // unregister customer data
      free(d->pArray[i].name);
      free(d->pArray[i].id);
      d->pArray[i].name = NULL;
      d->pArray[i].id = NULL;
      d->numItems = d->numItems-1;
      return 0;
    }
  }
  return -1;  // search failure
}

/*--------------------------------------------------------------------*/
int
UnregisterCustomerByName(DB_T d, const char *name)
{
  /* fill out this function */
  // error: return -1
  if (d==NULL || name==NULL ) return -1;
  // search for customer by name
  for(int i=0;i<d->curArrSize;i++){
    if(d->pArray[i].name==NULL) continue;
    if(strcmp(name,d->pArray[i].name)==0){  // search success
      // unregister customer data
      free(d->pArray[i].name);
      free(d->pArray[i].id);
      d->pArray[i].purchase = 0;
      d->pArray[i].name = NULL;
      d->pArray[i].id = NULL;
      d->numItems--;
      return 0;
    }
  }
  return -1;  // search failure
}
/*--------------------------------------------------------------------*/
int
GetPurchaseByID(DB_T d, const char* id)
{
  /* fill out this function */
  // error: return -1
  if( d==NULL || id==NULL ) return -1;
  // search for customer by id
  for(int i=0;i<d->curArrSize;i++){
    if(d->pArray[i].name==NULL) continue;
    if(strcmp(id,d->pArray[i].id)==0) // search success
      return d->pArray[i].purchase;
  }
  return -1;  // search failure
}
/*--------------------------------------------------------------------*/
int
GetPurchaseByName(DB_T d, const char* name)
{
  /* fill out this function */
  // error: return -1
  if( d==NULL || name==NULL ) return -1;
  // search for customer by name
  for(int i=0;i<d->curArrSize;i++){
    if(d->pArray[i].name==NULL) continue;
    if(strcmp(name,d->pArray[i].name)==0) // search success
      return d->pArray[i].purchase;
  }
  return -1;  // search failure
}
/*--------------------------------------------------------------------*/
int
GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  /* fill out this function */
  int sum=0;
  // iterate through whole array
  for(int i=0;i<d->curArrSize;i++){
    if(d->pArray[i].name==NULL) continue; // skip empty array indices
    // add fp to sum
    sum += fp(d->pArray[i].id,d->pArray[i].name,d->pArray[i].purchase);
  }
  return sum;
}
