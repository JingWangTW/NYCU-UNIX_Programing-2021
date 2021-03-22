#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "io.h"

int main ( const int argc, char * const * argv )
{
    ARG * arguments = parse_input ( argc, argv );

    if ( arguments == NULL )
        exit ( EXIT_FAILURE );

    return 0;
}