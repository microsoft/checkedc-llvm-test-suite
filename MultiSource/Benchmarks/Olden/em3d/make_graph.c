/* For copyright information, see olden_v1.0/COPYRIGHT */

/* make_graph.c - Create a graph to be solved for the electromagnetic
 *                problem in 3 dimensions.
 *
 * By:  Martin C. Carlisle
 * Date: Feb 23, 1994
 *
 */

#define SEED1 793
#define SEED2 39
#define SEED3 17

#include "em3d.h"
#include "util.h"
#include <stdchecked.h>
#include "hacks.h"
#pragma CHECKED_SCOPE ON

extern int NumNodes;
int NumMisses;

int n_nodes;
int d_nodes;
int local_p;

array_ptr<ptr<node_t>> make_table(int size, int procname) : count(size) {
  array_ptr<ptr<node_t>> retval : count(size) = calloc<ptr<node_t>>(size, sizeof(ptr<node_t>));
  assert(retval);
  return retval;
}

/* We expect node_table to be a local table of e or h nodes */
void fill_table(array_ptr<ptr<node_t>> node_table : count(size), array_ptr<double> values : count(size), int size, int procname)
{
  ptr<node_t> cur_node = NULL;
  ptr<node_t> prev_node = NULL;
  int i;
  
  prev_node = calloc<node_t>(1, sizeof(node_t));
  node_table[0] = prev_node;
  *values = gen_uniform_double();
  _Unchecked { prev_node->value = values++; }
  prev_node->from_values = _Dynamic_bounds_cast<array_ptr<ptr<double>>>(prev_node->from_values, count(0)),
    prev_node->coeffs = _Dynamic_bounds_cast<array_ptr<double>>(prev_node->coeffs, count(0)),
    prev_node->from_count = 0;
  
  /* Now we fill the node_table with allocated nodes */
  for (i=1; i<size; i++) {
    cur_node = calloc<node_t>(1, sizeof(node_t));
    *values = gen_uniform_double();
    _Unchecked { cur_node->value = values++; }
    cur_node->from_values = _Dynamic_bounds_cast<array_ptr<ptr<double>>>(cur_node->from_values, count(0)),
      cur_node->coeffs = _Dynamic_bounds_cast<array_ptr<double>>(cur_node->coeffs, count(0)),
      cur_node->from_count = 0;
    node_table[i] = cur_node;
    prev_node->next = cur_node;
    prev_node = cur_node;
  }
  cur_node->next = NULL;
}

void make_neighbors(ptr<node_t> nodelist, array_ptr<table_arr_t> table : count(PROCS), int tablesz,
		    int degree, int percent_local, int id)
{
  ptr<node_t> cur_node = nodelist;

  for(; cur_node; cur_node=cur_node->next) {
    ptr<node_t> other_node = NULL;
    int j,k;
    int dest_proc;

    array_ptr<ptr<node_t>> tmp : count(degree) = calloc<ptr<node_t>>(degree, (sizeof(ptr<node_t>)));
    dynamic_check(tmp != NULL);

    _Unchecked { cur_node->degree = degree, cur_node->to_nodes = tmp; }

    for (j=0; j<degree; j++) {
      do {
        array_ptr<ptr<node_t>> local_table : count(tablesz) = NULL;
        int number = gen_number(tablesz);

        if (check_percent(percent_local)) {
          dest_proc = id;
        } else {
          dest_proc = (id + PROCS + 4*gen_signed_number(1)) % PROCS;
        }

        /* We expect these accesses to be remote */
        _Unchecked { local_table = table[dest_proc].table; }
        other_node = local_table[number];   /* <------ 4% load miss penalty */
        if (!other_node) {
          chatting("Error! on dest %d @ %d\n",number,dest_proc);
          exit(1);
        }

        array_ptr<ptr<node_t>> ub = tmp + j;
        UncheckedBoundsInit(array_ptr<ptr<node_t>>, tmp2, bounds(tmp2, ub), tmp);
        for ( ; tmp2 < ub; tmp2++)
          if (other_node == *tmp2) break;
        k = tmp2 - tmp;
        /* Original code:
        for (k=0; k<j; k++)
          if (other_node == cur_node->to_nodes[k]) break;
        */
#if 0
        if ((((unsigned long long) other_node) >> 7) < 2048)
          chatting("pre other_node = 0x%x,number = %d,dest = %d\n",
                   other_node,number,dest_proc);
#endif
      }

      while (k<j);

      if (!cur_node || !cur_node->to_nodes) {
        chatting("Error! no to_nodes filed!\n");
        exit(1);
      }

      cur_node->to_nodes[j] = other_node;       /* <------ 6.5% store penalty */
#if 0
      if ((((unsigned long long) other_node) >> 7) < 2048)
        chatting("post other_node = 0x%x\n",other_node);
#endif
      ++other_node->from_count;          /* <----- 12% load miss penalty */
    }
  }
}

