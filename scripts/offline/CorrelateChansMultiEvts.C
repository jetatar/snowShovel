#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TChain.h>
#include <TFile.h>

#include "TAMSelector.h"
#include "TAMOutput.h"

#include "TSnRawTreeMaker.h"
#include "TSnBasicEvtSelMod.h"
#include "TSnCorrelateChans.h"
#include "NSnConstants.h"
#include "TSnCorrelateChans.h"
#include "TSnSaveCalibDataMod.h"
#include "TSnRejectEarlyEvtsMod.h"
#include "TSnConfigTreeLoader.h"
#include "TSnRejectBadClocksMod.h"
#include "TSnHeartbeatTreeLoader.h"

#endif

#include "loadtree.C"

TChain* nt( 0 );
TFile* fout;

void CorrelateChans( const Char_t* indtfl, const Char_t* outfl,
                    const Bool_t useThermals    = kTRUE,
                    const Bool_t useForced      = kFALSE )
{
    delete nt;

    const Bool_t bOk = tryLoadDataTree( indtfl, nt );

    if( !bOk )
    {
        Printf( "! Could not load data from: %s.\n", indtfl );

        return;
    }

    TAMSelector* sel = new TAMSelector;

    TSnBasicEvtSelMod* besmod = new TSnBasicEvtSelMod( "BasicEvtSelMod" );
    
    TSnConfigTreeLoader* confLoader = new TSnConfigTreeLoader;
    sel->AddLoader( confLoader );

    TSnHeartbeatTreeLoader* hbLoader = new TSnHeartbeatTreeLoader;
    sel->AddLoader( hbLoader );
    
    if( useThermals )
    {
        besmod->GetTrgBits().EnableTrig( TSnTrgInfo::kThermal );
    }
    if( useForced )
    {
        besmod->GetTrgBits().EnableTrig( TSnTrgInfo::kForced );
    }

    TSnRejectEarlyEvtsMod* reemod = 
                            new TSnRejectEarlyEvtsMod( "RejectEarlyEvtsMod" );

    TSnRejectBadClocksMod* rbcmod = 
                            new TSnRejectBadClocksMod( "RejectBadClocksMod" );

    besmod->Add( reemod );
    reemod->Add( rbcmod );
    
    besmod->SetCheckCRC( kTRUE );

    TSnCorrelateChans* cwmod = new TSnCorrelateChans( "CorrelateChans" );
    
    rbcmod->Add( cwmod );

    sel->AddInput( besmod );
     
    Printf( "Correlating waveforms..." );

    nt->Process( sel, "" );

    Printf( "Finished waveform correlation." );

    TAMOutput* mout = sel->GetModOutput( );

    if( mout != 0 )
    {
        fout = TFile::Open( outfl, "recreate" );
        fout->cd( );
        mout->Write( );

        delete fout;
        fout = 0;
    }
    else
    {
        Printf( "Could not get the output list from selector." );
    }

    delete sel;
}
