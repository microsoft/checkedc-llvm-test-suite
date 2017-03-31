/* For copyright information, see olden_v1.0/COPYRIGHT */

#include <stdchecked.h>
#include <stdlib.h>
#include <stdlib_checked.h>
#include "hash.h"
#define MAXPROC 1

#define chatting printf
extern int NumNodes;

struct vert_st {
  int mindist;
  ptr<struct vert_st> next;
  Hash edgehash;
};

typedef ptr<struct vert_st> Vertex;
typedef array_ptr<struct vert_st> VertexArr;

struct graph_st {
  VertexArr vlist checked[MAXPROC];
};

typedef ptr<struct graph_st> Graph;

Graph MakeGraph(int numvert, int numproc);
int dealwithargs(int argc, array_ptr<char*> argv : count(argc));

int atoi(const char *);
