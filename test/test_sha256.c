//
//  Sha256String
//
//  Outputs SHA256 hash of a string specified on command line. Hash is output in hex
//
//  This is free and unencumbered software released into the public domain - June 2013 waterjuice.org
//
// Performance test:
//	1,000,000 / 1.9 seconds @250 bytes string (1.9 us)
//	1,000,000 / 6.0 seconds @1000 bytes string (6.0 us)
//


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "sha256.h"


//
//  Program entry point
//
int	main(int ac, char **av)
{
    char*           string;
    Sha256Context   sha256Context;
    SHA256_HASH     sha256Hash;
    uint16_t        i;

    if( 2 != ac )
    {
        printf(
            "Syntax\n"
            "   Sha256String <String>\n" );
        return 1;
    }

    string = av[1];

    for (int nn = 0; nn < 1000000; nn++)
    {
	Sha256Initialise( &sha256Context );
	Sha256Update( &sha256Context, (unsigned char*)string, (uint32_t)strlen(string) );
	Sha256Finalise( &sha256Context, &sha256Hash );
    }

    for( i=0; i<sizeof(sha256Hash); i++ )
    {
        printf( "%2.2x", sha256Hash.bytes[i] );
    }
    printf( "\n" );

    return 0;
}

