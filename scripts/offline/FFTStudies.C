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
#include "TSnRejectBadClocksMod.h"
#include "TSnPlotCCMod.h"
#include "TSnCorrelateWavesMod.h"
#include "TSnCalWvData.h"
#include "TSnSelEvtsOnHighFFTBins.h"

#include "NSnConstants.h"

#endif


TChain* nt( 0 );
TFile*  fout;

const Char_t* infl      =
                    "/w2/jtatar/Analysis/Stn11/FPNSubTree.nt.ic11.all.root";
//                    "/w2/arianna/DATA/2013_Bounce_Studies/StationData/CombinedCalibTrees/CalTree.RawTree.SnEvtsM0002F7F0C61Ar00043s19-35.root";

const Char_t* tpltflnm  = "/w2/jtatar/Analysis/Templates/nt.sigtemps.root";

const Char_t* inclfl    = "ic11.ffttimeclustersnocuts3600s.root";
//const Char_t* inclfl    = "/w2/jtatar/tmp3600s.root";
//const Char_t* inclfl    = "3600sic11.ffttimeclusters.root";
//const Char_t* outfn     = "3600sic11.fftclusters.root";
const Char_t* outfn     = "/w2/jtatar/stn3.fftstudies.root";

const Double_t startTm  = 1389160000.; 
const Double_t endTm    = 1395500000.;


void FFTStudies( void )
{
    delete nt;

    nt = new TChain( TSnSaveCalibDataMod::kFpnEvtTreeNm );
//    nt = new TChain( "CalibTree" );
    nt->Add( infl );

    TAMSelector* sel = new TAMSelector;
    TSnBasicEvtSelMod* besmod       = new TSnBasicEvtSelMod( "BasicEvtSelMod" );
    besmod->GetTrgBits().EnableTrig( TSnTrgInfo::kThermal );
    besmod->SetCheckCRC( kTRUE );

    // Needed by TSnRejectEarlyEvtsMod
    TSnConfigTreeLoader* confLoader = new TSnConfigTreeLoader;
    sel->AddLoader( confLoader );

    TSnRejectEarlyEvtsMod* reemod   =
                            new TSnRejectEarlyEvtsMod( "RejectEarlyEvtsMod" );

    sel->AddInput( besmod );

    TSnRejectBadClocksMod* rSS    =
             new TSnRejectBadClocksMod( "Start_Stop_Tm" );
    rSS->SetInclude( 1 );
    rSS->SetMinTime( 1391812361 );
    rSS->SetMaxTime( 1391824694 );

    // Filter out events with low number of FFT bins above FWHM.
    TSnSelEvtsOnHighFFTBins* highffts = 
                      new TSnSelEvtsOnHighFFTBins( "TSnSelEvtsOnHighFFTBins" );

    besmod->Add( rSS );
    rSS->Add( reemod );
    reemod->Add( highffts );

    Printf( "Doing work..." );
    nt->Process( sel, "" );
    Printf( "Done working." );

    TAMOutput* modout = sel->GetModOutput( );
    
    if( modout != 0 )
    {
        fout = TFile::Open( outfn, "recreate" );
        
        fout->cd( );

        modout->Write( );

        delete fout;
        fout = 0;
    }
    else
    {
        Printf( "Could not get the output list from selector." );
    }

    delete sel;
}

