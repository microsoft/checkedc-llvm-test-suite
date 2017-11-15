/* For copyright information, see olden_v1.0/COPYRIGHT */

/* tree.h
 */

#include <stdio_checked.h>
#include <stdlib_checked.h>
#include <string_checked.h>
#include <stdchecked.h>

#pragma BOUNDS_CHECKED ON

#ifdef TORONTO
#define chatting(...) _Unchecked { printf(__VA_ARGS__); }
#define PLAIN
#endif

typedef struct tree {
    int		val;
    ptr<struct tree> left;
    ptr<struct tree> right;
} tree_t;

extern ptr<tree_t> TreeAlloc(int level, int lo, int hi);
int TreeAdd (ptr<tree_t> t);






#pragma BOUNDS_CHECKED OFF
