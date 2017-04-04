/* For copyright information, see olden_v1.0/COPYRIGHT */

/* build.c
 *
 * By:  Martin C. Carlisle
 * 6/15/94
 * builds the tree for the Power Pricing problem
 *
 * based on code by:  Steve Lumetta, Sherry Li, and Ismail Khalil
 * University of California at Berkeley
 */

#include "power.h"
// CHECKEDC : checked c header
#include <stdlib_checked.h>

// CHECKEDC : function without a prototype cannot have checked return type
Root build_tree(void)
{
  int i;
  // CHECKEDC : automatic checked pointer must have a initializer
  Root t = 0;
  Lateral l = 0;

  t = (Root) calloc(1, sizeof(*t));

  for (i=0; i<NUM_FEEDERS; i++) {
    /* Insert future here, split into two loops */
    l = build_lateral(i*LATERALS_PER_FEEDER,LATERALS_PER_FEEDER);
    // CHECKEDC : checked array type dereference, feeders checked[NUM_FEEDERS]
    // dynamic_check(i >= 0 && i < NUM_FEEDERS);
    // : redundant dynamic_check can be removed (reasoning facts)
    t->feeders[i]=l;
  }
  t->theta_R = 0.8;
  t->theta_I = 0.16;
  return t;
}

Lateral build_lateral(int i, int num)
{
  // CHECKEDC : automatic checked pointer must have a initializer
  Lateral l = 0;
  Branch b = 0;
  Lateral next = 0;
 
  if (num == 0) return NULL;
  l = (Lateral) calloc(1, sizeof(*l));

  next = build_lateral(i,num-1);
  b = build_branch(i*BRANCHES_PER_LATERAL,(num-1)*BRANCHES_PER_LATERAL,
    BRANCHES_PER_LATERAL);

  // CHECKEDC : p->m == (*p).m, ptr type dereference, non-null check
  // dynamic_check(l != NULL);
  l->next_lateral = next;
  l->branch = b;
  l->R = 1/300000.0;
  l->X = 0.000001;
  l->alpha = 0.0;
  l->beta = 0.0;
  return l;
}

Branch build_branch(int i, int j, int num)
{
  // CHECKEDC : automatic checked pointer must have a initializer
  Leaf l = 0;
  Branch b = 0;

  if (num == 0) return NULL;
  /* allocate branch */
  b = (Branch) calloc(1, sizeof(*b));
  
  /* fill in children */
  // CHECKEDC : p->m == (*p).m, ptr type dereference, non-null check
  // dynamic_check(b != NULL);
  b->next_branch= build_branch(i,j,num-1);

  for (i=0; i<LEAVES_PER_BRANCH; i++) {
    l = build_leaf();
    // CHECKEDC : ptr type dereference (b->..), non-null check
    // && array_ptr array subscript(leaves[]), bounds check
    // dynamic_check(b != NULL);
    // dynamic_check(i >= 0 && i < LEAVES_PER_BRANCH);
    // : compiler can optimize redundant dynamic check (reasoning facts)
    // : non-null check is already done before (previous check)
    b->leaves[i] = l;
  }
  
  /* fill in values */
  // CHECKEDC : ptr type dereference
  // dynamic_check(b != NULL);
  // : reasoning facts optimize this
  b->R = 0.0001;
  b->X = 0.00002;
  b->alpha = 0.0;
  b->beta = 0.0;
  return b;
}

// CHECKEDC : function without a prototype cannot have checked return type
Leaf build_leaf(void) {
  // CHECKEDC : automatic checked pointer must have a initializer
  Leaf l = 0;

  l = (Leaf) calloc(1, sizeof(*l));
  // CHECKEDC : ptr type dereference (l->)
  // dynamic_check(l != NULL);
  l->D.P = 1.0;
  l->D.Q = 1.0;
  return l;
}
