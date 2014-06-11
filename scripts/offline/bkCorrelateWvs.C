#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TChain.h>
#include <TFile.h>

#include "TAMSelector.h"
#include "TAMOutput.h"

#include "TSnRawTreeMaker.h"
#include "TSnBasicEvtSelMod.h"
#include "TSnCorrelateWvs.h"
#include "NSnConstants.h"
#include "TSnCorrelateWvs.h"
#include "TSnSaveCalibDataMod.h"
#include "TSnRejectEarlyEvtsMod.h"
#include "TSnConfigTreeLoader.h"
#include "TSnRejectBadClocksMod.h"

#endif

TChain* nt( 0 );
TFile* fout;

void CorrelateWvs(  const Char_t* indtfl, const Char_t* intmfl,
                    const Char_t* outfl,
                    const Bool_t useThermals    = kTRUE,
                    const Bool_t useForced      = kFALSE )
{
    delete nt;

    nt = new TChain( TSnSaveCalibDataMod::kFpnEvtTreeNm );
    nt->Add( indtfl );

    TAMSelector* sel = new TAMSelector;

    TSnBasicEvtSelMod* besmod = new TSnBasicEvtSelMod( "BasicEvtSelMod" );
    
    TSnConfigTreeLoader* confLoader = new TSnConfigTreeLoader;
    sel->AddLoader( confLoader );
    
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
    rbcmod->SetInclude( 0 );

    rbcmod->SetMinTime( 1391642000 );
    rbcmod->SetMaxTime( 1392074400 );


    besmod->Add( reemod );
    reemod->Add( rbcmod );
    
    besmod->SetCheckCRC( kTRUE );

    TFile* tmfl = new TFile( intmfl );
    TTree* tr = (TTree*)tmfl->Get( "runtree" );

    Float_t tpdt[NSnConstants::kNsamps];

    tr->SetBranchAddress( "dataps00", &tpdt[0] );
    
    std::vector< Float_t > vectpdt( NSnConstants::kNsamps );

    tr->GetEntry( 0 );

    for( UShort_t s = 0; s < NSnConstants::kNsamps; s++ )
    {
        vectpdt[s] = tpdt[s]; 
    }

    TSnCorrelateWvs* cwmod = new TSnCorrelateWvs( "CorrelateWvs", vectpdt );
    
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
