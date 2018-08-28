/* For copyright information, see olden_v1.0/COPYRIGHT */

/* tree-alloc.c
 */

#ifndef TORONTO
#include "mem-ref.h"
#include "future-cell.h"
#endif

#include "tree.h"

#pragma CHECKED_SCOPE ON

ptr<tree_t> TreeAlloc (int level, int lo, int proc) {
  if (level == 0)
    return NULL;
  else {
    ptr<tree_t> new = NULL, right = NULL, left = NULL;
    new = calloc<tree_t>(1, sizeof(tree_t));
    left = TreeAlloc(level -1, lo+proc/2, proc/2);
    right=TreeAlloc(level-1,lo,proc/2);
    new->val = 1;
    new->left = left;
    new->right = right;
    return new;
  }
}
