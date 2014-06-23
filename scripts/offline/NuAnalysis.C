/*
    Steering script for data analysis for 2013-2014 season.
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
#include "TSnCluster.h"
#include "TSnConfigTreeLoader.h"
#include "TSnCalWvData.h"
#include "TSnSelEvtsOnHighFFTBins.h"
#include "TSnCorrelateWavesMod.h"
#include "TSnRejectBadClocksMod.h"

#include "NSnConstants.h"

#endif

#include "loadtree.C" 


TChain* nt( 0 );
TFile*  fout;

// Station number
const Char_t* stnNum    = "3";

// Input file
const Char_t* infl      = Form("/w2/jtatar/Analysis/Stn%s/CalTree.CombNoHtbt.RawTree.Stn%sEvts.root", stnNum, stnNum);

// Output file
const Char_t* outfn     = Form("/w2/jtatar/Analysis/Stn%s/pl.stn%s.root", stnNum, stnNum);


void NuAnalysis( void )
{
    delete nt;

    const Bool_t bOk = tryLoadDataTree( infl, nt );

    if( !bOk )
    {
        Printf( "! Could not load data from: %s.\n", infl );

        return;
    }

    TAMSelector* sel        = new TAMSelector;

    // Select THERMAL events
    TSnBasicEvtSelMod*  bes = new TSnBasicEvtSelMod( "BES" );
    bes->GetTrgBits().EnableTrig( TSnTrgInfo::kThermal );
    bes->SetCheckCRC( kTRUE );

    // Remove events with a sharply peaked FFT distribution.
    TSnSelEvtsOnHMFFTWidth


    // PROCESS events.
    Printf( "Processing events..." );
    nt->Process( sel, "" );
    Printf( "Finished processing events." );

    // Write OUTPUT
    TAMOutput* outmod = sel->GetModOutput( );
    
    if( outmod != 0 )
    {
        Printf( "Writing output..." );
        fout = TFile::Open( outfn, "recreate" );

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

