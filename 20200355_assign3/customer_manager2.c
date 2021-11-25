/* Jihwan Shin / Assignemtn3 / customer_manager2.c */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

#define HASH_TABLE_SIZE 1024
enum {HASH_MULTIPLIER = 65599};

// UserInfo stores customer data
struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
  struct UserInfo* next_name;   // next address for hashName table
  struct UserInfo* next_id;     // next address for hashID table
};

// DB stores hash table data
struct DB {
  struct UserInfo **hashName;   //  hash table by name
  struct UserInfo **hashID;     //  hash table by id
  int curTableSize;        //   size of current hash table
  int numItems;            // # of stored items
			     // # whether the array should be expanded
			     // # or not
};

static int hash_function(const char *pcKey, int iBucketCount)
{
    int i;
    unsigned int uiHash = 0U;
    for(i=0;pcKey[i]!='\0';i++)
        uiHash = uiHash * (unsigned int)HASH_MULTIPLIER 
                    + (unsigned int)pcKey[i];
    return (int)(uiHash % (unsigned int)iBucketCount);
}

/*--------------------------------------------------------------------*/

DB_T CreateCustomerDB(void)
{
    DB_T d;
    // Allocate memory for d
    d = (DB_T) calloc(1,sizeof(struct DB));
    // Allocation failure
    if(d==NULL){
        // print to stderr
        fprintf(stderr,"Can't allocate a memory of DB_T\n");
        return NULL;
    }
    d->curTableSize = HASH_TABLE_SIZE;
    // Allocate memory for hash id table
    d->hashID = (struct UserInfo**)calloc(d->curTableSize,
                                            sizeof(struct UserInfo));
    // Allocation failure: free memory
    if(d->hashID==NULL){
        // print to stderr
        fprintf(stderr,"Can't allocate a memory for array of size %d\n",
        d->curTableSize);
        free(d);
        d=NULL;
        return NULL;
    }

    // Allocate memory for hash id table
    d->hashName = (struct UserInfo**)calloc(d->curTableSize,
                                            sizeof(struct UserInfo));
    // Allocation failure: free memory
    if(d->hashName==NULL){
        fprintf(stderr,"Can't allocate a memory for array of size %d\n",
        d->curTableSize);
        free(d);
        d=NULL;
        return NULL;
    }
    return d;
}
/*--------------------------------------------------------------------*/
void DestroyCustomerDB(DB_T d)
{
    assert(d!=NULL);    // Error if d is NULL
    struct UserInfo *p, *nextp=NULL;
    if(d->hashName!=NULL){
        // Iterate through hash table and its elements
        for(int i=0;i<d->curTableSize;i++){
            for(p=d->hashName[i]; p!=NULL; p=nextp){
                // free UserInfo memory of each customer data
                nextp = p->next_name;
                free(p->name);
                free(p->id);
                p->next_id = NULL;
                p->next_name = NULL;
                p->name = NULL;
                p->id = NULL;
                p->purchase=0;
                free(p);
            }
            // initialize pointer
            d->hashName[i] = NULL;
            d->hashID[i] = NULL;
        }
        // free hash name table and initialize
        free(d->hashName);
        d->hashName = NULL;
    }
    // free hash id table and initialize
    if(d->hashID!=NULL){
        free(d->hashID);
        d->hashID = NULL;
    }
    // free DB and initialize
    free(d);
    d=NULL;
    return;
}
/*--------------------------------------------------------------------*/
int RegisterCustomer(DB_T d, const char *id, 
            const char *name, const int purchase)
{
    // error: return -1
    if(d==NULL||id==NULL||name==NULL||purchase<=0)
        return -1;
    // evaluate hash keys for each table
    int key_name = hash_function(name,d->curTableSize);
    int key_id = hash_function(id,d->curTableSize);
    struct UserInfo *p;
    // search if d already has customer data by name
    for(p=d->hashName[key_name];p!=NULL;p=p->next_name){
        if(p->name == NULL) continue;
        if(strcmp(name,p->name)==0)   return -1;
    }
    // search if d already has customer data by id
    for(p=d->hashID[key_id];p!=NULL;p=p->next_id){
        if(p->id == NULL)   continue;
        if(strcmp(id,p->id)==0) return -1;
    }
    
    /*Hash Table Expansion*/
    if(d->numItems >= (d->curTableSize)*3/4 && d->numItems<1048576){
        /* Create Temporary Expanded DB_T*/
        DB_T expanded_d;    
        expanded_d = (DB_T)calloc(1,sizeof(struct DB));
            //check for allocation failure
        if(expanded_d==NULL){
            // print to stderr
            fprintf(stderr,"Can't allocate a memory for DB_T");
            return -1;
        }
        // double curTableSize
        expanded_d->curTableSize = 2*(d->curTableSize);
        expanded_d->numItems = 0;

        /*Create hashID table*/
        expanded_d->hashID = (struct UserInfo**)
            calloc(expanded_d->curTableSize,sizeof(struct UserInfo));
        // free data and initialize if allocation failed
        if(expanded_d->hashID==NULL){
            // print to stderr
            fprintf(stderr,
            "Can't allocate a memory for array of size %d\n",
                                                d->curTableSize);
            expanded_d->curTableSize = 0;
            expanded_d->numItems = 0;
            free(expanded_d);
            expanded_d = NULL;
            return -1;
        }
        /*Create hashName table*/
        expanded_d->hashName = (struct UserInfo**)
            calloc(expanded_d->curTableSize,sizeof(struct UserInfo));
        //free and initizlize memory if allocation failed
        if(expanded_d->hashName==NULL){
            // print to stderr
            fprintf(stderr,
            "Can't allocate a memory for array of size %d\n",
                    d->curTableSize);
            if(expanded_d->hashID!=NULL){
                for(int i=0;i<expanded_d->curTableSize;i++){
                    struct UserInfo *temp, *next_temp;
                    // free UserInfo memory and initialize
                    for(temp=expanded_d->hashID[i];
                                    temp!=NULL;temp=next_temp){
                        next_temp = temp->next_id;
                        free(temp->name);
                        free(temp->id);
                        free(temp->next_id);
                        free(temp->next_name);
                        temp->name = NULL;
                        temp->id = NULL;
                        temp->next_id = NULL;
                        temp->next_name = NULL;
                        temp->purchase = 0;
                        free(temp);
                        temp = NULL;
                    }
                }
                // free and initialize hash id table
                free(expanded_d->hashID);
                expanded_d->hashID = NULL;
            }
            // free and initialize expanded_d
            expanded_d->curTableSize=0;
            expanded_d->numItems=0;
            free(expanded_d);
            expanded_d=NULL;
            return -1;
        }

        /*Register all data to expanded_d*/
        for(int i=0;i<d->curTableSize;i++){
            struct UserInfo *p, *nextp;
            for(p=d->hashID[i];p!=NULL;p=nextp){
                nextp = p->next_id;
                //copy customer data
                RegisterCustomer(expanded_d,
                                p->id,p->name,p->purchase);
                // free and initialize UserInfo memory
                free(p->id);
                free(p->name);
                p->id = NULL;
                p->name = NULL;
                p->next_id = NULL;
                p->next_name = NULL;
                p->purchase = 0;
                free(p);
                p=NULL;
            }
            d->hashID[i] = NULL;
            d->hashName[i] = NULL;
        }
        // make d point to expanded_d
        free(d->hashID);
        free(d->hashName);
        d->hashID = expanded_d->hashID;
        d->hashName = expanded_d->hashName;
        d->curTableSize = expanded_d->curTableSize;
        d->numItems = expanded_d->numItems;
        free(expanded_d);
        expanded_d = NULL;

        //update hash keys
        key_id = hash_function(id,d->curTableSize);   
        key_name = hash_function(name,d->curTableSize);
    }

    /* Register customer data */
    struct UserInfo *q = (struct UserInfo*)
                                calloc(1,sizeof(struct UserInfo));
    q->id = strdup(id);
    q->name = strdup(name);
    q->purchase = purchase;

    q->next_name = d->hashName[key_name];
    q->next_id = d->hashID[key_id];

    d->hashName[key_name] = q;
    d->hashID[key_id] = q;
    
    d->numItems++;
    return 0;
}
/*--------------------------------------------------------------------*/
int UnregisterCustomerByID(DB_T d, const char *id)
{   
    // error: return -1
    if(d==NULL||id==NULL) return -1;
    // evaluate hash key for hash table by id
    int key_id = hash_function(id,d->curTableSize);
    struct UserInfo *p, *q;     // current customer in each hash table
    struct UserInfo* prev_id;   // previous customer in id hash table
    struct UserInfo* prev_name; // prveious customer in name hash table
    
    // search for id in hash id table
    for(p=d->hashID[key_id];p!=NULL;p=p->next_id){
        if(strcmp(id,p->id)==0){    // search success
            // evaluate hash name key for this customer data
            int key_name = hash_function(p->name,d->curTableSize);
            // search for name in hash name table
            for(q=d->hashName[key_name];q!=NULL;q=q->next_name){
                if(strcmp(p->name,q->name)==0){ // search success
                    // located at head of hashName table
                    if(q==d->hashName[key_name])
                        d->hashName[key_name] = q->next_name;
                    else
                        prev_name->next_name = q->next_name;
                }
                // update prev_name
                prev_name = q;
            }
            // located at head of hashID table
            if(p==d->hashID[key_id])
                d->hashID[key_id] = p->next_id;
            else
                prev_id->next_id = p->next_id;
            // free and initialize matched customer data
            free(p->name);
            free(p->id);
            p->name = NULL;
            p->id = NULL;
            p->next_name = NULL;
            p->next_id = NULL;
            p->purchase = 0;
            free(p);
            p=NULL;
            d->numItems--;
            return 0;
        }
        // update prev_id
        prev_id = p;
    }
    return -1;
}
/*--------------------------------------------------------------------*/
int UnregisterCustomerByName(DB_T d, const char *name)
{
    // error: return -1
    if(d==NULL||name==NULL) return -1;
    // evaluate hash key for hash table by name
    int key_name = hash_function(name,d->curTableSize);
    struct UserInfo *p, *q;     // current customer in each hash table
    struct UserInfo* prev_id;   // previous customer in id hash table
    struct UserInfo* prev_name; // previous customer in name hash table

    // search for name in hash name table
    for(p=d->hashName[key_name];p!=NULL;p=p->next_name){
        if(strcmp(name,p->name)==0){    // search success
            // evaluate hash id key for this customer data
            int key_id=hash_function(p->id,d->curTableSize);
            // search for id in hash id table
            for(q=d->hashID[key_id];q!=NULL;q=q->next_id){
                if(strcmp(p->id,q->id)==0){ // search success
                    // located at head of hashID table
                    if(d->hashID[key_id]==q)
                        d->hashID[key_id] = q->next_id;
                    else
                        prev_id->next_id = q->next_id;
                }
                // update prev_id
                prev_id = q;
            }
            // located at head of hashName table
            if(d->hashName[key_name]==p)
                d->hashName[key_name] = p->next_name;
            else
                prev_name->next_name = p->next_name;
            // free and initialize matched customer data
            free(p->name);
            free(p->id);
            p->name = NULL;
            p->id = NULL;
            p->next_name = NULL;
            p->next_id = NULL;
            p->purchase = 0;
            free(p);
            p=NULL;
            d->numItems--;
            return 0;
        }
        // update prev_name
        prev_name = p;
    }
    return -1;
}
/*--------------------------------------------------------------------*/
int GetPurchaseByID(DB_T d, const char* id)
{
    // error: return -1
    if(d==NULL||id==NULL) return -1;
    // evaluate hash key for id
    int key_id = hash_function(id,d->curTableSize);
    struct UserInfo* p;
    // search for customer by id
    for(p=d->hashID[key_id];p!=NULL;p=p->next_id){
        if(strcmp(id,p->id)==0) // search success
            return p->purchase;
    }
    return -1;      // search failure
}
/*--------------------------------------------------------------------*/
int GetPurchaseByName(DB_T d, const char* name)
{
    // error: return -1
    if(d==NULL||name==NULL) return -1;
    // evaluate hash key for name
    int key_name = hash_function(name,d->curTableSize);
    struct UserInfo* p;
    // search for customer by name
    for(p=d->hashName[key_name];p!=NULL;p=p->next_name){
        if(strcmp(name,p->name)==0) // search success
            return p->purchase;
    }
    return -1;      // search failure
}
/*--------------------------------------------------------------------*/
int GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp){
    int sum=0;
    // iterate through hash ID table and all of each elements
    for(int i=0;i<d->curTableSize;i++){
        struct UserInfo* p;
        for(p=d->hashID[i];p!=NULL;p=p->next_id){
            // add fp to sum
            sum+= fp(p->id,p->name,p->purchase);
        }
    }
    return sum;
}