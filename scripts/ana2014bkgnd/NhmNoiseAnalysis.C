/*
    Takes finite bandwidth events created with James' covariance matrix and calculates N_{HM}
*/

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
//#include "TSnCluster.h"
#include "TSnConfigTreeLoader.h"
#include "TSnCalWvData.h"
#include "TSnPlotNumHighFFTBins.h"
//#include "TSnCorrelateWavesMod.h"
//#include "TSnRejectBadClocksMod.h"

#include "NSnConstants.h"

#endif

#include "/data/users/jtatar/Work/snowShovel/scripts/offline/loadtree.C" 


TChain* nt( 0 );
TFile*  fout;

// Station number
const Char_t* stnNum    = "0";

// Input file
//const Char_t* infl      = "/w1/jtatar/Analysis/Stn0/realisticSignal.root"; 
//const Char_t* infl = "/w1/jtatar/Analysis/Stn0/realisticSignal_Ch0EqCh2_ReflectedOnly.root";
const Char_t* infl      = "stn11.NoiseTree.root";
// Output file
TString outfn           = 
    Form("/w1/jtatar/Analysis/Stn%s/pl.stn%s_NhmwithStn11CovMatrix.root", 
                                                            stnNum, stnNum);
//TString outfn = "/w1/jtatar/Analysis/Stn0/pl.realisticSignalNhm_Ch0EqCh2_ReflectedOnly.root";

// Signal Templates file
const Char_t* sigflnm   = "/w1/jtatar/Analysis/Templates/nt.sigtemps.root";


void NhmNoiseAnalysis( void )
{
    delete nt;

    const Bool_t bOk = tryLoadDataTree( infl, nt );

    if( !bOk )
    {
        Printf( "! Could not load data from: %s.\n", infl );

        return;
    }

    TAMSelector* sel        = new TAMSelector;

    // Remove events with a sharply peaked FFT distribution.
    TSnPlotNumHighFFTBins* nhfft = 
                            new TSnPlotNumHighFFTBins( "PlotNumHighFFTBins" );

    sel->AddInput( nhfft );

    // PROCESS events.
    Printf( "Processing events..." );
    nt->Process( sel, "" );
    Printf( "Finished processing events." );


    // Write OUTPUT
    TAMOutput* outmod = sel->GetModOutput( );
    
    if( outmod != 0 )
    {
        Printf( "Writing output..." );
        fout = TFile::Open( outfn.Data(), "recreate" );

        fout->cd( );
        outmod->Write( );

        delete fout;
        fout = 0;

        Printf( "Done writing output." );
    }
    else
    {
        Printf( "!!! Can't get output list from selector!." );
    }

    delete sel;
    sel = 0;
}
