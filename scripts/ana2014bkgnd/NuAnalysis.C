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
const Char_t* stnNum    = "11";

// Input file
const Char_t* infl      = 
    Form("/w1/jtatar/Analysis/Stn%s/CalTree.CombNoHtbt.RawTree.Stn%sEvts.root", 
                                                            stnNum, stnNum);

// Output file
const Char_t* outfn     = 
    Form("/w1/jtatar/Analysis/Stn%s/pl.stn%s_ampcalib_minbias.root", 
                                                            stnNum, stnNum);
// Signal Templates file
const Char_t* sigflnm   = "/w1/jtatar/Analysis/Templates/nt.sigtemps.root";


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
    //bes->GetTrgBits().EnableTrig( TSnTrgInfo::kThermal );
    bes->GetTrgBits().EnableTrig( TSnTrgInfo::kForced );
    bes->SetCheckCRC( kTRUE );

    // Config Tree Loader needed by TSnRejectEarlyEvtsMod
    TSnConfigTreeLoader* confLoader = new TSnConfigTreeLoader;
    sel->AddLoader( confLoader );

    // Reject the first event of every sequence, if it happens to be thermal
    // or forced, depending on BES.
    TSnRejectEarlyEvtsMod* ree  = 
                            new TSnRejectEarlyEvtsMod( "RejectEarlyEvtsMod" );
    // Reject only first event within 2 sec rather than the default, 
    // which is all events within 2 sec of the start of a sequence.
    ree->SetRejectOnlyFirstEvent( 1 );

    // Remove events with a sharply peaked FFT distribution.
    TSnPlotNumHighFFTBins* nhfft = 
                            new TSnPlotNumHighFFTBins( "PlotNumHighFFTBins" );

    //
    //  Correlation Coefficients Calculation Section
    //

    // Get the template waveforms for correlation coefficient calculations.
    TFile* sigfl = new TFile( sigflnm );
    TTree* sigtr = (TTree*)sigfl->Get( "Templates" );

    // Get template waveform samples and store them in a vec to pass to corr.
    // coeff module.    
    std::vector< Float_t > sigwv( NSnConstants::kNsamps );
    TSnCalWvData* wv = new TSnCalWvData( "tempwave", "TSnCal" );

    sigtr->SetBranchAddress( "wave.", &wv );
    
    sigtr->GetEntry( 28 );

    for( UChar_t s = 0; s < NSnConstants::kNsamps; s++ )
    {
        sigwv[s] = static_cast< Float_t >( *(wv->GetData(0) + s) );
    }

    // Compute correlation coefficients and skip events with low correlation
    // coefficient
    TSnSelEvtsOnCC* selCC = new TSnSelEvtsOnCC( "TSnSelEvtsOnCC", sigwv );
    //
    //  End Correlation Coefficients Calculation Section
    //


    // Order event processing modules 
    sel->AddInput( bes );
    bes->Add( nhfft );
//    bes->Add( ree );
//    ree->Add( selCC );
//    selCC->Add( nhfft );

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

