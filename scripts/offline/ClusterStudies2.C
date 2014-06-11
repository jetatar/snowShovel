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

const Char_t* inclfl    = "ic11.ch2.hm7.ffttimeclustersnocuts3600s.root";
//const Char_t* inclfl    = "/w2/jtatar/tmp3600s.root";
//const Char_t* inclfl    = "3600sic11.ffttimeclusters.root";
const Char_t* outfn     = "3600sic11.ch2.hm7.fftclusters.root";

//const Char_t* outfn     = "/w2/jtatar/tmp3600scluster_stn3.root";
//const Char_t* outfn     = "tmp.root";

const Double_t startTm  = 1389160000.; 
const Double_t endTm    = 1395500000.;


void ClusterStudies2( void )
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
    rSS->SetMinTime( startTm );
    rSS->SetMaxTime( endTm );

    // Filter out events with low number of FFT bins above FWHM.
    TSnSelEvtsOnHighFFTBins* highffts = 
                      new TSnSelEvtsOnHighFFTBins( "TSnSelEvtsOnHighFFTBins" );

    besmod->Add( rSS );
    rSS->Add( reemod );
    reemod->Add( highffts );

    TAModule* lastmod   = 0;
//    lastmod             = reemod;
    lastmod             = highffts;

    // Get times of clusters from file
    TFile* clfl              = new TFile( inclfl );
    TSnClusterTimes* cltimes = (TSnClusterTimes*)clfl->Get( "TSnClusterTimes" );

    if( cltimes != 0 )
    {
        std::vector< std::pair<Double_t, Double_t> >& sstimes = cltimes->Get( );

        Printf( "Number of clusters: %lu", sstimes.size() );

        Double_t totClustTm = 0.;
        // one for first evt, one for last
        Double_t DeadTm     = 2 * cltimes->GetMaxTmSep( ); 
        Double_t totDeadTm  = 0.;

        // Loop through pairs of times of clusters
        for( UInt_t i = 0; i < sstimes.size( ); i++ )
        {
            if( startTm <= cltimes->GetStartTm(i) )
            {
                totClustTm += (cltimes->GetStopTm(i) - cltimes->GetStartTm(i));
        
                if( i > 0 )
                {
                    Double_t gap = ( cltimes->GetStartTm(i) - 
                                     cltimes->GetStopTm(i - 1) );

                    if( gap >= 2 * cltimes->GetMaxTmSep() )
                    {
                        DeadTm += 2 * cltimes->GetMaxTmSep( );
                    }
                    else
                    {
                        DeadTm += gap;
                    }
                }        
            }


            TSnRejectBadClocksMod* rbcmod   =
                new TSnRejectBadClocksMod( Form("RejectBadClocksMod%d", i) ); 
//            rbcmod->SetVerbosity( 100 );
    
            rbcmod->SetInclude( 0 );
            rbcmod->SetMinTime( cltimes->GetStartTm(i) - 1 );
            rbcmod->SetMaxTime( cltimes->GetStopTm(i) + 1 );

            lastmod->Add( rbcmod );
            lastmod = rbcmod;
        }

        totDeadTm = totClustTm + DeadTm;

        Printf( "Total time cut: %f", totClustTm/60/60/24 );
        Printf( "Total dead time: %f", totDeadTm/60/60/24 );
    }
    else
    {
        Printf( "Couldn't get object from %s!", inclfl );
    }

    // Get the template waveforms for correlation coefficient calculations.
    TFile* tpltfl = new TFile( tpltflnm );
    TTree* tplttr = (TTree*)tpltfl->Get( "Templates" );

    // Get template waveform samples and store them in a vec to pass to corr.       // coeff module.
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
//    TSnSelEvtsOnCC* ccmod = new TSnSelEvtsOnCC( "TSnSelEvtsOnCC", tpltwv );
    TSnCorrelateWavesMod* ccmod = 
            new TSnCorrelateWavesMod( "TSnCorrelateWavesMod", tpltwv );

    TSnPlotCCMod* plotmod = new TSnPlotCCMod( "PlotMod" );

    lastmod->Add( ccmod );
    ccmod->Add( plotmod ); 

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

