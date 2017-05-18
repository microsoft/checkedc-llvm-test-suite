/* For copyright information, see olden_v1.0/COPYRIGHT */

#include <stdchecked.h>
#include <stdio_checked.h>

#define NULL 0

struct hash_entry {
  unsigned int key;
  void *entry;
  ptr<struct hash_entry> next;
};

#pragma BOUNDS_CHECKED ON

typedef ptr<struct hash_entry> HashEntry;

struct hash {
  array_ptr<HashEntry> array : count(size);
  ptr<int(unsigned int)> mapfunc;
  int size;
};

typedef ptr<struct hash> Hash;

Hash MakeHash(int size, ptr<int(unsigned int)> map);
unchecked void *HashLookup(unsigned int key, Hash hash);
unchecked void HashInsert(void *entry, unsigned int key, Hash hash);
void HashDelete(unsigned int key, Hash hash);

#pragma BOUNDS_CHECKED OFF
