/*
 *
 * hcg.c
 *
 */


#define HCG_CODE


/*
 *
 * Includes.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "types.h"
#include "hcg.h"
#include "channel.h"

#pragma CHECKED_SCOPE ON
#define printf(...) _Unchecked { printf(__VA_ARGS__); }
/*
 *
 * Code.
 *
 */

void
AllocHCG(void)
{
    HCG = malloc<nodeHCGType>((channelNets + 1) * sizeof(nodeHCGType));
    storageRootHCG = malloc<ulong>((channelNets + 1) * (channelNets + 1) * sizeof(ulong));
    _Unchecked { storageHCG = storageRootHCG; }
    storageLimitHCG = (channelNets + 1) * (channelNets + 1);
}

void
FreeHCG(void)
{
    _Unchecked { free<nodeHCGType>(HCG); }
    _Unchecked { free<ulong>(storageRootHCG); }
    storageLimitHCG = 0;
}

void
BuildHCG(void)
{
    ulong	net;
    ulong	which;
    ulong	constraint;
    ulong	first;
    ulong	last;
    ulong	check;
    ulong	add;

    /*
     * Allocate HCG storage.
     */
    AllocHCG();

    /*
     * Build HCG one net at a time.
     */
    for (net = 1; net <= channelNets; net++) {
	first = FIRST[net];
	last = LAST[net];
	constraint = 0;
	HCG[net].nets = constraint;
	_Unchecked { HCG[net].netsHook = storageHCG; }
	for (which = 1; which <= channelNets; which++) {
	    if (((FIRST[which] < first) && (LAST[which] < first)) || ((FIRST[which] > last) && (LAST[which] > last))) {
		/*
		 * No constraint.
		 */
	    }
	    else {
		/*
		 * No constraint should ever already exist.
		 * Because there is only one first and last
		 * for each net, the same constraint could
		 * never be added twice.
		 */
		add = TRUE;
		for (check = 0; check < constraint; check++) {
		    if (HCG[net].netsHook[check] == which) {
			add = FALSE;
			break;
		    }
		}
		assert(add);

		/*
		 * Add constraint.
		 */
		assert(storageLimitHCG > 0);
		HCG[net].nets = constraint;
		HCG[net].netsHook[constraint] = which;
		storageHCG++;
		storageLimitHCG--;
		constraint++;
	    }
	}
	HCG[net].nets = constraint;
    }
}

void
DFSClearHCG(_Array_ptr<nodeHCGType> HCG : count(channelNets + 1))
{
    ulong	net;

    for (net = 1; net <= channelNets; net++) {
	HCG[net].netsReached = FALSE;
    }	
}

void
DumpHCG(_Array_ptr<nodeHCGType> HCG : count(channelNets + 1))
{
    ulong	net;
    ulong	which;

    for (net = 1; net <= channelNets; net++) {
	printf("[%d]\n", net);
	for (which = 0; which < HCG[net].nets; which++) {
	    printf("%d ", HCG[net].netsHook[which]);
	}
	printf("\n\n");
    }	
}

void
NoHCV(_Array_ptr<nodeHCGType> HCG : count(channelNets + 1),
      ulong select,
      _Array_ptr<ulong> netsAssign : count(channelNets + 1),
      _Array_ptr<ulong> tracksNoHCV : count(channelTracks + 2))
{
    ulong	track;
    ulong	net;
    ulong	which;
    ulong	ok;

    for (track = 1; track <= channelTracks; track++) {
	/*
	 * For each track, check to see if any nets assigned
	 * to it would be an HCV for the selected net.
	 */
	ok = TRUE;
	for (net = 1; net <= channelNets; net++) {
	    if (netsAssign[net] == track) {
		/*
		 * Net assigned to track.
		 */
		for (which = 0; which < HCG[select].nets; which++) {
		    if (HCG[select].netsHook[which] == net) {
			/*
			 * HCV.
			 */
			ok = FALSE;
			break;
		    }
		}
		
		/*
		 * Is net an HCV?
		 */
		if (! ok) {
		    break;
		}
	    }
	}

	/*
	 * Is this track ok (no HCV's)?
	 */
	tracksNoHCV[track] = ok;
    }
}
