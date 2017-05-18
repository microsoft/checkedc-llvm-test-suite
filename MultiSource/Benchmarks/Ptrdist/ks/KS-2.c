/*
 *	program:	Graph partition via Kernighan-Lin, modified
 *			Kernighan-Lin, or Kernighan-Schweikert
 *
 *	author:		Todd M. Austin
 *			ECE 756
 *
 *	date:		Thursday, February 25, 1993
 */

#include <stdio_checked.h>
#include <stdlib_checked.h>
#include <string_checked.h>
#include <assert.h>

#include "KS.h"

#pragma BOUNDS_CHECKED ON

/* handle special cases where both nodes are switched */
float
CAiBj(ModuleRecPtr mrA, ModuleRecPtr mrB)
{
    NetPtr netNode = 0;
    ModulePtr modNode = 0;
    float gain = 0.0;
    float netCost;
    unsigned long module = (*mrB).module;

    /* is mrA connected to mrB? */
    /* mrA and mrB are both un-Swapped */
    for (netNode = modules[(*mrA).module];
	 netNode != NULL;
	 netNode = (*netNode).next) {
	netCost = cost[(*netNode).net];
	for (modNode = nets[(*netNode).net];
	     modNode != NULL;
	     modNode = (*modNode).next) {
	    if ((*modNode).module == module) {
		gain = gain + netCost;
	    }
	}
    }
    return gain;
}

/* swap a node out of the current group, and into a target group */
void
SwapNode(ModuleRecPtr maxPrev, ModuleRecPtr max,
	 ModuleListPtr group, ModuleListPtr swapTo)
{
    if (maxPrev == NULL) {	/* found at head of list */
	if ((*group).head == (*group).tail)	{ /* only one in the list */
	    (*group).head = NULL;
	    (*group).tail = NULL;
	    (*max).next = NULL;
	}
	else {
	    (*group).head = (*max).next;
	    (*max).next = NULL;
	}
    }
    else {	/* middle or end of list */
	if ((*group).tail == max)		/* end of list */
	    (*group).tail = maxPrev;
	(*maxPrev).next = (*max).next;
	(*max).next = NULL;
    }

    /* put max on the tail of swapTo */
    if ((*swapTo).tail == NULL) {	/* empty */
#if 0
	(*swapTo).head = (*swapTo).tail = max;
#endif
	(*swapTo).tail = max;
	(*swapTo).head = max;
    }
    else { /* end of list */
	(*(*swapTo).tail).next = max;
	(*swapTo).tail = max;
    }
    (*max).next = NULL;
}

/* incrementally update the D values in Kernighan-Lin algorithm */
void
UpdateDs(ModuleRecPtr max, Groups group)
{
    NetPtr net = 0;
    ModulePtr mod = 0;

    /* for all nets this is connected to */
    for (net = modules[(*max).module]; net != NULL; net = (*net).next) {

	/* for a modules this net is connected to */
	for (mod = nets[(*net).net]; mod != NULL; mod = (*mod).next) {

	    if (moduleToGroup[(*mod).module] < SwappedToA) {
		if (moduleToGroup[(*mod).module] == group)
		    D[(*mod).module] = D[(*mod).module] + cost[(*net).net];
		else
		    D[(*mod).module] = D[(*mod).module] - cost[(*net).net];
	    }
	}
    }
}

/* find the best swap available and do it */
float
FindMaxGpAndSwap(void)
{
    ModuleRecPtr mrA = 0, mrPrevA = 0, mrB = 0, mrPrevB = 0;
    ModuleRecPtr maxA = 0, maxPrevA = 0, maxB = 0, maxPrevB = 0;
    float gp, gpMax;

    gpMax = -9999999;
    maxA = maxPrevA = maxB = maxPrevB = NULL;
    for (mrA = groupA.head, mrPrevA = NULL;
	 mrA != NULL;
	 mrPrevA = mrA, mrA = (*mrA).next) {

	for (mrB = groupB.head, mrPrevB = NULL;
	     mrB != NULL;
	     mrPrevB = mrB, mrB = (*mrB).next) {

#ifdef KS_MODE
	    gp = D[(*mrA).module] + D[(*mrB).module] - CAiBj(mrA, mrB);
#else /* !KS_MODE */
	    gp = D[(*mrA).module] + D[(*mrB).module] - 2*CAiBj(mrA, mrB);
#endif /* !KS_MODE */
	    if (gp > gpMax) {
		gpMax = gp;
		maxA = mrA; maxPrevA = mrPrevA;
		maxB = mrB; maxPrevB = mrPrevB;
	    }
	}
    }

    /* swap the nodes out, into the swap lists */
    assert(maxA != NULL);
    SwapNode(maxPrevA, maxA, &(groupA), &(swapToB));
    assert(maxB != NULL);
    SwapNode(maxPrevB, maxB, &(groupB), &(swapToA));


    /* update the inverse mapping, these two node are now gone */
    assert(moduleToGroup[(*maxA).module] == GroupA);
    moduleToGroup[(*maxA).module] = SwappedToB;

    assert(moduleToGroup[(*maxB).module] == GroupB);
    moduleToGroup[(*maxB).module] = SwappedToA;

#ifndef KS_MODE
    /* update the Ds */
    UpdateDs(maxA, GroupA);
    UpdateDs(maxB, GroupB);
#endif /* !KS_MODE */

    return gpMax;
}

