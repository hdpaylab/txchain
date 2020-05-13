//
//  Sha512String
//
//  Outputs SHA512 hash of a string specified on command line. Hash is output in hex
//
//  This is free and unencumbered software released into the public domain - June 2013 waterjuice.org
//
// Performance test:
//	1,000,000 / 1.4 seconds @250 bytes string (1.4 us)
//	1,000,000 / 3.5 seconds @1000 bytes string (3.5 us)
//


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "sha512.h"


//
//  Program entry point
//
int	main(int ac, char **av)
{
    char*           string;
    Sha512Context   sha512Context;
    SHA512_HASH     sha512Hash;
    uint16_t        i;

    if( 2 != ac )
    {
        printf(
            "Syntax\n"
            "   Sha512String <String>\n" );
        return 1;
    }

    string = av[1];

    for (int nn = 0; nn < 1000000; nn++)
    {
    Sha512Initialise( &sha512Context );
    Sha512Update( &sha512Context, string, (uint32_t)strlen(string) );
    Sha512Finalise( &sha512Context, &sha512Hash );
    }

    for( i=0; i<sizeof(sha512Hash); i++ )
    {
        printf( "%2.2x", sha512Hash.bytes[i] );
    }
    printf( "\n" );

    return 0;
}

