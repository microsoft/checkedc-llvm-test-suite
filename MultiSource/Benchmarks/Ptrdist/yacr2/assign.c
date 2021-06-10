/*
 *
 * assign.c
 *
 */


#define ASSIGN_CODE


/*
 *
 * Includes.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "types.h"
#include "assign.h"
#include "channel.h"
#include "vcg.h"
#include "hcg.h"

#pragma CHECKED_SCOPE ON
#define printf(...) _Unchecked { printf(__VA_ARGS__); }

/*
 *
 * Code.
 *
 */

void
AllocAssign(void)
{
    ulong	net;

    /*
     * Allocate cost matrix.
     */
    costMatrix = malloc<struct costMatrixRow>((channelNets+1) * sizeof(struct costMatrixRow));
    for (net = 1; net <= channelNets; net++) {
	costMatrix[net].len = channelTracks + 2,
	  costMatrix[net].row = malloc<long>((channelTracks+2) * sizeof(long));
    }

    /*
     * Allocate structures associated with cost matrix.
     */
    tracksNotPref = malloc<ulong>((channelTracks+2) * sizeof(ulong));
    tracksTopNotPref = malloc<ulong>((channelTracks+2) * sizeof(ulong));
    tracksBotNotPref = malloc<ulong>((channelTracks+2) * sizeof(ulong));
    tracksNoHCV = malloc<ulong>((channelTracks+2) * sizeof(ulong));
    tracksAssign = malloc<ulong>((channelTracks+2) * sizeof(ulong));
    netsAssign = malloc<ulong>((channelNets+1) * sizeof(ulong));
    netsAssignCopy = malloc<ulong>((channelNets+1) * sizeof(ulong));
}

void
FreeAssign(void)
{
    ulong	net;

    /*
     * Free cost matrix.
     */
    for (net = 1; net <= channelNets; net++) {
	_Unchecked { free<long>(costMatrix[net].row); }
    }
    _Unchecked { free<struct costMatrixRow>(costMatrix); }

    /*
     * Free structures associated with cost matrix.
     */
    _Unchecked { free<ulong>(tracksNotPref); }
    _Unchecked { free<ulong>(tracksTopNotPref); }
    _Unchecked { free<ulong>(tracksBotNotPref); }
    _Unchecked { free<ulong>(tracksNoHCV); }
    _Unchecked { free<ulong>(tracksAssign); }
    _Unchecked { free<ulong>(netsAssign); }
    _Unchecked { free<ulong>(netsAssignCopy); }
}

void
NetsAssign(void)
{
    ulong	net;

    /*
     * Init.
     */
    for (net = 1; net <= channelNets; net++) {
	netsAssign[net] = 0;
    }
    
    /*
     * Assign nets in max density column.
     */
    MaxNetsAssign();

#ifdef SCOTT
    /*
     * Assign net in columns right of max density column.
     */
    RightNetsAssign();

    /*
     * Assign net in columns left of max density column.
     */
    LeftNetsAssign();
#else
    /*
     * Assign net in columns left of max density column.
     */
    LeftNetsAssign();

    /*
     * Assign net in columns right of max density column.
     */
    RightNetsAssign();
#endif
}

void
MaxNetsAssign(void)
{
    ulong	net;
    ulong	netSelect;
    ulong	netsCrossing;
    ;

#ifdef VERBOSE
    printf("density = %d\n", channelDensity);
	printf("pivot = %d\n\n", channelDensityColumn);
#endif

    /*
     * Determine nets crossing column of max channel density.
     */
    netsCrossing = 0;
    for (net = 1; net <= channelNets; net++) {
	if ((FIRST[net] <= channelDensityColumn) && (LAST[net] >= channelDensityColumn)) {
	    CROSSING[net] = TRUE;
	    netsCrossing++;
	}
	else {
	    CROSSING[net] = FALSE;
	}
    }

    while (netsCrossing > 0) {
	/*
	 * Choose net from CROSSING to assign.
	 */
	Select(VCG, HCG, netsAssign, &netSelect, CROSSING);

	/*
	 * Assign net to a track.
	 */
	Assign(VCG, netsAssign, netSelect);

	/*
	 * Done net.
	 */
	CROSSING[netSelect] = FALSE;
	netsCrossing--;
    }
    ;
}

