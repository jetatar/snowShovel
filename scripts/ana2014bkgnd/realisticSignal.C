/*
    Takes neutrino templates as input, adds noise, and scales amplitudes across
    channels appropriately.
*/

#if !defined(__CINT__) || defined(__MAKECINT__)

#include <vector>
#include <iostream>

#include <TChain.h>
#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TMath.h>
#include <TRandom3.h>
#include <TH1F.h>
#include <TCanvas.h>

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

// Signal Templates file
TString sigflnm     = "/w1/jtatar/Analysis/Templates/nt.sigtemps.root";

// File containing the relevant channel voltages.
//TString relflnm = "nt.relStnVoltNoNoise.root"; 
TString relflnm     = "nt.relStnVoltNoNoise_Ch0EqCh2_ReflectedOnly.root";

TString noiseflnm   = "/w1/jtatar/Analysis/Stn0/stn11.NoiseTree.root";

// Output file
//TString outfn     = "/w1/jtatar/Analysis/Stn0/realisticSignal_Ch0EqCh2_ReflectedOnly.root";
TString outfn = "/w1/jtatar/Analysis/Templates/nt.realisticSignalFiniteBW.root";


Float_t Amplitude       = 100.;    // in mV
ULong64_t numNoiseEvts  = 100;


void realisticSignal( void )
{
    TRandom3* rand = new TRandom3( );

    TFile* sigfl = new TFile( sigflnm );
    TFile* relfl = new TFile( relflnm );
    TFile* nozfl = new TFile( noiseflnm );
    TFile* outfl = new TFile( outfn, "recreate" );

    TTree* sigtr = (TTree*)sigfl->Get( "Templates" );
    TTree* reltr = (TTree*)relfl->Get( "VoltRatio" ); 
    TTree* noztr = (TTree*)nozfl->Get( "CalibTree" );
    outfl->cd( );
    TTree* outtr = new TTree( "CalibTree", "" );

    TH1F* hrel[NSnConstants::kNchans];
    TH1F* hScale[NSnConstants::kNchans];

    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        hrel[ch]    = dynamic_cast<TH1F*>( relfl->Get(Form("hrtt%d", ch)) );
        hScale[ch]  = new TH1F( Form("hScale%d", ch), "", 501, -0.05, 50.05 );
    }

    TSnCalWvData* wv    = new TSnCalWvData( "Waveform", "TSnCalWvData" );
    TSnCalWvData* outwv = new TSnCalWvData( "Wave", "TSnCalWvData" );

    Float_t vRelToThreshProper[NSnConstants::kNchans];
    TSnCalWvData* fbNoise = new TSnCalWvData;

    sigtr->SetBranchAddress( "wave.", &wv );
    noztr->SetBranchAddress( "AmpOutData.", &fbNoise );

    reltr->SetBranchAddress( "vRelToThreshProper", vRelToThreshProper );

    outtr->Branch( "AmpOutData.", &outwv );
    
    ULong64_t nevts     = reltr->GetEntries( );
    ULong64_t nsigevts  = sigtr->GetEntries( ); 

    for( ULong64_t i = 0; i < nsigevts; i++ )
    {
        sigtr->GetEntry( i );

        for( UChar_t ch = 1; ch < NSnConstants::kNchans; ch++ )
        {
            for( UChar_t sm = 0; sm < NSnConstants::kNsamps; sm++ )
            {
                wv->SetData( ch, sm, wv->GetData(0, sm) );
            }
        }

        for( ULong64_t j = 0; j < nevts; j++ )
        {
            reltr->GetEntry( j );
            noztr->GetEntry( j );

//    Float_t gmax    = TMath::MaxElement(NSnConstants::kNsamps, gSig->GetY());
//    Float_t gmin    = TMath::MinElement(NSnConstants::kNsamps, gSig->GetY());


//            Float_t* samp[NSnConstants::kNchans] = { NULL };

        // Copy the original signal waveform from Ch 0 to all other channels.
        // The script that generates the TSnCalWvData for the templates needs to 
        // be modified so that all channels have the same waveform to start with.
            outwv->ClearAll( );

            for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
            {
                Float_t scaleCh = hrel[ch]->GetRandom( );

                while( scaleCh < 0. )
                {
                    scaleCh = hrel[ch]->GetRandom( );
                }

//            Printf( "Ch: %d, Val: %f", ch, scaleCh );
//                hScale[ch]->Fill( scaleCh );        
 
//                samp[ch] = wv->GetData( ch );

                for( UChar_t sm = 0; sm < NSnConstants::kNsamps; sm++ )
                {
                    Float_t v = wv->GetData(ch, sm) * Amplitude *
                                vRelToThreshProper[ch];
                    v += fbNoise->GetData( ch, sm );
//                    Float_t v = wv->GetData(ch, sm);
                    outwv->SetData( ch, sm, v );
//                    samp[ch][sm]    = (*(samp[ch] + sm) * Amplitude  
//                                    * vRelToThreshProper[ch]) + rand->Gaus(0, 20);
                /*
                *
                * Uncomment code below to get the scale factor drawn from a

                * distribution of channel voltages sorted by size.
                *
                */

//                samp[ch][sm]    = (*(samp[ch] + sm) * Amplitude + 
//                                                rand->Gaus(0, 20)) * scaleCh;
                }
        
//        Printf( "Evt: %llu, Amplitude: %f, Scale: %f", j, Amplitude, scaleCh );
            }

//            outwv = wv;
        
            outtr->Fill( );
        }
    }

    outtr->Write( );
    outfl->Close( );
/*
    TGraph* gSig[NSnConstants::kNchans];
    TCanvas* cScale[NSnConstants::kNchans];

    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        gSig[ch] = wv->NewGraphForChan( ch );
        cScale[ch] = new TCanvas( Form("cScale%d", ch), "", 500, 500 );

        if( ch == 0 )
        {
            cScale[ch]->cd( );
            gSig[ch]->Draw( "APL*" );
//            hScale[ch]->Draw( );
        }
        else
        {
            cScale[ch]->cd( );
            gSig[ch]->Draw( "APL*" );
            gSig[ch]->SetLineColor( kRed + ch );
//            hScale[ch]->Draw( );
        }
        
//        hrel[ch]->Draw( "SAME" );
//        hrel[ch]->SetLineColor( kMagenta );
        
//        hScale[ch]->SetLineColor( ch + 1 );
    }
*/
/*
*   Should signal templates' amplitudes be symmetric around 0?  Probably not.
*   Should the Pk2Pk amplitude be scaled? 
*

    if( gmax > 0. )
    {
    }
    else
    {
    }

    if( gmin > 0. )
    {
    }    
    else
    {
    }
*/
}

