#include "TFile.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TH1F.h" 
#include "TObjArray.h"
#include "TMath.h"
#include "TTree.h"

static const Char_t* inflnm = 
        "/w1/jtatar/Analysis/Stn0/result_Jul16_1000000_GZK_5sig_realAT.root";

//TString outflnm = "/w1/jtatar/Analysis/Stn0/nt.relStnVolt.root";
TString outflnm = "nt.relStnVoltNoNoise.root";


const UShort_t kNchans      = 4;
const Float_t noiseFloor    = 1.752696e-5; 
const Float_t threshold     = noiseFloor * 5.;

UInt_t nNegVolt = 0;


Float_t* calcRatios( Float_t v[kNchans] )
{
    Float_t* ratio = new Float_t[kNchans];
    
    for( UShort_t c = 0; c < kNchans; c++ )
    {
        if( v[c] > 0. )
        {
            ratio[c] = v[0] / v[c];
        }
        else
        {
            nNegVolt++;

            delete[] ratio;        
    
            return NULL;
        }
    }

    return ratio;
}


void TriggeredStationVoltages( void )
{
    // Input File & Tree.
    TFile* infl = new TFile( inflnm );
    TTree* intr = (TTree*)infl->Get( "PAM" );

    // Output File & Tree
    TFile* outfl = new TFile( outflnm, "recreate" );
    TTree* outtr = new TTree( "VoltRatio", "" );

    TObjArray* hRelToThresh = new TObjArray( kNchans );
    hRelToThresh->SetName( "hRelToThresh" );

    TH1F* hrtt[kNchans];


    for( UChar_t ch = 0; ch < kNchans; ch++ )
    {
        hrtt[ch] = new TH1F( Form("hrtt%d", ch), "", 501, -0.05, 50.05 ); 
        hRelToThresh->AddAt( hrtt[ch], ch );
    }

    Float_t voltRatio[kNchans]; 
    Float_t volt[kNchans];
    Float_t vRelToThresh[kNchans];
    Float_t vRelToThreshProper[kNchans];   
 
    outtr->Branch( "voltRatio", &voltRatio, "voltRatio[4]/F" ); 
    outtr->Branch( "volt",      &volt,      "volt[4]/F" ); 
    outtr->Branch( "vRelToThresh", &vRelToThresh, "vRelToThresh[4]/F" );
    outtr->Branch( "vRelToThreshProper", &vRelToThreshProper, 
                                            "vRelToThreshProper[4]/F" );

    Float_t direct[4];
    Float_t reflected[4];

    // With noise.
//    intr->SetBranchAddress( "volt_LPA", &(direct[0]) );
//    intr->SetBranchAddress( "volt_LPA_mirror", &(reflected[0]) );

    // No noise added.
    intr->SetBranchAddress( "volt_LPA_preNoise", &(direct[0]) );
    intr->SetBranchAddress( "volt_LPA_mirror_preNoise", &(reflected[0]) );

    ULong64_t nevts = intr->GetEntries( );

    Printf( "Total number of events: %llu", nevts );

    ULong64_t ntrigs    = 0;

    ULong64_t totDir    = 0;
    ULong64_t totRef    = 0;
    ULong64_t totNon    = 0;
    ULong64_t totBoth   = 0;

    for( ULong64_t i = 0; i < nevts; i++ )
    {
        intr->GetEntry( i );

        UShort_t nDir = 0;
        UShort_t nRef = 0;
    
        Float_t* dirPassRatio = NULL;
        Float_t* refPassRatio = NULL;

        // Count number of channels above threshold.
        for( UShort_t c = 0; c < kNchans; c++ )
        {
            if( direct[c] >= threshold )
            {
                nDir++;
            }
            if( reflected[c] >= threshold )
            {
                nRef++;
            }
        } 

        // Check that at least two of the channels in the event are above 
        // threshold and calculate ratios.
        if( nDir >= 2 )
        {
            dirPassRatio = calcRatios( direct );
        }
        if( nRef >= 2 )
        {
            refPassRatio = calcRatios( reflected );
        }
        if( nDir < 2 && nRef < 2 )
        {
            totNon++;

            Printf( "!!! Event %d does not have two channels with " 
                    "direct or reflected values above threshold", i );

            continue;
        }

        // Write ratios for events.
        outtr->SetDirectory( outfl );

        if( refPassRatio != NULL && dirPassRatio != NULL )
        {
            totBoth++;
        }


        if( dirPassRatio != NULL )
        {
            Int_t idx[kNchans];

            for( UChar_t ch = 0; ch < kNchans; ch++ )
            {
                vRelToThreshProper[ch] = direct[ch] / threshold;
            }

            TMath::Sort( (Int_t)(kNchans), direct, idx );

            for( UShort_t c = 0; c < kNchans; c++ )
            {
                volt[c]         = direct[c];
                voltRatio[c]    = dirPassRatio[c];
                vRelToThresh[c] = (direct[idx[c]] / threshold);

                if( vRelToThresh[c] < 0. )
                {
                    Printf( "!!! Ratio smaller than 0!" );
                }

                hrtt[c]->Fill( vRelToThresh[c] );
            }
            
            totDir++;

            outtr->Fill( );

            delete[] dirPassRatio;
        }

        if( refPassRatio != NULL )
        {
            Int_t idx[kNchans];

            for( UChar_t ch = 0; ch < kNchans; ch++ )
            {
                vRelToThreshProper[ch] = reflected[ch] / threshold;
            }

            TMath::Sort( (Int_t)(kNchans), reflected, idx );

            for( UShort_t c = 0; c < kNchans; c++ )
            {
                volt[c]         = reflected[c];
                voltRatio[c]    = refPassRatio[c];
                vRelToThresh[c] = (reflected[idx[c]] / threshold);

                if( vRelToThresh[c] < 0. )
                {
                    Printf( "!!! Ratio smaller than 0!" );
                }

                hrtt[c]->Fill( vRelToThresh[c] );
            }

            totRef++;

            outtr->Fill( );

            delete[] refPassRatio;
        }
    }

 
    outfl->Write( );
    outfl->Close( );


    Printf( "Total: %d Dir: %d, Ref: %d, Both: %d, Neither: %d, Negative: %d", 
                    nevts, totDir, totRef, totBoth, totNon, nNegVolt );
}