void
RightNetsAssign(void)
{
    ulong	net;
    ulong	col;
    ulong	top;	
    ulong	bot;
    ulong	netsCrossing;
    ulong	netSelect;
    ;

    /*
     * Init.
     */
    for (net = 1; net <= channelNets; net++) {
	CROSSING[net] = FALSE;
    }
    netsCrossing = 0;

    /*
     * Assign nets within channel.
     */
    for (col = channelDensityColumn+1; col <= channelColumns; col++) {
	/*
	 * Collection.
	 */
	top = TOP[col];
	bot = BOT[col];
	if (top != bot) {
	    if (top && (FIRST[top] == col)) {
		CROSSING[top] = TRUE;
		netsCrossing++;
	    }
	    if (bot && (FIRST[bot] == col)) {
		CROSSING[bot] = TRUE;
		netsCrossing++;
	    }
	}
	else {
	    if (top && (FIRST[top] == col)) {
		CROSSING[top] = TRUE;
		netsCrossing++;
	    }
	}

	/*
	 * Assignment.
	 */
	if ((LAST[top] == col) || (LAST[bot] == col)) {
	    while (netsCrossing > 0) {
		/*
		 * Choose net from CROSSING to assign.
		 */
		Select(VCG, HCG, netsAssign, &netSelect, CROSSING);
		
		/*
		 * Assign net to a track.
		 */
		Assign(VCG, netsAssign, netSelect);
		
		/*
		 * Done net.
		 */
		CROSSING[netSelect] = FALSE;
		netsCrossing--;
	    }
	}
    }
    assert(! netsCrossing);
    ;
}

void
LeftNetsAssign(void)
{
    ulong	net;
    ulong	col;
    ulong	top;	
    ulong	bot;
    ulong	netsCrossing;
    ulong	netSelect;
    ;

    /*
     * Init.
     */
    for (net = 1; net <= channelNets; net++) {
	CROSSING[net] = FALSE;
    }
    netsCrossing = 0;

    /*
     * Assign nets within channel.
     */
    for (col = channelDensityColumn-1; col >= 1; col--) {
	/*
	 * Collection.
	 */
	top = TOP[col];
	bot = BOT[col];
	if (top != bot) {
	    if (top && (LAST[top] == col)) {
		CROSSING[top] = TRUE;
		netsCrossing++;
	    }
	    if (bot && (LAST[bot] == col)) {
		CROSSING[bot] = TRUE;
		netsCrossing++;
	    }
	}
	else {
	    if (top && (LAST[top] == col)) {
		CROSSING[top] = TRUE;
		netsCrossing++;
	    }
	}

	/*
	 * Assignment.
	 */
	if ((FIRST[top] == col) || (FIRST[bot] == col)) {
	    while (netsCrossing > 0) {
		/*
		 * Choose net from CROSSING to assign.
		 */
		Select(VCG, HCG, netsAssign, &netSelect, CROSSING);
		
		/*
		 * Assign net to a track.
		 */
		Assign(VCG, netsAssign, netSelect);
		
		/*
		 * Done net.
		 */
		CROSSING[netSelect] = FALSE;
		netsCrossing--;
	    }
	}
    }
    assert(! netsCrossing);
    ;
}