/* find the best point, during the last numModules/2 swaps */
float
FindGMax(_Ptr<unsigned long> iMax)
{
    int i;
    float gMax;

    gMax = -9999999;
    *iMax = 0xffffffff;
    for (i=0; i<numModules/2; i++) {
	if (GP[i] > gMax) {
	    gMax = GP[i];
	    *iMax = i;
	}
    }
    return gMax;
}

/* swap groupA and groupB from [0..iMax] */
void
SwapSubsetAndReset(unsigned long iMax)
{
    unsigned long i;
    ModuleRecPtr mrPrevA = 0, mrA = 0, mrPrevB = 0, mrB = 0;

    /* re-splice the lists @ iMax pointers into the lists */
    for (mrPrevA = NULL, mrA = swapToA.head,
	 mrPrevB = NULL, mrB = swapToB.head, i=0;
	 i <= iMax;
	 mrPrevA = mrA, mrA = (*mrA).next,
	 mrPrevB = mrB, mrB = (*mrB).next,
	 i++);

    /* must at least select one to swap, case where gMax is first */
    assert(mrPrevA != NULL && mrPrevB != NULL);

    if (mrA == NULL) {	
	/* swap entire list */
	groupA = swapToA;
	groupB = swapToB;
    }
    else {
	/* splice the lists */
	(*mrPrevA).next = mrB;
	groupA.head = swapToA.head;
	groupA.tail = swapToB.tail;

	(*mrPrevB).next = mrA;
	groupB.head = swapToB.head;
	groupB.tail = swapToA.tail;
    }

    /* reset the inverse mappings */
    for (mrA = groupA.head; mrA != NULL; mrA = (*mrA).next)
	moduleToGroup[(*mrA).module] = GroupA;
    for (mrB = groupB.head; mrB != NULL; mrB = (*mrB).next)
	moduleToGroup[(*mrB).module] = GroupB;

    /* clear the swap lists */
    swapToA.head = swapToA.tail = NULL;
    swapToB.head = swapToB.tail = NULL;
}


struct {
    unsigned long total;
    unsigned long edgesCut;
    unsigned long netsCut;
} netStats _Checked [256];
long maxStat;

