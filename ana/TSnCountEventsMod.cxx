#include "TSnCountEventsMod.h"

#include "TAModule.h"

ClassImp( TSnCountEventsMod );


void TSnCountEventsMod::SlaveBegin( void )
{
    fNumEvts = 0;
}


void TSnCountEventsMod::Process( void )
{
    fNumEvts++;
}


void TSnCountEventsMod::Terminate( void )
{
    Printf( "Number of events: %llu", fNumEvts );
}
