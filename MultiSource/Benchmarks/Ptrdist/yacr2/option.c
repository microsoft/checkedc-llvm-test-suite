
/*
 *
 * option.c
 *
 */

#include <string.h>

#define OPTION_CODE


/*
 *
 * Includes.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "channel.h"

#pragma CHECKED_SCOPE ON
#define printf(...) _Unchecked { printf(__VA_ARGS__); }

/*
 *
 * Code.
 *
 */

void
Option(int argc,
       _Array_ptr<_Nt_array_ptr<char>> argv : count(argc))
{
    /*
     * Check arguments.
     */
    if (argc != 2) {
	printf("\nUsage: yacr2 <filename>\n\n");
	exit(1);
    }

    /*
     * Specified options.
     */
    _Unchecked { channelFile = argv[1]; }
}
