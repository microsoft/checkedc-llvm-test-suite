/* For copyright information, see olden_v1.0/COPYRIGHT */

/* em3d.h - Header file for the electromagnetic problem in 3 dimensions
 *
 * By:  Martin C. Carlisle
 * Date: Feb. 23, 1994
 *
 */

#ifndef EM3D
#define EM3D

#include <stdchecked.h>

void dealwithargs(int argc, array_ptr<char*> argv : count(argc));
void printstats(void);
void srand48(long);
long lrand48(void);

#include <stdio.h>
#include <stdio_checked.h>
#include <stdlib.h>
#include <stdlib_checked.h>

#define chatting printf

extern char * min_ptr;
extern char * max_ptr;

extern int n_nodes; /* number of nodes (E and H) */
extern int d_nodes; /* degree of nodes */
extern int local_p; /* percentage of local edges */
#define PROCS 1

#define assert(a) if (!a) {printf("Assertion failure\n"); exit(-1);}

// CHECKED C: Here we cannot reason about the length of the array_ptrs for
// coeffs and from_values because array_ptrs in structs do not yet work.
// CHECKED C: Here we cannot reason about the length of the array_ptr for 
// to_nodes because that information is not kept in this struct (it's kept
// in a global).
typedef struct node_t {
  ptr<double> value;
  ptr<struct node_t> next;
  ptr<struct node_t>* to_nodes : itype(array_ptr<ptr<struct node_t>>); /* array of nodes pointed to */
  ptr<double> *from_values : itype(array_ptr<ptr<double>>); /* array of ptrs to vals where data comes from */
  double *coeffs : itype(array_ptr<double>); /* array of coeffs on edges */
  int from_count;
  int from_length;
} node_t;

typedef struct graph_t {
  ptr<node_t> e_nodes checked[PROCS];
  ptr<node_t> h_nodes checked[PROCS];
} graph_t;

typedef struct table_t {
  ptr<node_t> *e_table checked[PROCS] : itype(array_ptr<ptr<node_t>> checked[PROCS]);
  ptr<node_t> *h_table checked[PROCS] : itype(array_ptr<ptr<node_t>> checked[PROCS]);
} table_t;

/* Perform 1 step for a nodelist */
void compute_nodes(ptr<node_t> nodelist);
double gen_uniform_double(void);

#endif
