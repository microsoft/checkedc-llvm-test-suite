/* For copyright information, see olden_v1.0/COPYRIGHT */

#include <stdchecked.h>
#include <stdlib_checked.h>
#include "hash.h"
#pragma CHECKED_SCOPE ON
#define MAXPROC 1

#define printf(...) unchecked { printf(__VA_ARGS__); }
extern int NumNodes;

struct vert_st {
  int mindist;
  ptr<struct vert_st> next;
  Hash edgehash;
};

typedef ptr<struct vert_st> Vertex;

struct vert_arr_st {
  array_ptr<struct vert_st> block : count(len);
  int len;
  array_ptr<struct vert_st> starting_vertex : bounds(block, block + len);
};

typedef struct vert_arr_st VertexArray;

struct graph_st {
  struct vert_arr_st vlist checked[MAXPROC];
};

typedef ptr<struct graph_st> Graph;

Graph MakeGraph(int numvert, int numproc);
int dealwithargs(int argc, array_ptr<nt_array_ptr<char>> argv : count(argc));

int atoi(const char * : itype(nt_array_ptr<const char>));
void chatting(nt_array_ptr<char> str);

#pragma CHECKED_SCOPE OFF
