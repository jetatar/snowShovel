#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TObjArray.h>

#include "TAMSelector.h"
#include "TAMOutput.h"

#include "TSnRawTreeMaker.h"
#include "TSnBasicEvtSelMod.h"
#include "TSnPruneBadEvtsMod.h"
#include "TSnConfigTreeLoader.h"
#include "NSnConstants.h"
#include "TSnSaveCalibDataMod.h"

#endif

TChain* nt( 0 );
TFile* fout;

void PruneBadEvents( const Char_t* infn, const Char_t* outfn )
{
    delete nt;

    nt = new TChain( TSnSaveCalibDataMod::kFpnEvtTreeNm );
    nt->Add( infn );

    TAMSelector* sel = new TAMSelector;

    TSnConfigTreeLoader* confLoader = new TSnConfigTreeLoader;
    sel->AddLoader( confLoader );
    
    TSnBasicEvtSelMod* besmod = new TSnBasicEvtSelMod( "ThmTrgSelMod" );  
    besmod->GetTrgBits().EnableTrig( TSnTrgInfo::kThermal );

    besmod->SetCheckCRC( kTRUE );

    TSnPruneBadEvtsMod* prnmod = new TSnPruneBadEvtsMod( "PruneBadEventsMod" );

    besmod->Add( prnmod );

    sel->AddInput( besmod );

    Printf( "Looking for bad events..." );

    nt->Process( sel, "" );

    Printf( "Finished processing events." );

    TAMOutput* mout = sel->GetModOutput( );

    if( mout != 0 )
    {
        fout = TFile::Open( outfn, "recreate" );
        fout->cd( );
        mout->Write( );

        delete fout;
        fout = 0;   // global variable so it is a good idea to set to NULL.
    }
    else
    {
        Printf( "Could not get the output list from selector." );
    }

    delete sel;
    delete confLoader;
}