void
Assign(_Array_ptr<nodeVCGType> VCG : count(channelNets + 1),
       _Array_ptr<ulong> assign : count(channelNets + 1),
       ulong select)
{
    long	dist;
    ulong	ideal;
    ulong	track;
    ulong	tracks;
    ulong	trackAssign;
    ulong	vcv;
    long	vcvDist;
    ulong	vcvAssign;
    _Array_ptr<long>	costNet : count(channelTracks + 2) = NULL;
    ;

#ifdef VERBOSE
    printf("\n*** Assign %d ***\n", select);
#endif
    
    /*
     * Need information for the selected net.
     * (Must recompute...not current for the selected net)
     *
     * tracksNoHCV
     * tracksNotPref
     * tracksTopNotPref
     * tracksBotNotPref
     * cardNotPref
     * cardTopNotPref
     * cardBotNotPref
     */
    LongestPathVCG(VCG, select);
    NoHCV(HCG, select, assign, tracksNoHCV);
    IdealTrack(channelTracks, cardTopNotPref, cardBotNotPref, &ideal);

#ifdef VERBOSE
	printf("HCV's:\n");
    for (track = 1; track <= channelTracks; track++) {
	if (tracksNoHCV[track]) {
	    printf("[%d] no hcv\n", track);
	}
	else {
	   	printf("[%d] hcv\n", track);
	}
    }
	printf("\n");
#endif

    /*
     * What tracks to consider for assign.
     */
    _Unchecked { costNet = costMatrix[select].row; }
    assert((select >= 1) && (select <= channelNets));
    tracks = 0;
    for (track = 1; track <= channelTracks; track++) {
	tracksAssign[track] = FALSE;
    }

    /*
     * Choice 1.
     */
    for (track = 1; track <= channelTracks; track++) {
	if (tracksNoHCV[track] && (! tracksNotPref[track])) {
	    tracksAssign[track] = TRUE;
	    tracks++;
	}
    }
#ifdef VERBOSE
    if (tracks != 0) {
	printf("using choice 1...\n");
    }
#endif
    /*
     * Choice 2.
     */
    if (tracks == 0) {
	for (track = 1; track <= channelTracks; track++) {
	    if (tracksNoHCV[track] && tracksTopNotPref[track] && tracksBotNotPref[track]) {
		tracksAssign[track] = TRUE;
		tracks++;
	    }
	}
#ifdef VERBOSE
	if (tracks != 0) {
	    printf("using choice 2...\n");
	}
#endif
    }
    /*
     * Choice 3.
     */
    if (tracks == 0) {
	for (track = 2; track < channelTracks; track++) {
	    if (tracksNoHCV[track] && tracksNotPref[track]) {
		tracksAssign[track] = TRUE;
		tracks++;
	    }
	}
#ifdef VERBOSE
	if (tracks != 0) {
	    printf("using choice 3...\n");
	}
#endif
    }
    /*
     * Choice 4.
     */
    if (tracks == 0) {
	if (tracksNoHCV[1]) {
	    tracksAssign[1] = TRUE;
	    tracks++;
	}
	if (tracksNoHCV[channelTracks]) {
	    tracksAssign[channelTracks] = TRUE;
	    tracks++;
	}
#ifdef VERBOSE
	printf("using choice 4...\n");
#endif
    }
    assert(tracks != 0);

/*
    costNet = costMatrix[select];
    assert((select >= 1) && (select <= channelNets));
    tracks = 0;
    for (track = 1; track <= channelTracks; track++) {
	tracksAssign[track] = FALSE;
    }
    for (track = 1; track <= channelTracks; track++) {
	if ((costNet[track] < HIGH) {
	    tracksAssign[track] = TRUE;
	    tracks++;
	}
    }
    if (tracks == 0) {
	for (track = 2; track < channelTracks; track++) {
	    if (costNet[track] < INFINITY) {
		tracksAssign[track] = TRUE;
		tracks++;
	    }
	}
    }
    if (tracks == 0) {
	if (costNet[1] < INFINITY) {
	    tracksAssign[1] = TRUE;
	    tracks++;
	}
	if (costNet[channelTracks] < INFINITY) {
	    tracksAssign[channelTracks] = TRUE;
	    tracks++;
	}
    }
    assert(tracks != 0);
*/

    /*
     * Assign to one of the tracks under consideration.
     */
    trackAssign = 0;
    vcvAssign = INFINITY;
    for (track = 1; track <= channelTracks; track++) {
	if (tracksAssign[track]) {
	    vcv = VCV(VCG, select, track, netsAssign);
	    if (vcv < vcvAssign) {
		vcvAssign = vcv;
		trackAssign = track;
		vcvDist = (long) (track - ideal);
		if (vcvDist < 0) {
		    vcvDist *= -1;
		}
	    }
	    else if (vcv == vcvAssign) {
		assert(trackAssign);
		dist = (long) (track - ideal);
		if (dist < 0) {
		    dist *= -1;
		}
		if (dist < vcvDist) {
		    vcvDist = dist;
		    vcvAssign = vcv;
		    trackAssign = track;
		}
	    }
	}
    }
    assert(trackAssign);
    assign[select] = trackAssign;
#ifdef VERBOSE
    printf("\n*** Assign %d -> %d ***\n", select, trackAssign);
#endif
    ;
}

