

#include <stdio.h>
#include <sstmac/replacements/mpi.h>

namespace errcode {
void ChkMsg( int, int, const char [] );

/**
 * This routine is used to check the message associated with an error
 * code.  Currently, it uses MPI_Error_string to get the corresponding
 * message for a code, and prints out the cooresponding class and original
 * message.
 * 
 * Eventually, we should also access the generic anc specific messages
 * separately.
 */
void ChkMsg( int err, int msgclass, const char msg[] )
{
    char errmsg[MPI_MAX_ERROR_STRING];
    int len;

    MPI_Error_string( err, errmsg, &len );
    
    fprintf( stdout, "[0x%08x] %2d %s \tgives %s\n", err, msgclass, msg, errmsg );
}

}
