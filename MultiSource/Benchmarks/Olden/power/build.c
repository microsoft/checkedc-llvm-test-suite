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

#include <stdlib.h>
#include "power.h"

#pragma CHECKED_SCOPE ON

Root build_tree(void)
{
  int i;
  Root t = 0;
  Lateral l = 0;

  t = calloc<struct root>(1, sizeof(*t));

  for (i=0; i<NUM_FEEDERS; i++) {
    /* Insert future here, split into two loops */
    l = build_lateral(i*LATERALS_PER_FEEDER,LATERALS_PER_FEEDER);
    t->feeders[i]=l;
  }
  t->theta_R = 0.8;
  t->theta_I = 0.16;
  return t;
}

Lateral build_lateral(int i, int num)
{
  Lateral l = 0;
  Branch b = 0;
  Lateral next = 0;
 
  if (num == 0) return NULL;
  l = calloc<struct lateral>(1, sizeof(*l));

  next = build_lateral(i,num-1);
  b = build_branch(i*BRANCHES_PER_LATERAL,(num-1)*BRANCHES_PER_LATERAL,
    BRANCHES_PER_LATERAL);

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
  Leaf l = 0;
  Branch b = 0;

  if (num == 0) return NULL;
  /* allocate branch */
  b = calloc<struct branch>(1, sizeof(*b));
  
  /* fill in children */
  b->next_branch= build_branch(i,j,num-1);

  for (i=0; i<LEAVES_PER_BRANCH; i++) {
    l = build_leaf();
    b->leaves[i] = l;
  }
  
  /* fill in values */
  b->R = 0.0001;
  b->X = 0.00002;
  b->alpha = 0.0;
  b->beta = 0.0;
  return b;
}

Leaf build_leaf(void) {
  Leaf l = 0;

  l = calloc<struct leaf>(1, sizeof(*l));
  l->D.P = 1.0;
  l->D.Q = 1.0;
  return l;
}
