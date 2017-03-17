/* For copyright information, see olden_v1.0/COPYRIGHT */

/* tree.h
 */

#ifdef TORONTO
#include <stdio_checked.h>
#include <stdlib_checked.h>
#include <string_checked.h>
#include <stdchecked.h>
#define chatting printf
#define PLAIN
#endif

typedef struct tree {
    int		val;
    ptr<struct tree> left;
    ptr<struct tree> right;
};

typedef ptr<struct tree> Tree;
extern Tree TreeAlloc(int level, int lo, int hi);
int TreeAdd (Tree t);







