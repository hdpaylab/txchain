//
//  Rc4String
//
//  Outputs bytes from an RC4 stream. Key is taken from command line. Bytes are output as hex
//
//  This is free and unencumbered software released into the public domain - June 2013 waterjuice.org
//


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "rc4.h"


#ifndef __min
   #define __min( x, y )  (((x) < (y))?(x):(y))
#endif


#define BUFFER_SIZE             1024

//
//  Program entry point
//
int	main(int ac, char **av)
{
    char*           string;
    uint32_t        numBytes;
    uint32_t        i;
    uint8_t         buffer [BUFFER_SIZE];
    uint32_t        amountLeft;
    uint32_t        chunk;
    Rc4Context      rc4 = {0};
    uint32_t        dropN = 0;

    if( 3 != ac  &&  4 != ac )
    {
        printf(
            "Syntax\n"
            "   Rc4Output <Key> <NumBytes> [DropN]\n" );
        return 1;
    }

    string = av[1];
    numBytes = atoi( av[2] );
    if( 4 == ac )
    {
        dropN = atoi( av[3] );
    }

    Rc4Initialise( &rc4, string, (uint32_t)strlen(string), dropN );


    amountLeft = numBytes;
    while( amountLeft > 0 )
    {
        chunk = __min( amountLeft, BUFFER_SIZE );
        Rc4Output( &rc4, buffer, chunk );
        amountLeft -= chunk;

        for( i=0; i<chunk; i++ )
        {
            printf( "%2.2x", buffer[i] );
        }
    }

    printf( "\n" );

    return 0;
}

