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
#include "TSnChanTimeCorlAltEvtsMod.h"
#include "TSnFindPureThermalFromAutoCorrMod.h"

#include "NSnConstants.h"

#endif

// make a softlink: "ln -s ../offline/loadtree.C"
#include "loadtree.C" 


TChain* nt( 0 );
TFile*  fout;

// Station number
const Char_t* stnNum    = "3";

// Input file
TString infl      = 
//    Form("/w1/jtatar/Analysis/Stn%s/CalTree.CombNoHtbt.RawTree.Stn%sEvts.root", 
//                                                            stnNum, stnNum);
//    Form("/w1/arianna/2014_InSitu/combinedNoHtbt/CalTree/"
//         "CalTree.CombNoHtbt.RawTree.Stn%sEvts.root", 
   Form("/w1/arianna/2014_InSitu/combinedNoHtbt/CalTree/"
        "SingleSidedInvertedCh2Ch3/CalTree.CombNoHtbt.RawTree.Stn%sEvts.root", 
         stnNum);//, stnNum);



// Output file
TString outfn     = 
//    Form("/w1/jtatar/Analysis/Stn%s/pl.stn%s_minbias.root", 
//                                                            stnNum, stnNum);
   Form("~/work/BkgndReject/NHM/"
        "plots.stn%s.SingleSidedInvertedCh2Ch3.frcPureThemNonThm.root", stnNum);
   
// Signal Templates file
//const Char_t* sigflnm   = "/w1/jtatar/Analysis/Templates/nt.sigtemps.root";
const Char_t* sigflnm   = "/w1/arianna/jtatar/Templates/nt.sigtemps.root";


void NuAnalysis( void )
{
    delete nt;

    TString dataBranchName;
    const Bool_t bOk = tryLoadDataTree( infl.Data(), nt, &dataBranchName );

    if( !bOk )
    {
        Printf( "! Could not load data from: %s.\n", infl.Data() );

        return;
    }

    TAMSelector* sel        = new TAMSelector;

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
    
    // use module hierarchy to make 3 bins of plots:
    //               reject early evts
    //       +--------------^----------------+
    //  forced trgs                     thermal trgs
    //                                       |
    //                                 auto correlate
    //                                       |
    //       |                    +----------^-----------+
    //  frc trg plots        select pure thm         select non-thm
    //                            |                      |
    //                       pure thm plots          non-thm plots
    
    // Select forced events
    TSnBasicEvtSelMod*  besfrc = new TSnBasicEvtSelMod( "Forced" );
    besfrc->GetTrgBits().EnableTrig( TSnTrgInfo::kForced );
    besfrc->SetCheckCRC( kTRUE );

    // Select THERMAL events
    TSnBasicEvtSelMod*  besthm = new TSnBasicEvtSelMod( "Thermal" );
    besthm->GetTrgBits().EnableTrig( TSnTrgInfo::kThermal );
    besthm->SetCheckCRC( kTRUE );
    
    // find auto correlation
    TSnChanTimeCorlAltEvtsMod* autocc
       = new TSnChanTimeCorlAltEvtsMod("AutoCorrelateMod",
                                       dataBranchName.Data(),
                                       dataBranchName.Data());
    autocc->SetCorrelationsName("AutoCorrelations.");
    
    // select pure thermal
    TSnFindPureThermalFromAutoCorrMod* fptPureThm
       = new TSnFindPureThermalFromAutoCorrMod("SelPureThm");
    fptPureThm->SetTagBehavior("CutNonPureThermal");
    fptPureThm->SetAutoCorrName(autocc->GetCorrelationsName());
    
    // select non-thermal
    TSnFindPureThermalFromAutoCorrMod* fptNonThm
       = new TSnFindPureThermalFromAutoCorrMod("SelNonThm");
    fptNonThm->SetTagBehavior("CutPureThermal");
    fptNonThm->SetAutoCorrName(autocc->GetCorrelationsName());
    
    
    // Remove events with a sharply peaked FFT distribution.
    TSnPlotNumHighFFTBins* nhfftFrc = 
       new TSnPlotNumHighFFTBins( "ForcedPlots", "Forced" );
    TSnPlotNumHighFFTBins* nhfftPureThm = 
       new TSnPlotNumHighFFTBins( "PureThermPlots", "PureThm" );
    TSnPlotNumHighFFTBins* nhfftNonThm = 
       new TSnPlotNumHighFFTBins( "NonThermPlots", "NonThm" );
    
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

/*
    // Order event processing modules 
    sel->AddInput( bes );
//    bes->Add( nhfft );
    bes->Add( ree );
//    ree->Add( selCC );
    ree->Add( nhfft );
//    selCC->Add( nhfft );
*/
    
    sel->AddInput( ree );
    // forced branch
    ree->Add(besfrc);
    besfrc->Add(nhfftFrc);
    // thermal branch
    ree->Add(besthm);
    besthm->Add(autocc);
    // thermal/pure sub-branch
    autocc->Add(fptPureThm);
    fptPureThm->Add(nhfftPureThm);
    // thermal/bkgnd sub-branch
    autocc->Add(fptNonThm);
    fptNonThm->Add(nhfftNonThm);
    
    // PROCESS events.
    
    sel->SetVerbosity(10);
    
    Printf( "Processing events..." );
    nt->Process( sel, "");
    //nt->Process( sel, "", 100 );
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

