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
TString sigflnm = "/w1/jtatar/Analysis/Templates/nt.sigtemps.root";

// File containing the relevant channel voltages.
TString relflnm = "nt.relStnVoltNoNoise.root"; 

// Output file
TString outfn     = "/w1/jtatar/Analysis/Stn0/realisticSignal.root";

Float_t Amplitude       = 100.;    // in mV
ULong64_t numNoiseEvts  = 10000;


void realisticSignal( void )
{
    TRandom3* rand = new TRandom3( );

    TFile* sigfl = new TFile( sigflnm );
    TFile* relfl = new TFile( relflnm );

    TTree* sigtr = (TTree*)sigfl->Get( "Templates" );

    TH1F* hrel[NSnConstants::kNchans];
    TH1F* hScale[NSnConstants::kNchans];

    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        hrel[ch]    = dynamic_cast<TH1F*>( relfl->Get(Form("hrtt%d", ch)) );
        hScale[ch]  = new TH1F( Form("hScale%d", ch), "", 501, -0.05, 50.05 );
    }

    TSnCalWvData* wv = new TSnCalWvData( "Waveform", "TSnCalWvData" );

    sigtr->SetBranchAddress( "wave.", &wv );

    
    for( ULong64_t j = 0; j < numNoiseEvts; j++ )
    {
    sigtr->GetEntry( 28 );

//    Float_t gmax    = TMath::MaxElement(NSnConstants::kNsamps, gSig->GetY());
//    Float_t gmin    = TMath::MinElement(NSnConstants::kNsamps, gSig->GetY());

    Float_t* samp[NSnConstants::kNchans] = { NULL };

    // Copy the original signal waveform from Ch 0 to all other channels.
    // The script that generates the TSnCalWvData for the templates needs to be
    // modified so that all channels have the same waveform to start with.    
    for( UChar_t ch = 1; ch < NSnConstants::kNchans; ch++ )
    {
        for( UChar_t sm = 0; sm < NSnConstants::kNsamps; sm++ )
        {
            wv->SetData( ch, sm, wv->GetData(0, sm) );
        }
    }

    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        Float_t scaleCh = hrel[ch]->GetRandom( );

        while( scaleCh < 0. )
        {
            scaleCh = hrel[ch]->GetRandom( );
        }

        Printf( "Ch: %d, Val: %f", ch, scaleCh );
        hScale[ch]->Fill( scaleCh );        
 
        samp[ch] = wv->GetData( ch );

        for( UChar_t sm = 0; sm < NSnConstants::kNsamps; sm++ )
        {
            samp[ch][sm]    = (*(samp[ch] + sm) * Amplitude + rand->Gaus(0, 20))
                                                                    * scaleCh;
        }
        
//        Printf( "Evt: %llu, Amplitude: %f, Scale: %f", j, Amplitude, scaleCh );
    }
    }    
    TGraph* gSig[NSnConstants::kNchans];
    TCanvas* cScale[NSnConstants::kNchans];

    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        gSig[ch] = wv->NewGraphForChan( ch );
        cScale[ch] = new TCanvas( Form("cScale%d", ch), "", 500, 500 );

        if( ch == 0 )
        {
//            gSig[ch]->Draw( "APL*" );
            cScale[ch]->cd( );
            hScale[ch]->Draw( );
        }
        else
        {
//            gSig[ch]->Draw( "PL" );
//            gSig[ch]->SetLineColor( kRed + ch );
            cScale[ch]->cd( );
            hScale[ch]->Draw( );
        }
        
        hrel[ch]->Draw( "SAME" );
        hrel[ch]->SetLineColor( kMagenta );
        
        hScale[ch]->SetLineColor( ch + 1 );
    }
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

