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

TChain* nt( 0 );
TFile*  fout;

const Char_t* infl      = 
                    "/w2/jtatar/Analysis/Stn11/FPNSubTree.nt.ic11.all.root";

const Char_t* tpltflnm  = 
                    "/w2/jtatar/Analysis/Templates/nt.sigtemps.root";

const Char_t* outfn     = "ic11.ch2.hm7.ffttimeclustersnocuts";

//const Char_t* outfn = "/w2/jtatar/tmp";

Double_t maxclusttm     = 3600.;

//void ClusterStudies( const Char_t* infl, const Char_t* outfl )
void ClusterStudies( void )
{
    delete nt;

    nt = new TChain( TSnSaveCalibDataMod::kFpnEvtTreeNm );
    nt->Add( infl );

    TAMSelector* sel                = new TAMSelector;
    TSnBasicEvtSelMod* besmod       = new TSnBasicEvtSelMod( "BasicEvtSelMod" );
//    besmod->SetVerbosity( 100 );
    besmod->GetTrgBits().EnableTrig( TSnTrgInfo::kThermal );
//    besmod->GetTrgBits().EnableTrig( TSnTrgInfo::kForced );
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
//    reemod->SetVerbosity( 100 );    


    // Filter out events with low number of FFT bins above FWHM.
    TSnSelEvtsOnHighFFTBins* highffts = 
                      new TSnSelEvtsOnHighFFTBins( "TSnSelEvtsOnHighFFTBins" );

    // Get the template waveforms for correlation coefficient calculations.
    TFile* tpltfl = new TFile( tpltflnm );
    TTree* tplttr = (TTree*)tpltfl->Get( "Templates" );

    // Get template waveform samples and store them in a vec to pass to corr.
    // coeff module.    
    std::vector< Float_t > tpltwv( NSnConstants::kNsamps );
    TSnCalWvData* wv = new TSnCalWvData( "tempwave", "TSnCal" );

    tplttr->SetBranchAddress( "wave.", &wv );
    
    tplttr->GetEntry( 28 );

    for( UChar_t s = 0; s < NSnConstants::kNsamps; s++ )
    {
        tpltwv[s] = static_cast< Float_t >( *(wv->GetData(0) + s) );
    }

    // Compute correlation coefficients and skip events with low correlation
    // coefficient
    TSnSelEvtsOnCC* ccmod = new TSnSelEvtsOnCC( "TSnSelEvtsOnCC", tpltwv );
//    TSnCorrelateWavesMod* ccmod = 
//                    new TSnCorrelateWavesMod( "Compute Corr. Coeff.", tpltwv );
    // Create clusters of times (start time, stop time) corresponding to
    // event clusters and save output.

    TSnCluster* clust = new TSnCluster( "TSnCluster", maxclusttm );

    
//    ccmod->SetVerbosity( 10 );

    sel->AddInput( besmod );
    besmod->Add( rSS );
    rSS->Add( reemod );
    reemod->Add( highffts );
    highffts->Add( ccmod );
//    reemod->Add( ccmod );
//    ccmod->Add( highffts );
    ccmod->Add( clust );

    Printf( "Doing work..." );
    nt->Process( sel, "" );

    Printf( "Done working." );

    TAMOutput* modout = sel->GetModOutput( );
    
    if( modout != 0 )
    {
        fout = TFile::Open( Form("%s%ds.root", outfn, (UInt_t)maxclusttm), 
                                                                "recreate" );
        
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

