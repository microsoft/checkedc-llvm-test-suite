/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "mst.h"
#pragma CHECKED_SCOPE ON

typedef struct blue_return {
  Vertex vert;
  int dist;
} BlueReturn;


typedef struct fc_br {
  BlueReturn value;
} future_cell_BlueReturn;


static BlueReturn BlueRule(Vertex inserted, Vertex vlist) 
{
  BlueReturn retval = {0};
  Vertex tmp = NULL, prev = NULL;
  Hash hash = NULL;
  int dist,dist2;
  int count;
  
  if (!vlist) {
    retval.dist = 999999;
    return retval;
  }

  prev = vlist;
  retval.vert = vlist;
  retval.dist = vlist->mindist;
  hash = vlist->edgehash;
  unchecked { dist = (int) HashLookup((unsigned int) inserted, hash); }
  /*printf("Found %d at 0x%x for 0x%x\n",dist,inserted,vlist);*/
  if (dist) 
    {
      if (dist<retval.dist) 
        {
          vlist->mindist = dist;
          retval.dist = dist;
        }
    }
  else printf("Not found\n");
  
  count = 0;
  /* We are guaranteed that inserted is not first in list */
  for (tmp=vlist->next; tmp; prev=tmp,tmp=tmp->next) 
    {
      count++;
      if (tmp==inserted) 
        {
          Vertex next = tmp->next;
          prev->next = next;
        }
      else 
        {
          hash = tmp->edgehash; /* <------  6% miss in tmp->edgehash */ 
          dist2 = tmp->mindist;
          unchecked { dist = (int) HashLookup((unsigned int) inserted, hash); }
          /*printf("Found %d at 0x%x for 0x%x\n",dist,inserted,tmp);*/
          if (dist) 
            {
              if (dist<dist2) 
                {
                  tmp->mindist = dist;
                  dist2 = dist;
                }
            }
          else printf("Not found\n");
          if (dist2<retval.dist) 
            {
              retval.vert = tmp;
              retval.dist = dist2;
            }
        } /* else */
    } /* for */
  /*printf("Count was %d\n",count);*/
  return retval;
}

          

static Vertex MyVertexList = NULL;

static BlueReturn Do_all_BlueRule(Vertex inserted, int nproc, int pn) {
  future_cell_BlueReturn fcleft = {0};
  BlueReturn retright = {0};

  if (nproc > 1) {
     fcleft.value = Do_all_BlueRule(inserted,nproc/2,pn+nproc/2);
     retright = Do_all_BlueRule(inserted,nproc/2,pn);

     if (fcleft.value.dist < retright.dist) {
       retright.dist = fcleft.value.dist;
       retright.vert = fcleft.value.vert;
       }
     return retright;
  }
  else {
     if (inserted == MyVertexList)
       MyVertexList = MyVertexList->next;
     return BlueRule(inserted,MyVertexList);
  }
}

static int ComputeMst(Graph graph,int numproc,int numvert) 
{
  Vertex inserted = NULL, tmp = NULL;
  int cost=0,dist;

  /* make copy of graph */
  printf("Compute phase 1\n");

  /* Insert first node */
  _Unchecked { inserted = (Vertex)graph->vlist[0].starting_vertex; }
  tmp = inserted->next;
  _Unchecked { graph->vlist[0].starting_vertex = tmp; }
  MyVertexList = tmp;
  numvert--;
  /* Announce insertion and find next one */
  printf("Compute phase 2\n");
  while (numvert) 
    {
      BlueReturn br = {0};
      
      br = Do_all_BlueRule(inserted,numproc,0);
      inserted = br.vert;    
      dist = br.dist;
      numvert--;
      cost = cost+dist;
    }
  return cost;
}

int main(int argc, array_ptr<nt_array_ptr<char>> argv : count(argc))
{
  Graph graph = NULL;
  int dist;
  int size;
 
  size = dealwithargs(argc,argv);
  printf("Making graph of size %d\n",size);

  graph = MakeGraph(size,NumNodes);
  printf("Graph completed\n");

  printf("About to compute mst \n");

  dist = ComputeMst(graph,NumNodes,size);

  printf("MST has cost %d\n",dist);
  exit(0);
}
