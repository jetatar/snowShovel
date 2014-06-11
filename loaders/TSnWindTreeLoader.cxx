#include "TSnWindTreeLoader.h"

#include <TString.h>

#include "TSnRawTreeMaker.h"
#include "TSnWindBranchLoader.h"


ClassImp( TSnWindTreeLoader );

TAMVirtualBranchLoader* TSnWindTreeLoader::CreateBranchLoader( 
                                        TTree* tree, TAMBranchInfo* brInfo )
{
    TString brname( brInfo->GetName() );

    if( brname.CompareTo(fBrName) == 0 )
    {
        return( new TSnWindBranchLoader(brInfo, fTName, fBrName,
                                        TSnRawTreeMaker::kEHdBrNm, fMeth) );
    }

    return( 0 );
}
