//
//  Sha1String
//
//  Outputs SHA1 hash of a string specified on command line. Hash is output in hex
//
//  This is free and unencumbered software released into the public domain - June 2013 waterjuice.org
//
// Performance test:
//	1,000,000 / 1.2 seconds @250 bytes string (1.2 us)
//	1,000,000 / 2.8 seconds @1000 bytes string (2.8 us)
//


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "sha1.h"


//
//  Program entry point
//
int	main(int ac, char **av)
{
    char*           string;
    Sha1Context     sha1Context;
    SHA1_HASH       sha1Hash;
    uint16_t        i;

    if( 2 != ac )
    {
        printf(
            "Syntax\n"
            "   Sha1String <String>\n" );
        return 1;
    }

    string = av[1];

    for (int nn = 0; nn < 1000000; nn++)
    {
    Sha1Initialise( &sha1Context );
    Sha1Update( &sha1Context, string, (uint32_t)strlen(string) );
    Sha1Finalise( &sha1Context, &sha1Hash );
    }

    for( i=0; i<sizeof(sha1Hash); i++ )
    {
        printf( "%2.2x", sha1Hash.bytes[i] );
    }
    printf( "\n" );

    return 0;
}

