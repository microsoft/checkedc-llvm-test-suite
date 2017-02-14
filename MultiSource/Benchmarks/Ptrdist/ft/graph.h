/*
 * graph.h
 *
 * The author of this software is Alain K\"{a}gi.
 *
 * Copyright (c) 1993 by Alain K\"{a}gi and the University of Wisconsin
 * Board of Trustees.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHOR NOR THE UNIVERSITY OF
 * WISCONSIN MAKE ANY REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING
 * THE MERCHANTABILITY OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR
 * PURPOSE.
 *
 * ------------------------------------------------------------------------
 *
 * The graph etc.
 *
 * ------------------------------------------------------------------------
 *
 * $Id$
 *
 */

#ifndef _graph_h
#define _graph_h

struct _Vertices;

typedef struct _Edges
{
  int               weight;

  _Ptr<struct _Vertices>  source;
  _Ptr<struct _Vertices>  vertex;
  _Ptr<struct _Edges>  next;
} Edges;

typedef struct _Vertices
{
  int               id;
  _Ptr<Edges>  edges;
  _Ptr<struct _Vertices>  next;

  /*
   * For the ft algorithm.
   */
  int               key;
  _Ptr<Edges>  chosenEdge;
} Vertices;

#define NULL_EDGE	((void *) 0)
#define NULL_VERTEX	((void *) 0)

#define MAX_WEIGHT	100

#define ID(V)		((*(V)).id)
#define EDGES(V)	((*(V)).edges)
#define NEXT_VERTEX(V)	((*(V)).next)

#define WEIGHT(E)	((*(E)).weight)
#define SOURCE(E)	((*(E)).source)
#define VERTEX(E)	((*(E)).vertex)
#define NEXT_EDGE(E)	((*(E)).next)

#define KEY(V)		((*(V)).key)
#define CHOSEN_EDGE(V)	((*(V)).chosenEdge)

_Ptr<Vertices>  GenGraph(int nVertex, int nEdge);
void      PrintGraph(_Ptr<Vertices>  graph);

#endif