void update_from_coeffs(ptr<node_t> nodelist) {
  ptr<node_t> cur_node = nodelist;
  
  /* Setup coefficient and from_nodes vectors for h nodes */  
  for (; cur_node; cur_node=cur_node->next) {
    int from_count = cur_node->from_count;
    
    if (from_count < 1) {
      chatting("Help! no from count (from_count=%d) \n", from_count);
      cur_node->from_values = calloc<ptr<double>>(20, sizeof(ptr<double>));
      cur_node->coeffs = calloc<double>(20, sizeof(double));
      cur_node->from_length = 0;
    } else {
      cur_node->from_values = calloc<ptr<double>>(from_count, sizeof(ptr<double>));
      cur_node->coeffs = calloc<double>(from_count, sizeof(double));
      cur_node->from_length = 0;
    }
  }
}

void fill_from_fields(ptr<node_t> nodelist, int degree) {
  ptr<node_t> cur_node = nodelist;
  for(; cur_node; cur_node = cur_node->next) {
    int j;

    for (j=0; j<degree; j++) {
      int count,thecount;
      ptr<node_t> other_node = cur_node->to_nodes[j]; /* <-- 6% load miss penalty */
      array_ptr<ptr<double>> otherlist : count(other_node->from_count) = NULL;
      ptr<double> value = cur_node->value;

      if (!other_node) { chatting("Help!!\n"); }
      count=(other_node->from_length)++;  /* <----- 30% load miss penalty */
      _Unchecked { otherlist=other_node->from_values;  /* <----- 10% load miss penalty */ }
      thecount=other_node->from_count;
      if (!otherlist) {
        /*chatting("node 0x%p list 0x%p count %d\n",
                 other_node,otherlist,thecount);*/
        _Unchecked { otherlist = other_node->from_values; }
        /*chatting("No from list!! 0x%p\n",otherlist);*/
      }
      
      otherlist[count] = value;                 /* <------ 42% store penalty */

      /* <----- 42+6.5% store penalty */
      other_node->coeffs[count]=gen_uniform_double();
    }
  }
}

void localize_local(ptr<node_t> nodelist) {
  ptr<node_t> cur_node = nodelist;

  for(; cur_node; cur_node = cur_node->next) {
    cur_node->coeffs = cur_node->coeffs;
    cur_node->from_values =  cur_node->from_values;
    cur_node->value =  cur_node->value;
  }
}


void make_tables(ptr<table_t> table,int groupname) {
  array_ptr<ptr<node_t>> h_table : count(n_nodes/PROCS) = NULL;
  array_ptr<ptr<node_t>> e_table : count(n_nodes/PROCS) = NULL;
  array_ptr<double> h_values : count(n_nodes/PROCS) = NULL;
  array_ptr<double> e_values : count(n_nodes/PROCS) = NULL;
  int procname = 0;

  init_random(SEED1*groupname);
  h_values = calloc<double>(n_nodes/PROCS, sizeof(double));
  h_table = make_table(n_nodes/PROCS,procname);
  fill_table(h_table,h_values,n_nodes/PROCS,procname);
  e_values = calloc<double>(n_nodes/PROCS, sizeof(double));
  e_table = make_table(n_nodes/PROCS,procname);
  fill_table(e_table,e_values,n_nodes/PROCS,procname);

  /* This is done on procname-- we expect table to be remote */
  /* We use remote writes */
  _Unchecked {
    table->e_table[groupname].size = n_nodes/PROCS,
      table->e_table[groupname].table = e_table;
  }
  _Unchecked {
    table->h_table[groupname].size = n_nodes/PROCS,
      table->h_table[groupname].table = h_table;
  }
}

void make_all_neighbors(ptr<table_t> table,int groupname) {
  ptr<node_t> first_node = NULL;
  int local_table_size = 1;
  int local_table_bounds = local_table_size;
  array_ptr<ptr<node_t>> local_table : count(local_table_bounds) = NULL;
  array_ptr<table_arr_t> local_table_array : count(1) = NULL;

  init_random(SEED2*groupname);
  /* We expect table to be remote */
  local_table_size = table->h_table[groupname].size;
  _Unchecked { local_table = table->h_table[groupname].table; }
  _Unchecked { local_table_array = table->e_table; }
  first_node = local_table[0];
  make_neighbors(first_node,
		 local_table_array,n_nodes/PROCS,
		 d_nodes,local_p,groupname);

  local_table_size = table->e_table[groupname].size;
  _Unchecked { local_table = table->e_table[groupname].table; }
  _Unchecked { local_table_array = table->h_table; }
  first_node = local_table[0];
  make_neighbors(first_node,
		 local_table_array,n_nodes/PROCS,
		 d_nodes,local_p,groupname);
}

void update_all_from_coeffs(ptr<table_t> table, int groupname)    
{
  int local_table_size = 1;
  int local_table_bounds = local_table_size;
  array_ptr<ptr<node_t>> local_table : count(local_table_bounds) = NULL;
  ptr<node_t> first_node = NULL;

  /* Done by do_all, table not local */
  local_table_size = table->h_table[groupname].size;
  _Unchecked { local_table = table->h_table[groupname].table; }
  /* We expect this to be local */
  first_node = local_table[0];
  update_from_coeffs(first_node);

  local_table_size = table->e_table[groupname].size;
  _Unchecked { local_table = table->e_table[groupname].table; }
  first_node = local_table[0];
  update_from_coeffs(first_node);
}

