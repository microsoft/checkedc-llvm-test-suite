/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "em3d.h"
#include "make_graph.h"
#include <stdchecked.h>
#pragma CHECKED_SCOPE ON

extern int NumNodes;

int DebugFlag;

void print_graph(ptr<graph_t> graph, int id)
{
  ptr<node_t> cur_node = graph->e_nodes[id];

  for(; cur_node; cur_node=cur_node->next)
    {
      chatting("E: value %f, from_count %d\n", *cur_node->value,
	       cur_node->from_count);
    }
  cur_node=graph->h_nodes[id];
  for(; cur_node; cur_node=cur_node->next)
    {
      chatting("H: value %f, from_count %d\n", *cur_node->value,
	       cur_node->from_count);
    }
}

extern int nonlocals;

int main(int argc, array_ptr<nt_array_ptr<char>> argv : count(argc))
{
  int i;
  ptr<graph_t> graph = NULL;

  dealwithargs(argc,argv);

  chatting("Hello world--Doing em3d with args %d %d %d %d\n",
           n_nodes,d_nodes,local_p,NumNodes);
  graph=initialize_graph();
  if (DebugFlag) 
    for(i=0; i<NumNodes;i++)
      print_graph(graph,i);

  compute_nodes(graph->e_nodes[0]);
  compute_nodes(graph->h_nodes[0]);
  chatting("nonlocals = %d\n",nonlocals);

  printstats();
  return 0;
}
