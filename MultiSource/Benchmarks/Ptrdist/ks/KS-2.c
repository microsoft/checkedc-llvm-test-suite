/*
 *	program:	Graph partition via Kernighan-Lin, modified
 *			Kernighan-Lin, or Kernighan-Schweikert
 *
 *	author:		Todd M. Austin
 *			ECE 756
 *
 *	date:		Thursday, February 25, 1993
 */

// CHECKEDC : bounds-safe interface, checked header inclusion
#include <stdio_checked.h>
#include <stdlib_checked.h>
#include <string_checked.h>
#include <assert.h>

#include "KS.h"

/* handle special cases where both nodes are switched */
float
CAiBj(ModuleRecPtr mrA, ModuleRecPtr mrB)
{
    // CHECKEDC : automatic checkedc pointer variable initializer
    NetPtr netNode = 0;
    ModulePtr modNode = 0;
    float gain = 0.0;
    float netCost;
    unsigned long module = (*mrB).module;

    /* is mrA connected to mrB? */
    /* mrA and mrB are both un-Swapped */
    // CHECKEDC : checkedc array type, in-bounds check
    // CHECKEDC : pointer dereference non-null check
    // dynamic_check(mrA != NULL)
    // dynamic_check((*mrA).module >= 0 && (*mrA).module < G_SZ)
    for (netNode = modules[(*mrA).module];
	 netNode != NULL;
	 netNode = (*netNode).next) {
	netCost = cost[(*netNode).net];
    // CHECKEDC : checkedc array type, in-bounds check
    // dynamic_check(netNode != NULL)
    // dynamic_check((*netNode).net >= 0 && (*netNode).net < G_SZ)
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
    // CHECKEDC : _Ptr type dereference (non-null check)
    // dynamic_check(group != NULL)
    // dynamic_check(max != NULL)
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
    // CHECKEDC : _Ptr type dereference (non-null check)
    // dynamic_check(swapTo != NULL)
    if ((*swapTo).tail == NULL) {	/* empty */
#if 0
	(*swapTo).head = (*swapTo).tail = max;
#endif
	(*swapTo).tail = max;
	(*swapTo).head = max;
    }
    else { /* end of list */
        // CHECKEDC : _Ptr type dereference (non-null check)
        // dynamic_check((*swapTo).tail != NULL)
        // reasoning facts ((*swapTo).tail != NULL) since else stmt
        // reasoning facts can remove unnecessary runtime bounds check
	(*(*swapTo).tail).next = max;
	(*swapTo).tail = max;
    }
    (*max).next = NULL;
}

/* incrementally update the D values in Kernighan-Lin algorithm */
void
UpdateDs(ModuleRecPtr max, Groups group)
{
    // CHECKEDC : automatic checkedc pointer variable initializer
    NetPtr net = 0;
    ModulePtr mod = 0;

    /* for all nets this is connected to */
    // CHECKEDC : _Ptr type dereference (non-null check)
    // CHECKEDC : checkedc array type (in-bounds check)
    // dynamic_check(max != NULL)
    // dynamic_check((*max).module >= 0 && (*max).module < G_SZ)
    for (net = modules[(*max).module]; net != NULL; net = (*net).next) {

	/* for a modules this net is connected to */
        // CHECKEDC : _Ptr type dereference (non-null check)
        // CHECKEDC : checkedc array type (in-bounds check)
        // dynamic_check(net != NULL)
        // dynamic_check(mod != NULL)
        // dynamic_check((*net).net >= 0 && (*net).net < G_SZ)
        // dynamic_check((*mod).module >= 0 && (*mod).module < G_SZ)
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
FindMaxGpAndSwap()
{
    // CHECKEDC : automatic checkedc pointer variable initializer
    ModuleRecPtr mrA = 0, mrPrevA = 0, mrB = 0, mrPrevB = 0;
    ModuleRecPtr maxA = 0, maxPrevA = 0, maxB = 0, maxPrevB = 0;
    float gp, gpMax;

    gpMax = -9999999;
    maxA = maxPrevA = maxB = maxPrevB = NULL;
    // CHECKEDC : dereference (non-null check)
    // dynamic_check(mrA != NULL)
    for (mrA = groupA.head, mrPrevA = NULL;
	 mrA != NULL;
	 mrPrevA = mrA, mrA = (*mrA).next) {

        // CHECKEDC : dereference (non-null check)
        // dynamic_check(mrB != NULL)
	for (mrB = groupB.head, mrPrevB = NULL;
	     mrB != NULL;
	     mrPrevB = mrB, mrB = (*mrB).next) {

        // CHECKEDC : checked array type (in-bounds check)
        // dynamic_check((*mrA).module >= 0 && (*mrA).module < G_SZ)
        // dynamic_check((*mrB).module >= 0 && (*mrB).module < G_SZ)
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
    // CHECKEDC : checked array type (in-bounds check)
    // dynamic_check((*maxA).module >= 0 && (*maxA).module < G_SZ)
    // dynamic_check((*maxB).module >= 0 && (*maxB).module < G_SZ)
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
    // CHECKEDC : automatic checkedc pointer variable initializer
    ModuleRecPtr mrPrevA = 0, mrA = 0, mrPrevB = 0, mrB = 0;

    // CHECKEDC : non-null check for _Ptr checkedc pointer type
    // dynamic_check(mrA != NULL && mrB != NULL)
    /* re-splice the lists @ iMax pointers into the lists */
    for (mrPrevA = NULL, mrA = swapToA.head,
	 mrPrevB = NULL, mrB = swapToB.head, i=0;
	 i <= iMax;
	 mrPrevA = mrA, mrA = (*mrA).next,
	 mrPrevB = mrB, mrB = (*mrB).next,
	 i++);

    /* must at least select one to swap, case where gMax is first */
    assert(mrPrevA != NULL && mrPrevB != NULL);
    // CHECKEDC : programmer-inserted dynamic_check
    // introduce new facts which can be used in later analysis
    // dynamic_check(mrPrevA != NULL && mrPrevB != NULL)

    if (mrA == NULL) {	
	/* swap entire list */
	groupA = swapToA;
	groupB = swapToB;
    }
    else {
	/* splice the lists */
        // CHECKEDC : _Ptr dereference (non-null check)
        // dynamic_check(mrPrevA != NULL && mrPrevB != NULL)
        // reasoning facts can optimize. compiler can remove unnecessary bounds check
	(*mrPrevA).next = mrB;
	groupA.head = swapToA.head;
	groupA.tail = swapToB.tail;

	(*mrPrevB).next = mrA;
	groupB.head = swapToB.head;
	groupB.tail = swapToA.tail;
    }

    /* reset the inverse mappings */
    // CHECKEDC : checked array type (in-bounds check)
    // dynamic_check(0 <= (*mrA).module < G_SZ)
    // dynamic_check(0 <= (*mrB).module < G_SZ)
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
void
PrintResults(int verbose)
{
    // CHECKEDC : automatic checkedc pointer variable initializer
    ModuleRecPtr mr = 0;
    NetPtr nn = 0;
    ModulePtr mn = 0;
    unsigned long cuts;
    Groups grp;
    int i, netSz;

    fprintf(stdout, "----------------------------------------------\n");

    maxStat = -1;
    for (i=0; i<256; i++)
        // CHECKEDC : checkedc array type, in-bounds check
        // dynamic_check(i >= 0 && i < 256)
        // compiler can remove unnecessary bounds check
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

        // CHECKEDC
        // dynamic_check(mr != NULL)
        // dynamic_check((*mr).module < G_SZ)
	assert(moduleToGroup[(*mr).module] == GroupA);

	/* for all nets on this module */
    // CHECKEDC : checked array type (in-bounds check)
    // dynamic_check((*mr).module < G_SZ)
	for (nn = modules[(*mr).module]; nn != NULL; nn = (*nn).next) {
	    
	    netSz = 0;
        // CHECKEDC : checked array type (in-bounds check)
        // dynamic_check((*mr).net < G_SZ)
	    for (mn = nets[(*nn).net]; mn != NULL; mn = (*mn).next)
		netSz++;
	    assert(netSz >= 2);

	    /* for all modules on this net */
	    for (mn = nets[(*nn).net]; mn != NULL; mn = (*mn).next) {

		/* only check nodes other than self, and not swapped */
        // CHECKEDC : checked array type (in-bounds check)
        // dynamic_check((*mr).module < G_SZ)
        // dynamic_check((*mn).module < G_SZ)
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
    // CHECKEDC : checked array type (in-bounds check)
    // dynamic_check(0 <= i < G_SZ)
    // dynamic_check(mn != NULL)
	for (mn = nets[i]; mn != NULL; mn = (*mn).next)
	    netSz++;
	assert(netSz >= 2);
    // CHECKEDC : in bounds check
    // dynamic_check(0 <= netSz < 256)
	netStats[netSz].total++;
	if (netSz > maxStat)
	    maxStat = netSz;

    // CHECKEDC : non-null check && in-bounds check
    // dynamic_check(i >= 0 && i < G_SZ)
    // dynamic_check(nets[i] != NULL)
    // dynamic_check(0 <= (*(nets[i])).module < G_SZ)
	for (grp=moduleToGroup[(*(nets[i])).module],mn = (*(nets[i])).next;
	     mn != NULL;
	     mn = (*mn).next) {
	    
	    /* only check nodes other than self, and not swapped */
        // CHECKEDC : _Ptr type (non-null check)
        // CHECKEDC : checked array type (in-bounds check)
        // dynamic_check(mn != NULL)
        // dynamic_check(0 <= (*mn).module < G_SZ)
	    if (grp != moduleToGroup[(*mn).module]) {
		if (verbose)
		    fprintf(stdout, "Net %3lu cut.\n", i+1);
		cuts++;
        // CHECKEDC : checked array type (in-bounds check)
        // dynamic_check(0 <= netSz < G_SZ)
		netStats[netSz].netsCut++;
		break;
	    }
	}
    }
    fprintf(stdout, "Total net cuts  = %lu\n", cuts);

    // CHECKEDC : programmer-inserted dynamic_checks can enhance runtime overhead
    // dynamic_check(maxStat < 256)
    for (i=2; i<=maxStat; i++)
        // CHECKEDC
        // dynamic_check(0 <= i < 256)
        // compile can remove unnecessary bounds check
	fprintf(stdout,
		"sz:%5lu     total:%5lu     edgesCut:%5lu     netsCuts:%5lu\n",
		i, netStats[i].total,
		netStats[i].edgesCut, netStats[i].netsCut);
}

int
main(int argc, char **argv)
{
    unsigned long p, iMax;
    float gMax, lastGMax;
    // CHECKEDC : automatic checkedc pointer variable initializer
    ModuleRecPtr mr = 0;
    ;

    /* parse argument */
    if (argc != 2) {
	fprintf(stderr, "Usage: KL <input_file>\n");
        ;
	exit(1);
    }

    /* prepare the data structures */
    ReadNetList(argv[1]);
    NetsToModules();
    ComputeNetCosts();

    assert((numModules % 2) == 0);

    /* initial partition */
    InitLists();
    lastGMax = 0;

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
        // CHECKEDC : checkedc array type (in-bounds check)
        // dynamic_check(p >= 0 && p < G_SZ)
	    GP[p] = FindMaxGpAndSwap();

	}
	/* lists should both be empty now */
	assert(groupA.head == NULL && groupA.tail == NULL);
	assert(groupB.head == NULL && groupB.tail == NULL);

	gMax = FindGMax(&iMax);

	/* debug/statistics */
	if (lastGMax == gMax)
	    fprintf(stdout, "No progress: gMax = %f\n", gMax);
	lastGMax = gMax;
	fprintf(stdout, "gMax = %f, iMax = %lu\n", gMax, iMax);

	if (gMax > 0.0)
	    SwapSubsetAndReset(iMax);
	PrintResults(0);
    } while (gMax > 0.0);	/* progress made? */

    /* all swaps rejected */
    groupA = swapToB;
    // CHECKEDC : checked array type (in-bounds check)
    // dynamic_check(0 <= (*mr).module < G_SZ)
    for (mr = groupA.head; mr != NULL; mr = (*mr).next)
	moduleToGroup[(*mr).module] = GroupA;
    groupB = swapToA;
    // CHECKEDC : checked array type (in-bounds check)
    // dynamic_check(0 <= (*mr).module < G_SZ)
    for (mr = groupB.head; mr != NULL; mr = (*mr).next)
	moduleToGroup[(*mr).module] = GroupB;

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
