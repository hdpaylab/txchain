//
//  Outputs MD5 hash of a string specified on command line. Hash is output in hex
//
//  This is free and unencumbered software released into the public domain - June 2013 waterjuice.org
//


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "md5.h"

//
//  Program entry point
//
int	main(int ac, char **av)
{
    char*           string;
    Md5Context      md5Context;
    MD5_HASH        md5Hash;
    uint16_t        i;

    if( 2 != ac )
    {
        printf(
            "Syntax\n"
            "   Md5String <String>\n" );
        return 1;
    }

    string = av[1];

    Md5Initialise( &md5Context );
    Md5Update( &md5Context, string, (uint32_t)strlen(string) );
    Md5Finalise( &md5Context, &md5Hash );

    for( i=0; i<sizeof(md5Hash); i++ )
    {
        printf( "0x%2.2x,", md5Hash.bytes[i] );
    }
    printf( "\n" );

    return 0;
}

