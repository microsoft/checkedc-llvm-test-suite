/*
 *
 * hcg.h
 *
 */


/*
 *
 * Includes.
 *
 */

#include "types.h"


#ifndef HCG_H
#define HCG_H

#pragma CHECKED_SCOPE ON
/*
 *
 * Defines.
 *
 */


/*
 *
 * Types.
 *
 */

typedef struct _nodeHCGType {
    _Array_ptr<ulong>	netsHook : count(nets + 1);
    ulong	nets;
    ulong	netsReached;
} nodeHCGType;


/*
 *
 * Globals.
 *
 */

extern ulong channelNets;
extern ulong channelTracks;

#ifdef HCG_CODE

_Array_ptr<nodeHCGType>			HCG : count(channelNets + 1);
_Array_ptr<ulong>				storageRootHCG : count(channelNets + 1);
_Array_ptr<ulong>				storageHCG : bounds(storageRootHCG, storageRootHCG + channelNets + 1);
ulong					storageLimitHCG;

#else	/* HCG_CODE */

extern _Array_ptr<nodeHCGType>			HCG : count(channelNets + 1);
extern _Array_ptr<ulong>			storageRootHCG : count(channelNets + 1);
extern _Array_ptr<ulong>			storageHCG : bounds(storageRootHCG, storageRootHCG + channelNets + 1);
extern ulong				storageLimitHCG;

#endif	/* HCG_CODE */


/*
 *
 * Prototypes.
 *
 */

#ifdef HCG_CODE

void
AllocHCG(void);

void
FreeHCG(void);

void
BuildHCG(void);

void
DFSClearHCG(_Array_ptr<nodeHCGType> : count(channelNets + 1));

void
DumpHCG(_Array_ptr<nodeHCGType> : count(channelNets + 1));

void
NoHCV(_Array_ptr<nodeHCGType> : count(channelNets + 1),
      ulong,
      _Array_ptr<ulong> : count(channelNets + 1),
      _Array_ptr<ulong> : count(channelTracks + 2));

#else	/* HCG_CODE */

extern void
AllocHCG(void);

extern void
FreeHCG(void);

extern void
BuildHCG(void);

extern void
DFSClearHCG(_Array_ptr<nodeHCGType> : count(channelNets + 1));

extern void
DumpHCG(_Array_ptr<nodeHCGType> : count(channelNets + 1));

extern void
NoHCV(_Array_ptr<nodeHCGType> : count(channelNets + 1),
      ulong,
      _Array_ptr<ulong> : count(channelNets + 1),
      _Array_ptr<ulong> : count(channelTracks + 2));

#endif	/* HCG_CODE */

#pragma CHECKED_SCOPE OFF
#endif	/* HCG_H */