void
Select(_Array_ptr<nodeVCGType> VCG : count(channelNets + 1),
	_Array_ptr<nodeHCGType> HCG : count(channelNets + 1),
	_Array_ptr<ulong> netsAssign : count(channelNets + 1),
	_Ptr<ulong> netSelect,
	_Array_ptr<ulong> CROSSING : count(channelNets + 1))
{
    ulong	net;
    ulong	track;
    ulong	select;
    _Array_ptr<long>	costNet : count(channelTracks + 2) = NULL;
    long	cost;
    long	largest;

    /*
     * Build cost matrix.
     */
    BuildCostMatrix(VCG, HCG, netsAssign, CROSSING);

    /*
     * Select most restricted net.
     * That is, net with greatest column sum in cost matrix.
     */
    largest = -1;
    select = 0;
    for (net = 1; net <= channelNets; net++) {
	if (CROSSING[net]) {
	    cost = 0;
	    _Unchecked { costNet = costMatrix[net].row; }
	    for (track = 1; track <= channelTracks; track++) {
		cost += costNet[track];
	    }
	    if (cost > largest) {
		largest = cost;
		select = net;
	    }
	}
    }
    assert(select);

    /*
     * Net selected.
     */
    *netSelect = select;
}

void
BuildCostMatrix(_Array_ptr<nodeVCGType> VCG : count(channelNets + 1),
		_Array_ptr<nodeHCGType> HCG : count(channelNets + 1),
		_Array_ptr<ulong> netsAssign : count(channelNets + 1),
		_Array_ptr<ulong> CROSSING : count(channelNets + 1))
{
    ulong	net;
    ulong	track;
    ulong	ideal;
    long       	dist;
    long	mult;
    _Array_ptr<long>	costNet : count(channelTracks + 2) = NULL;
    ;

    /*
     * Initialize cost matrix.
     */
    for (net = 1; net <= channelNets; net++) {
	_Unchecked { costNet = costMatrix[net].row; }
	for (track = 1; track <= channelTracks; track++) {
	    costNet[track] = 0;
	}
    }

    for (net = 1; net <= channelNets; net++) {
	if (CROSSING[net]) {
	    /*
	     * Compute one column in cost matrix.
	     * That is, the cost associated with each track for some net.
	     */
	    _Unchecked { costNet = costMatrix[net].row; }

	    /*
	     * Compute measures related to cost.
	     */
	    LongestPathVCG(VCG, net);
	    NoHCV(HCG, net, netsAssign, tracksNoHCV);
	    IdealTrack(channelTracks, cardTopNotPref, cardBotNotPref, &ideal);

	    /*
	     * Compute cost associated with each track.
	     */
	    for (track = 1; track <= channelTracks; track++) {
		if (tracksNoHCV[track]) {
		    if (cardNotPref != channelTracks) {
			if (tracksNotPref[track]) {
			    costNet[track] = HIGH;
			}
			else {
			    costNet[track] = MEDIUM * cardNotPref;
			}
		    }
		    else {
			if ((track > (channelTracks - cardBotNotPref)) && (track <= cardTopNotPref)) {
			    mult = cardNotPref - (cardTopNotPref + cardBotNotPref - channelTracks);
			    assert(mult >= 0);
			    costNet[track] = MEDIUM * mult;
			}
			else {
			    costNet[track] = HIGH;
			}
		    }

		    if (costNet[track] < INFINITY) {
			dist = (long) (ideal - track);
			if (dist < 0) {
			    dist *= -1;
			}
			costNet[track] += LOW * dist;
		    }
		}
		else {
		    costNet[track] = INFINITY;
		}
	    }
	}
    }
    ;
}

void
IdealTrack(ulong tracks,
	   ulong top,
	   ulong bot,
	   _Ptr<ulong> ideal)
{
    ulong	num;
    ulong	den;

    num = (top * (tracks - bot)) + (bot * (top + 1));
    den = top + bot;

    if (den != 0) {
	*ideal = (ulong) ((ulong) num / (ulong) den);
    }
    else {
	*ideal = 1;
    }
}

