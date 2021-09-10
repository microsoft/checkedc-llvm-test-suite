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

void dealwithargs(int argc, array_ptr<nt_array_ptr<char>> argv : count(argc));
void printstats(void);
void srand48(long);
long lrand48(void);

#include <stdio_checked.h>
#include <stdlib_checked.h>

#pragma CHECKED_SCOPE ON
#define chatting(...) unchecked { printf(__VA_ARGS__); }

// extern char * min_ptr;
// extern char * max_ptr;

extern int n_nodes; /* number of nodes (E and H) */
extern int d_nodes; /* degree of nodes */
extern int local_p; /* percentage of local edges */
#define PROCS 1

#define assert(a) if (!a) { puts("Assertion failure"); exit(-1); }

typedef struct node_t {
  ptr<double> value;
  ptr<struct node_t> next;
  array_ptr<ptr<struct node_t>> to_nodes : count(degree); /* array of nodes pointed to */
  array_ptr<ptr<double>> from_values : count(from_count); /* array of ptrs to vals where data comes from */
  array_ptr<double> coeffs : count(from_count); /* array of coeffs on edges */
  unsigned int from_count;
  int from_length;
  int degree;
} node_t;

typedef struct graph_t {
  ptr<node_t> e_nodes checked[PROCS];
  ptr<node_t> h_nodes checked[PROCS];
} graph_t;

typedef struct table_arr_t {
  array_ptr<ptr<node_t>> table : count(size);
  int size;
} table_arr_t;

typedef struct table_t {
  table_arr_t e_table checked[PROCS];
  table_arr_t h_table checked[PROCS];
} table_t;

/* Perform 1 step for a nodelist */
void compute_nodes(ptr<node_t> nodelist);
double gen_uniform_double(void);

#pragma CHECKED_SCOPE OFF
#endif
