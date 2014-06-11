#if !defined(__CINT__) || defined(__MAKECINT__)

#include <vector>

#include <TChain.h>
#include <TFile.h>
#include <TTree.h>

#include "TAMSelector.h"
#include "TAMOutput.h"

#include "TSnSaveCalibDataMod.h"
#include "TSnBasicEvtSelMod.h"
#include "TSnRejectEarlyEvtsMod.h"
#include "TSnSelEvtsOnCC.h"
#include "TSnCluster.h"
#include "TSnConfigTreeLoader.h"
#include "TSnCalWvData.h"
#include "TSnSelEvtsOnHighFFTBins.h"
#include "TSnCorrelateWavesMod.h"
#include "TSnRejectBadClocksMod.h"
#include "TSnCountEventsMod.h"

#include "NSnConstants.h"

#endif

TChain* nt( 0 );
TFile*  fout;

const Char_t* infl      = 
                    "/w2/jtatar/Analysis/Stn3/FPNSubTree.nt.ic3.all.root";

void GetNumEvts( void )
{
    delete nt;

    nt = new TChain( TSnSaveCalibDataMod::kFpnEvtTreeNm );
    nt->Add( infl );

    TAMSelector* sel                = new TAMSelector;
    TSnBasicEvtSelMod* besmod       = new TSnBasicEvtSelMod( "BasicEvtSelMod" );
//    besmod->SetVerbosity( 100 );
//    besmod->GetTrgBits().EnableTrig( TSnTrgInfo::kThermal );
    besmod->GetTrgBits().EnableTrig( TSnTrgInfo::kForced );
    besmod->SetCheckCRC( kTRUE );

    TSnConfigTreeLoader* confLoader = new TSnConfigTreeLoader;
    sel->AddLoader( confLoader );

    TSnRejectEarlyEvtsMod* reemod   =
                            new TSnRejectEarlyEvtsMod( "RejectEarlyEvtsMod" );

    TSnRejectBadClocksMod* rSS      =
                            new TSnRejectBadClocksMod( "Start_Stop_Tm" );
    rSS->SetInclude( 1 );
    rSS->SetMinTime( 1389160000. );
    rSS->SetMaxTime( 1395500000. );
    
    TSnCountEventsMod* countevts    = 
                            new TSnCountEventsMod( "TSnCountEventsMod" );

    sel->AddInput( besmod );
    besmod->Add( rSS );
    rSS->Add( countevts );
//    rSS->Add( reemod );
//    reemod->Add( countevts );

    Printf( "Doing work..." );
    nt->Process( sel, "" );
    Printf( "Done working." );

    delete sel;
}

