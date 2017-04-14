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

// CHECKEDC : checked array type
NetPtr modules _Checked [G_SZ];		/* all modules -> nets */
unsigned long numModules;

ModulePtr nets _Checked [G_SZ];	     	/* all nets -> modules */
unsigned long numNets;

ModuleList groupA, groupB;			/* current A, B */
ModuleList swapToA, swapToB;			/* swapped from A,B, ordered */
float GP _Checked [G_SZ];			/* GPs, ordered */

Groups moduleToGroup _Checked [G_SZ];	/* current inverse mapping */
float D _Checked [G_SZ];			/* module costs */
float cost _Checked [G_SZ];			/* net costs */


/* read the netlist into the nets[] structure */
void
ReadNetList(char *fname : itype(_Ptr<char>))
{
    // CHECKEDC : automatic checkedc pointer variable initializer
    _Ptr<FILE> inFile = 0;
    char line[BUF_LEN];
    char *tok;
    unsigned long net, dest;
    // CHECKEDC : automatic checkedc pointer variable initializer
    ModulePtr node = 0, prev = 0, head = 0;

    TRY(inFile = fopen(fname, "r"),
	inFile != NULL, "ReadData",
	"unable to open input file [%s]", inFile, 0, 0,
	exit(1));

    TRY(fgets(line, BUF_LEN, inFile),
	sscanf(line, "%lu %lu", &numNets, &numModules) == 2, "ReadData",
	"unable to parse header in file [%s]", inFile, 0, 0,
	exit(1));

    for (net = 0; net < numNets; net++) {
	fgets(line, BUF_LEN, inFile);
	
	/* net connections for "dest" */
	dest = atol(strtok(line, " \t\n"))-1;

	/* parse out all the net module connections */
    // CHECKEDC : allocated pointer non-null check can be removed
    // since compiler insert dynamic non-null check before dereference of _Ptr pointer type
    // static checking is unnecessary with checkedc extension
	TRY(head = prev = calloc(1, sizeof(Module)),
	    prev != NULL, "ReadData",
	    "unable to allocate a module list node", 0, 0, 0,
	    exit(1));
    // CHECKEDC : _Ptr pointer dereference (non-null check)
    // dynamic_check(prev != NULL)
	(*prev).module = atol(strtok(NULL, " \t\n"))-1;
	(*prev).next = NULL;
	while ((tok = strtok(NULL, " \t\n")) != NULL) {
	    TRY(node = calloc(1, sizeof(Module)),
		node != NULL, "ReadData",
		"unable to allocate a module list node", 0, 0, 0,
		exit(1));
        // CHECKEDC : _Ptr pointer dereference (non-null check)
        // dynamic_check(node != NULL)
	    (*node).module = atol(tok)-1;
	    (*node).next = NULL;
	    (*prev).next = node;
	    prev = node;
	}
	nets[dest] = head;
    }
}

/* invert the previously read nets, into a module to net structure */
void
NetsToModules(void)
{
    unsigned long net, mod;
    // CHECKEDC : automatic checkedc pointer variable initializer
    ModulePtr modNode = 0;
    NetPtr netNode = 0;

    // CHECKEDC : programmer-inserted dynamic_check
    // dynamic_check(numModules < G_SZ)
    for (mod = 0; mod<numModules; mod++)
        // CHECKEDC : checkedc array type, arrary subscript
        // dynamic_check(mod >= 0 && mod < G_SZ)
        // if (numModules < G_SZ), then compiler can remove redundant bounds check
        // new facts can be inserted by programmer via dynamic_check built-in method
	modules[mod] = NULL;

    // CHECKEDC : programmer-inserted dynamic check
    // dynamic_check(numNets < G_SZ)
    for (net=0; net<numNets; net++) {
        // CHECKEDC : checkedc array type, array subscript
        // dynamic_check(net >= 0 && net < G_SZ)
        // programmer-inserted dynamic_check can remove unnecessary bounds check
	for (modNode = nets[net]; modNode != NULL; modNode = (*modNode).next) {
	    TRY(netNode = calloc(1, sizeof(Net)),
		netNode != NULL, "NetsToModules",
		"unable to allocate net list node", 0, 0, 0,
		exit(1));
        // CHECKEDC : pointer dereference, memory read
        // dynamic_check(netNode != NULL)
        // : above non-null checks can be replaced with CheckedC pointer type & dynamic_check
	    (*netNode).net = net;
	    (*netNode).next = modules[(*modNode).module];
	    modules[(*modNode).module] = netNode;
	}
    }
}


