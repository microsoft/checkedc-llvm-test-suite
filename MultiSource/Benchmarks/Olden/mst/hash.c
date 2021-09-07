/* For copyright information, see olden_v1.0/COPYRIGHT */

#include <stdchecked.h>
#include <stdlib.h>
#include "hash.h"
#pragma CHECKED_SCOPE ON

#define printf(...) unchecked { printf(__VA_ARGS__); }
#define assert(num,a) if (!(a)) {printf("Assertion failure:%d in hash\n",num); exit(-1);}
#include "hacks.h"

static int remaining = 0;
static array_ptr<char> temp : count(remaining);

static array_ptr<void> localmalloc(int size) : byte_count(size)
{
  array_ptr<void> blah : byte_count(size) = 0;
  
  if (size>remaining) 
    {
      remaining = 32768;
      temp = calloc<char>(remaining, sizeof(char));
      if (!temp) printf("Error! malloc returns null\n");
    }
  blah = _Dynamic_bounds_cast<array_ptr<void>>(temp, byte_count(size));
  temp += size;
  remaining -= size;
  return blah;
}

#define localfree(sz)

Hash MakeHash(int size, ptr<int(unsigned int)> map)
{
  Hash retval = NULL;
  int i;

  retval = (Hash) localmalloc(sizeof(*retval));
  retval->size = size,
    retval->array = (array_ptr<HashEntry>)localmalloc(size*sizeof(HashEntry));
  for (i=0; i<size; i++)
    retval->array[i] = NULL;
  retval->mapfunc = map;
  return retval;
}

unchecked void *HashLookup(unsigned int key, Hash hash)
{
  int j;
  HashEntry ent = NULL;

  j = (hash->mapfunc)(key);        /* 14% miss in hash->mapfunc */  
  assert(1,j>=0);
  assert(2,j<hash->size);
  for (ent = hash->array[j];       /* 17% miss in hash->array[j] */ /* adt_pf can't detect :( */
       ent &&                      /* 47% miss in ent->key */       /* adt_pf can detect :) */
           ent->key!=key; 
       ent=ent->next);             /* 8% miss in ent->next */       /* adt_pf can detect :) */
  if (ent) return ent->entry;
  return NULL;
}

unchecked void HashInsert(void *entry,unsigned int key,Hash hash)
{
  HashEntry ent = NULL;
  int j;
  
  assert(3,!HashLookup(key,hash));
  
  j = (hash->mapfunc)(key);
  ent = (HashEntry)localmalloc(sizeof(*ent));
  ent->next = hash->array[j];
  hash->array[j]=ent;
  ent->key = key;
  ent->entry = entry;
}

void HashDelete(unsigned key, Hash hash) {
  HashEntry tmp = NULL;
  int j = (hash->mapfunc)(key);
  int size = hash->size;
  _Dynamic_check(j <= size);
  UncheckedPtrInit(ptr<HashEntry>, ent, &hash->array[j]);

  while (*ent && (*ent)->key != key) {
    ent = &(*ent)->next;
  }

  assert(4, *ent);

  tmp = *ent;
  HashEntry temp_entry = (*ent)->next;
  *ent = temp_entry;
  localfree(tmp);
}

  
  


