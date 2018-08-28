#include <stdio_checked.h>
#include <stdlib_checked.h>
#include <string_checked.h>
#include <stdchecked.h>

#pragma CHECKED_SCOPE ON

#ifdef TORONTO
/* Toronto's hack */
#define ALLOC(p, sz)      calloc(1, sz)
#define ALLOC(T, p, sz)      calloc<T>(1, sz)
#define chatting(...) _Unchecked { printf(__VA_ARGS__); } 
extern int NumNodes, NDim;
#endif

extern int flag;

int atoi(const char * : itype(nt_array_ptr<const char>));
int dealwithargs(int argc, array_ptr<nt_array_ptr<char>> argv : count(argc));

/* For copyright information, see olden_v1.0/COPYRIGHT */

struct tree {
  int sz;
  double x,y;
  ptr<struct tree> left;
  ptr<struct tree> right;
#ifdef TORONTO
  ptr<struct tree> next;
  ptr<struct tree> prev;
#else
  ptr<struct tree> next {95};
  ptr<struct tree> prev {95};
#endif

};

typedef ptr<struct tree> Tree;

#ifdef ORDER

#define TREE_SIZE_LOGICAL    1024    /* number of nodes allocated each time (logically) whenever the current tree is full */
#define NODES_PF_AHEAD       5       /* prefetch distances (in terms of tree nodes) */
#define TREE_SIZE_PHYSICAL   (TREE_SIZE_LOGICAL + NODES_PF_AHEAD) /* actually size of a tree. We need to add NODES_PF_AHEAD
								     nodes to prevent prefetching invalid addresses. This
								     will be not necessary if we have non-excepting pfs */
#endif

/* Builds a 2D tree of n nodes in specified range with dir as primary 
   axis (0 for x, 1 for y) */
Tree build_tree(int n,int dir,int lo,int num_proc,double min_x,
                double max_x,double min_y,double max_y);
/* Compute TSP for the tree t -- use conquer for problems <= sz */
Tree tsp(Tree t, int sz, int nproc);















#pragma CHECKED_SCOPE OFF
