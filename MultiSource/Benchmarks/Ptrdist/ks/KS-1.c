/*
 *	program:	Graph partition via Kernighan-Lin, modified
 *			Kernighan-Lin, or Kernighan-Schweikert
 *
 *	author:		Todd M. Austin
 *			ECE 756
 *
 *	date:		Thursday, February 25, 1993
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "KS.h"

#pragma CHECKED_SCOPE ON

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
ReadNetList(_Nt_array_ptr<char> fname)
{
    _Ptr<FILE> inFile = 0;
    char line _Nt_checked[BUF_LEN + 1] = { 0 };
    unsigned long net, dest;
    ModulePtr node = 0, prev = 0, head = 0;

    TRY(inFile = fopen(fname, "r"),
	inFile != NULL, "ReadData",
	"unable to open input file [%s]", fname, 0, 0,
	exit(1));

    _Unchecked { TRY(fgets(line, BUF_LEN, inFile),
	sscanf((const char*)line, "%lu %lu", &numNets, &numModules) == 2, "ReadData",
	"unable to parse header in file [%s]", fname, 0, 0,
	exit(1)); }

    for (net = 0; net < numNets; net++) {
	fgets(line, BUF_LEN, inFile);
	
	/* net connections for "dest" */
	dest = atol(strtok(line, " \t\n"))-1;

	/* parse out all the net module connections */
	TRY(head = prev = calloc<Module>(1, sizeof(Module)),
	    prev != NULL, "ReadData",
	    "unable to allocate a module list node", 0, 0, 0,
	    exit(1));
	(*prev).module = atol(strtok(NULL, " \t\n"))-1;
	(*prev).next = NULL;
    _Nt_array_ptr<char> tok = NULL;
	while ((tok = strtok(NULL, " \t\n")) != NULL) {
	    TRY(node = calloc<Module>(1, sizeof(Module)),
		node != NULL, "ReadData",
		"unable to allocate a module list node", 0, 0, 0,
		exit(1));
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
    ModulePtr modNode = 0;
    NetPtr netNode = 0;

    for (mod = 0; mod<numModules; mod++)
	modules[mod] = NULL;

    for (net=0; net<numNets; net++) {
	for (modNode = nets[net]; modNode != NULL; modNode = (*modNode).next) {
	    TRY(netNode = calloc<Net>(1, sizeof(Net)),
		netNode != NULL, "NetsToModules",
		"unable to allocate net list node", 0, 0, 0,
		exit(1));
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

    for (i=0; i<numNets; i++) {
#ifndef WEIGHTED
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
    ModuleRecPtr mr = 0;

    groupA.head = groupA.tail = NULL;
    groupB.head = groupB.tail = NULL;

    /* for all modules */
    for (p = 0; p<numModules/2; p++) {

	/* build the group A module list */
	TRY(mr = calloc<ModuleRec>(1, sizeof(ModuleRec)),
	    mr != NULL, "main",
	    "unable to allocate ModuleRec", 0, 0, 0,
	    exit(1));
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
	moduleToGroup[p] = GroupA;

	/* build the group B module list */
	TRY(mr = calloc<ModuleRec>(1, sizeof(ModuleRec));,
	    mr != NULL, "main",
	    "unable to allocate ModuleRec", 0, 0, 0,
	    exit(1));
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

    NetPtr netNode = 0;
    ModulePtr modNode = 0;
    ModuleRecPtr groupNode = 0;
    unsigned long numInG, numInNet;
    ModulePtr oneInG = 0;

    /* for all modules in group */
    for (groupNode = (*group).head;
	 groupNode != NULL;
	 groupNode = (*groupNode).next) {

	assert(moduleToGroup[(*groupNode).module] == myGroup);

	/* for all nets on this module, check if groupNode move unifies net */
	for (netNode = modules[(*groupNode).module];
	     netNode != NULL;
	     netNode = (*netNode).next) {

	    /* look for single node nets, or single partition nets */
	    numInG = numInNet = 0;
	    oneInG = NULL;
	    /* for all modules on this net */
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

    NetPtr netNode = 0;
    ModulePtr modNode = 0;
    ModuleRecPtr groupNode = 0;

    /* for all modules in group */
    for (groupNode = (*group).head;
	 groupNode != NULL;
	 groupNode = (*groupNode).next) {

	assert(moduleToGroup[(*groupNode).module] == myGroup);

	/* initial conditions */
	I = E = 0.0;

	/* for all nets on this module */
	for (netNode = modules[(*groupNode).module];
	     netNode != NULL;
	     netNode = (*netNode).next) {
	    
	    /* for all modules on this net */
	    for (modNode = nets[(*netNode).net];
		 modNode != NULL;
		 modNode = (*modNode).next) {

		/* only check nodes other than self, and not swapped */
		if (((*modNode).module != (*groupNode).module) &&
		    (moduleToGroup[(*modNode).module] < SwappedToA)) {
		    if (moduleToGroup[(*modNode).module] == myGroup)
			I = I + cost[(*netNode).net]; /* internal */
		    else
			E = E + cost[(*netNode).net]; /* external */
		}
	    }
	}
	D[(*groupNode).module] = E - I;
    }

#endif /* !KS_MODE */

}