/* compute the net edge costs, based on the weighting strategy */
void
ComputeNetCosts(void)
{
#ifdef WEIGHTED
    ModulePtr nn;
    unsigned long count;
#endif /* WEIGHTED */
    unsigned long i;

    // CHECKEDC : programmer-inserted dynamic_check can reduce performance overhead
    // dynamic_check(numNets < G_SZ)
    for (i=0; i<numNets; i++) {
#ifndef WEIGHTED
        // CHECKEDC : checkedc array type, compiler inserts runtime bounds check
        // dynamic_check(i >= 0 && i < G_SZ)
	cost[i] = 1.0;
#else
	count = 0;
	for (nn = nets[i]; nn != NULL; nn = (*nn).next)
	    count++;

	cost[i] = 1.0/((float)count - 1.0);
#endif /* WEIGHTED */
    }
}

/* set up the initial groups, just split down the middle */
void
InitLists(void)
{
    unsigned long p;
    // CHECKEDC : automatic checkedc pointer variable initializer
    ModuleRecPtr mr = 0;

    groupA.head = groupA.tail = NULL;
    groupB.head = groupB.tail = NULL;

    /* for all modules */
    // CHECKEDC
    // dynamic_check(numModules < G_SZ)
    // programmer-inserted dynamic_check introduce a new fact
    // Those facts can help the compiler to remove unnecessary runtime bounds check
    for (p = 0; p<numModules/2; p++) {

	/* build the group A module list */
        // CHECKEDC : checkedc pointer type non-null check at dereference
        // dynamic_check(mr != NULL)
	TRY(mr = calloc(1, sizeof(ModuleRec)),
	    mr != NULL, "main",
	    "unable to allocate ModuleRec", 0, 0, 0,
	    exit(1));
    // CHECKEDC : _Ptr dereference (non-null check)
    // dynamic_check(mr != NULL)
	(*mr).module = p;
	if (groupA.head == NULL) {
	    /* first item */
	    groupA.head = groupA.tail = mr;
	    (*mr).next = NULL;
	}
	else {
	    /* add to tail */
	    (*mr).next = NULL;
	    (*groupA.tail).next = mr;
	    groupA.tail = mr;
	}
    // CHECKEDC : checkedc array type
    // dynamic_check(p >= 0 && p < G_SZ)
    // To remove useless dynamic_check, insert dynamic_check outside for-loop
    // programmer-inserted dynamic_check can reduce performance overhead of runtime bounds check
	moduleToGroup[p] = GroupA;

	/* build the group B module list */
    // CHECKEDC : _Ptr dereference (non-null check)
	TRY(mr = calloc(1, sizeof(ModuleRec)),
	    mr != NULL, "main",
	    "unable to allocate ModuleRec", 0, 0, 0,
	    exit(1));
    // CHECKEDC : dereference
    // dynamic_check(mr != NULL)
	(*mr).module = (numModules/2) + p;
	if (groupB.head == NULL) {
	    /* first item */
	    groupB.head = groupB.tail = mr;
	    (*mr).next = NULL;
	}
	else {
	    /* add to tail */
	    (*mr).next = NULL;
	    (*groupB.tail).next = mr;
	    groupB.tail = mr;
	}
    // CHECKEDC : checkedc array type
    // dynamic_check(((numMouldes/2)+p) >= 0 && ((numModules/2)+p) < G_SZ)
	moduleToGroup[(numModules/2) + p] = GroupB;
    }

    /* initially clear the swap chains */
    swapToA.head = swapToA.tail = NULL;
    swapToB.head = swapToB.tail = NULL;
}