void fill_all_from_fields(ptr<table_t> table, int groupname)
{
  int local_table_size = 1;
  int local_table_bounds = local_table_size;
  array_ptr<ptr<node_t>> local_table : count(local_table_bounds) = NULL;
  ptr<node_t> first_node = NULL;

  init_random(SEED3*groupname);
  local_table_size = table->h_table[groupname].size;
  _Unchecked { local_table = table->h_table[groupname].table; }
  first_node = local_table[0];
  fill_from_fields(first_node,d_nodes);

  local_table_size = table->e_table[groupname].size;
  _Unchecked { local_table = table->e_table[groupname].table; }
  first_node = local_table[0];
  fill_from_fields(first_node,d_nodes);
}

void localize(ptr<table_t> table, int groupname)
{
  int local_table_size = 1;
  int local_table_bounds = local_table_size;
  array_ptr<ptr<node_t>> local_table : count(local_table_bounds) = NULL;
  ptr<node_t> first_node = NULL;

  local_table_size = table->h_table[groupname].size;
  _Unchecked { local_table = table->h_table[groupname].table; }
  first_node = local_table[0];
  localize_local(first_node);

  local_table_size = table->e_table[groupname].size;
  _Unchecked { local_table = table->e_table[groupname].table; }
  first_node = local_table[0];
  localize_local(first_node);
}
  
void clear_nummiss(ptr<table_t> table, int groupname)
{
  NumMisses = 0;
}

void do_all(ptr<table_t> table, int groupname, int nproc,
	    ptr<void(ptr<table_t>, int)> func, int groupsize) {
  /*chatting("do all group %d with %d\n",groupname,nproc);*/
  if (nproc > 1) {
    do_all(table,groupname+nproc/2,nproc/2,func,groupsize);
    do_all(table,groupname,nproc/2,func,groupsize);
  } else {
    func(table,groupname);
  }
}

ptr<graph_t> initialize_graph(void) {
  ptr<table_t> table = NULL;
  ptr<graph_t> retval = NULL;
  int i,j,blocksize;
  int groupsize;

  table = calloc<table_t>(1, sizeof(table_t));
  retval = calloc<graph_t>(1, sizeof(graph_t));

  groupsize = PROCS/NumNodes;

  chatting("making tables \n");
  do_all(table,0,PROCS,make_tables,groupsize);

  /* At this point, for each h node, we give it the appropriate number
     of neighbors as defined by the degree */
  chatting("making neighbors\n");

  do_all(table,0,PROCS,make_all_neighbors,groupsize);

  /* We now create from count and initialize coefficients */
  chatting("updating from and coeffs\n");
  do_all(table,0,PROCS,update_all_from_coeffs,groupsize);

  /* Fill the from fields in the nodes */
  chatting("filling from fields\n");
  do_all(table,0,PROCS,fill_all_from_fields,groupsize);

  chatting("localizing coeffs, from_nodes\n");
  do_all(table,0,PROCS,localize,groupsize);

  blocksize = PROCS/NumNodes;

  chatting("cleanup for return now\n");
  for (i=0; i<NumNodes; i++) {
    int local_table_size = table->e_table[i*blocksize].size;
    int local_table_bounds = local_table_size;
    array_ptr<ptr<node_t>> local_table : count(local_table_bounds) = NULL;
    _Unchecked { local_table = table->e_table[i*blocksize].table; }
    ptr<node_t> local_node_r = local_table[0];

    retval->e_nodes[i] = local_node_r;
      
    local_table_size = table->h_table[i*blocksize].size;
    _Unchecked { local_table = table->h_table[i*blocksize].table; }
    local_node_r = local_table[0];
    retval->h_nodes[i] = local_node_r;
    for (j = 1; j < blocksize; j++) {
      _Ptr<node_t>  local_node_l = NULL;

      local_table_size = table->e_table[i*blocksize+j-1].size;
      _Unchecked { local_table = table->e_table[i*blocksize+j-1].table; }
      local_node_l = (_Ptr<node_t>)local_table[(n_nodes/PROCS)-1];
      local_table_size = table->e_table[i*blocksize+j].size;
      _Unchecked { local_table = table->e_table[i*blocksize+j].table; }
      local_node_r = local_table[0];
      local_node_l->next = local_node_r;
      
      local_table_size = table->h_table[i*blocksize+j-1].size;
      _Unchecked { local_table = table->h_table[i*blocksize+j-1].table; }
      local_node_l = (_Ptr<node_t>)local_table[(n_nodes/PROCS)-1];
      local_table_size = table->h_table[i*blocksize+j].size;
      _Unchecked { local_table = table->h_table[i*blocksize+j].table; }
      local_node_r = local_table[0];
      local_node_l->next = local_node_r;
    }
  }
  
  chatting("Clearing NumMisses\n");
  do_all(table,0,PROCS,clear_nummiss,groupsize);
  chatting("Returning\n");

  return retval;
}