/* print the current groups, and their edge and net cut counts */
_Unchecked void
PrintResults(int verbose)
{
    ModuleRecPtr mr = 0;
    NetPtr nn = 0;
    ModulePtr mn = 0;
    unsigned long cuts;
    Groups grp;
    int i, netSz;

    fprintf(stdout, "----------------------------------------------\n");

    maxStat = -1;
    for (i=0; i<256; i++)
	netStats[i].total = netStats[i].edgesCut = netStats[i].netsCut = 0; 

    /* partitions */
    if (verbose) {
	fprintf(stdout, "Group A:  \n");
	for (mr = groupA.head; mr != NULL; mr = (*mr).next)
	    fprintf(stdout, "%3lu ", (*mr).module+1);
	fprintf(stdout, "\n");

	fprintf(stdout, "Group B:  \n");
	for (mr = groupB.head; mr != NULL; mr = (*mr).next)
	    fprintf(stdout, "%3lu ", (*mr).module+1);
	fprintf(stdout, "\n");
    }

    /* total edge cuts */
    cuts = 0;
    for (mr = groupA.head; mr != NULL; mr = (*mr).next) {

	assert(moduleToGroup[(*mr).module] == GroupA);

	/* for all nets on this module */
	for (nn = modules[(*mr).module]; nn != NULL; nn = (*nn).next) {
	    
	    netSz = 0;
	    for (mn = nets[(*nn).net]; mn != NULL; mn = (*mn).next)
		netSz++;
	    assert(netSz >= 2);

	    /* for all modules on this net */
	    for (mn = nets[(*nn).net]; mn != NULL; mn = (*mn).next) {

		/* only check nodes other than self, and not swapped */
		if (moduleToGroup[(*mr).module] != moduleToGroup[(*mn).module]) {
		    if (verbose)
			fprintf(stdout, "Conn %3lu - %3lu cut.\n",
				(*mr).module+1, (*mn).module+1);
		    netStats[netSz].edgesCut++;
		    cuts++;
		}
	    }
	}
    }
    fprintf(stdout, "Total edge cuts = %lu\n", cuts);

    /* total net cuts */
    cuts = 0;
    for (i=0; i<numNets; i++) {

	netSz = 0;
	for (mn = nets[i]; mn != NULL; mn = (*mn).next)
	    netSz++;
	assert(netSz >= 2);
	netStats[netSz].total++;
	if (netSz > maxStat)
	    maxStat = netSz;

	for (grp=moduleToGroup[(*(nets[i])).module],mn = (*(nets[i])).next;
	     mn != NULL;
	     mn = (*mn).next) {
	    
	    /* only check nodes other than self, and not swapped */
	    if (grp != moduleToGroup[(*mn).module]) {
		if (verbose)
		    fprintf(stdout, "Net %3lu cut.\n", i+1);
		cuts++;
		netStats[netSz].netsCut++;
		break;
	    }
	}
    }
    fprintf(stdout, "Total net cuts  = %lu\n", cuts);

    for (i=2; i<=maxStat; i++)
	fprintf(stdout,
		"sz:%5lu     total:%5lu     edgesCut:%5lu     netsCuts:%5lu\n",
		i, netStats[i].total,
		netStats[i].edgesCut, netStats[i].netsCut);
}

_Unchecked int
main(int argc, _Array_ptr<_Ptr<char>> argv : count(argc))
{
    unsigned long p, iMax;
    float gMax, lastGMax;
    ModuleRecPtr mr = 0;
    ;

    /* parse argument */
    if (argc != 2) {
	fprintf(stderr, "Usage: KL <input_file>\n");
        ;
	exit(1);
    }

    /* prepare the data structures */
    _Checked {ReadNetList(argv[1]);}
    _Checked {NetsToModules();}
    _Checked {ComputeNetCosts();}

    assert((numModules % 2) == 0);

    /* initial partition */
    _Checked {InitLists();}
    lastGMax = 0;

    _Checked {
    /* do until we don't make any progress */
    do {

#ifndef KS_MODE
	/* compute the swap costs */
	ComputeDs(&(groupA), GroupA, SwappedToA);
	ComputeDs(&(groupB), GroupB, SwappedToB);
#endif /* !KS_MODE */

	/* for all pairs of nodes in A,B */
	for (p = 0; p<numModules/2; p++) {

#ifdef KS_MODE
	    /* compute the swap costs */
	    ComputeDs(&(groupA), GroupA, SwappedToA);
	    ComputeDs(&(groupB), GroupB, SwappedToB);
#endif /* KS_MODE */

	    /* find the max swap opportunity, and swap */
	    GP[p] = FindMaxGpAndSwap();

	}
	/* lists should both be empty now */
	assert(groupA.head == NULL && groupA.tail == NULL);
	assert(groupB.head == NULL && groupB.tail == NULL);

	gMax = FindGMax(&iMax);

	/* debug/statistics */
	if (lastGMax == gMax)
	    _Unchecked {fprintf(stdout, "No progress: gMax = %f\n", gMax);}
	lastGMax = gMax;
	_Unchecked {fprintf(stdout, "gMax = %f, iMax = %lu\n", gMax, iMax);}

	if (gMax > 0.0)
	    SwapSubsetAndReset(iMax);
	_Unchecked {PrintResults(0);}
    } while (gMax > 0.0);	/* progress made? */
    }

    /* all swaps rejected */
    _Checked {
    groupA = swapToB;
    for (mr = groupA.head; mr != NULL; mr = (*mr).next)
	moduleToGroup[(*mr).module] = GroupA;
    groupB = swapToA;
    for (mr = groupB.head; mr != NULL; mr = (*mr).next)
	moduleToGroup[(*mr).module] = GroupB;
    }

    ;

    /* all done, show results */
    PrintResults(1);
#ifdef PLUS_STATS
    PrintDerefStats(stderr);
    PrintHeapSize(stderr);
#endif /* PLUS_STATS */
    exit(0);
    return 0;
}