/* compute the cost of switching every node in group to the other group */
void
ComputeDs(ModuleListPtr group, Groups myGroup, Groups mySwap)
{
#ifdef KS_MODE

    // CHECKEDC : automatic checkedc pointer variable initializer
    NetPtr netNode = 0;
    ModulePtr modNode = 0;
    ModuleRecPtr groupNode = 0;
    unsigned long numInG, numInNet;
    ModulePtr oneInG = 0;

    /* for all modules in group */
    // CHECKEDC : dereference (non-null check)
    // dynamic_check(group != NULL && groupNode != NULL)
    for (groupNode = (*group).head;
	 groupNode != NULL;
	 groupNode = (*groupNode).next) {

        // CHECKEDC : _Ptr dereference (non-null check)
        // CHECKEDC : checked array type (in-bounds check)
        // dynamic_check(groupNode != NULL)
        // dynamic_check(0 <= (*groupNode).module < G_SZ)
	assert(moduleToGroup[(*groupNode).module] == myGroup);

	/* for all nets on this module, check if groupNode move unifies net */
    // CHECKEDC : dereference (non-null check) && checked array type (in-bounds check)
    // reasoning facts can remove unnecessary bounds check
	for (netNode = modules[(*groupNode).module];
	     netNode != NULL;
	     netNode = (*netNode).next) {

	    /* look for single node nets, or single partition nets */
	    numInG = numInNet = 0;
	    oneInG = NULL;
	    /* for all modules on this net */
        // CHECKEDC : non-null check before pointer dereference
        // dynamic_check(netNode != NULL)
	    for (modNode = nets[(*netNode).net];
		 modNode != NULL;
		 modNode = (*modNode).next) {
		if ((moduleToGroup[(*modNode).module] == myGroup) ||
		    (moduleToGroup[(*modNode).module] == mySwap)) {
		    numInG++;
		    oneInG = modNode;
		}
		numInNet++;
	    }
	    /* single node net? */
	    if ((numInG == 1) && ((*oneInG).module == (*groupNode).module))
		D[(*groupNode).module] = D[(*groupNode).module] + 1;
	    /* single partition net? */
	    if (numInG == numInNet)
		D[(*groupNode).module] = D[(*groupNode).module] - 1;
	}
    }

#else /* !KS_MODE */

    float I, E;

    // CHECKEDC : automatic checkedc pointer variable initializer
    NetPtr netNode = 0;
    ModulePtr modNode = 0;
    ModuleRecPtr groupNode = 0;

    /* for all modules in group */
    // CHECKEDC : _Ptr dereference (non-null check)
    // dynamic_check(group != NULL && groupNode != NULL)
    // dynamic_check(0 <= (*groupNode).module < G_SZ)
    for (groupNode = (*group).head;
	 groupNode != NULL;
	 groupNode = (*groupNode).next) {

	assert(moduleToGroup[(*groupNode).module] == myGroup);

	/* initial conditions */
	I = E = 0.0;

	/* for all nets on this module */
    // CHECKEDC : dereference
    // dynamic_check(groupNode != NULL)
    // dynamic_check(0 <= (*groupNode).module < G_SZ)
    // reaonsing facts can remove unnecessary bounds check
	for (netNode = modules[(*groupNode).module];
	     netNode != NULL;
	     netNode = (*netNode).next) {
	    
	    /* for all modules on this net */
        // CHECKEDC : dereference (non-null check)
        // CHECKEDC : checked array type (in-bounds check)
        // dynamic_check(netNode != NULL)
        // dynamic_check(0 <= (*netNode).net < G_SZ)
	    for (modNode = nets[(*netNode).net];
		 modNode != NULL;
		 modNode = (*modNode).next) {

		/* only check nodes other than self, and not swapped */
            // CHECKEDC : dereference (non-null check), checked array type (in-bounds check)
            // dynamic_check(modNode != NULL && groupNode != NULL)
            // dynamic_check(0 <= (*modNode).module < G_SZ)
		if (((*modNode).module != (*groupNode).module) &&
		    (moduleToGroup[(*modNode).module] < SwappedToA)) {
            // CHECKEDC : checked array type (in-bounds check)
            // dynamic_check(0 <= (*netNode).net < G_SZ)
            // reasoning facts remove redundant bounds check
		    if (moduleToGroup[(*modNode).module] == myGroup)
			I = I + cost[(*netNode).net]; /* internal */
		    else
			E = E + cost[(*netNode).net]; /* external */
		}
	    }
	}
    // CHECKEDC : checkedc array type, in-bounds check & non-null check
    // dynamic_check(groupNode != NULL)
    // dynamic_check((*groupNode).module >= 0 && (*groupNode).module < G_SZ)
	D[(*groupNode).module] = E - I;
    }

#endif /* !KS_MODE */

}
