
/*
 *
 * option.c
 *
 */

#include <string_checked.h>

#define OPTION_CODE


/*
 *
 * Includes.
 *
 */

#include <stdio_checked.h>
#include <stdlib_checked.h>
#include "channel.h"

#pragma BOUNDS_CHECKED ON
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
