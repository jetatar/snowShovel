#if !defined(__CINT__) || defined(__MAKECINT__)

#include "TMath.h"
#include "TSnRecoChanOffsets.h"
#include "TFile.h"
#include "TObjArray.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLegend.h"

#include "NSnConstants.h"

#endif

const UChar_t kNChCmb       = TMath::Binomial( NSnConstants::kNchans, 2 );
static const UChar_t kNfls  = 8;    // number of input files

static const Char_t* infn[kNfls] = {   
                                // co polarized channels are Ch 1, 3
                                "gr.bounce.r35.root",
                                "gr.bounce.r36.root",
                                "gr.bounce.r42.root",
                                "gr.bounce.r43.2.root",
                                // co polarized channels are Ch 0, 2
                                "gr.bounce.r37.root",
                                "gr.bounce.r39.root",
                                "gr.bounce.r41.root",
                                "gr.bounce.r43.1.root" };

static const Char_t* outfn  = "gr.mkchancorrplots.root";

// LOWER co polarized channel (i.e: if ch 1 and 3, put 1; if ch 0 and 2, put 2)
UChar_t copoC[kNfls]        = { 1, 1, 1, 1, 0, 0, 0, 0 };
UChar_t crspoC[kNfls]       = { 0, 0, 0, 0, 1, 1, 1, 1 };
const Int_t cols[kNfls]     = { kRed, kRed, kRed, kRed, kBlack, kBlack, 
                            kBlack, kBlack };

const UChar_t y[kNfls]      = { 35, 36, 42, 44, 37, 39, 41, 43 };

void mkChanCorrPlots( void )
{
    Float_t meancopo[kNfls];
    Float_t meancrspo[kNfls];
    Float_t rmscopo[kNfls];
    Float_t rmscrspo[kNfls];

    TFile* fout = TFile::Open( outfn, "recreate" );

    TGraphErrors* gCopo02 = new TGraphErrors;
    TGraphErrors* gCopo13 = new TGraphErrors;
    
    TGraphErrors* gCrspo02 = new TGraphErrors;
    TGraphErrors* gCrspo13 = new TGraphErrors;

    for( UChar_t f = 0; f < kNfls; f++ )
    {
        Float_t mean[kNChCmb];
        Float_t rms[kNChCmb];

        TFile* ifl = TFile::Open( infn[f] );
 
        for( UChar_t c = 0; c < NSnConstants::kNchans; c++ )
        {
            for( UChar_t c2 = c + 1; c2 < NSnConstants::kNchans; c2++ )
            {
                TH1F* h = dynamic_cast<TH1F*>(ifl->Get(Form("hCC%d%d", c, c2)));

                UInt_t idx  = TSnRecoChanOffsets::IndexFor(c2, c);
                mean[idx]   = h->GetMean( );
                rms[idx]    = h->GetRMS( );
            }
        }

        meancopo[f]     = mean[TSnRecoChanOffsets::IndexFor(copoC[f] + 2, 
                                                                   copoC[f])];
        meancrspo[f]    = mean[TSnRecoChanOffsets::IndexFor(crspoC[f] + 2, 
                                                                   crspoC[f])];

        rmscopo[f]      = rms[TSnRecoChanOffsets::IndexFor(copoC[f] + 2, 
                                                                   copoC[f])];
        rmscrspo[f]     = rms[TSnRecoChanOffsets::IndexFor(crspoC[f] + 2, 
                                                                   crspoC[f])];

        Printf( "co po mean: %f rms: %f run: %u", meancopo[f], rmscopo[f], 
                                                                    y[f] );
        Printf( "cross po mean: %f rms: %f run: %u\n", meancrspo[f], 
                                                        rmscrspo[f], y[f] );

        ifl->Close( );
    }

//    Float_t y[kNfls]    = { 1., 1., 1., 1., 1., 1., 1., 1. };
    Float_t ye[kNfls]   = { 0., 0., 0., 0., 0., 0., 0., 0. }; 

    fout->cd( );

    UInt_t nc0 = 0;
    UInt_t nc1 = 0;

    for( UChar_t f = 0; f < kNfls; f++ )
    {
        if( copoC[f] == 0 && crspoC[f] == 1 )
        {
            gCopo02->SetPoint( nc0, meancopo[f], 0. );
            gCrspo13->SetPoint( nc0, meancrspo[f], 0. );

            gCrspo13->SetPointError( nc0, rmscrspo[f], ye[f] );
            gCopo02->SetPointError( nc0++, rmscopo[f], ye[f] );

            gCopo02->SetMarkerColor( kRed );
            gCopo02->SetLineColor( kRed );
            gCopo02->GetHistogram()->SetMinimum( -0.5 );
            gCopo02->GetHistogram()->SetMaximum( 1.5 );

            gCrspo13->SetMarkerColor( kRed );
            gCrspo13->SetLineColor( kRed );
            gCrspo13->GetHistogram()->SetMinimum( -0.5 );
            gCrspo13->GetHistogram()->SetMaximum( 1.5 );
        }            
        if( copoC[f] == 1 && crspoC[f] == 0 )
        {
            gCopo13->SetPoint( nc1, meancopo[f], 1. );
            gCrspo02->SetPoint( nc1, meancrspo[f], 1. );

            gCrspo02->SetPointError( nc1, rmscrspo[f], ye[f] );
            gCopo13->SetPointError( nc1++, rmscopo[f], ye[f] );

            gCopo13->SetMarkerColor( kBlack );
            gCopo13->SetLineColor( kBlack );
            gCopo13->GetHistogram()->SetMinimum( -0.5 );
            gCopo13->GetHistogram()->SetMaximum( 1.5 );

            gCrspo02->SetMarkerColor( kBlack );
            gCrspo02->SetLineColor( kBlack );
            gCrspo02->GetHistogram()->SetMinimum( -0.5 );
            gCrspo02->GetHistogram()->SetMaximum( 1.5 );
        }
    }

    TLegend* leg = new TLegend( 0.258, 0.742, 0.410, 0.993 );
    leg->SetBorderSize( 0 );
    leg->SetFillColor( 0 );

    TLegend* legcr = new TLegend( 0.258, 0.742, 0.410, 0.993 );
    legcr->SetBorderSize( 0 );
    legcr->SetFillColor( 0 );
    legcr->SetFillStyle( 0 );
    legcr->SetEntrySeparation( 0.01 );
    legcr->SetTextSize( 0.04 );
//    gCopo = new TGraphErrors( kNfls, meancopo, y, rmscopo, ye );
    TCanvas* c = new TCanvas( "c", "", 800, 400 );
    c->Divide( 2, 1 );

    c->cd( 1 );
    gCopo02->SetName( "gcopoCC02" );
    gCopo02->Draw( "AP*" );
    gCopo13->Draw( "P*" );

    leg->AddEntry( gCopo02, "Ch 0 & Ch 2 Co. Po.", "lp" );
    leg->AddEntry( gCopo13, "Ch 1 & Ch 3 Co. Po.", "lp" );
    leg->Draw( );

    gCopo02->Write( ); 

    c->cd( 2 );
    gCrspo13->SetName( "gcrspoCC13" );
    gCrspo13->Draw( "AP*" );
    gCrspo13->GetXaxis()->SetLimits( 0.45, 0.85 );

    gCrspo02->SetName( "gcrspoCC02" );
    gCrspo02->Draw( "P*" );

    legcr->AddEntry( gCrspo13, "Ch 1 & Ch 3 Cross Po.", "lp" );
    legcr->AddEntry( gCrspo02, "Ch 0 & Ch 2 Cross Po.", "lp" );
    legcr->Draw( );

    gCrspo13->Write( ); 


    TCanvas* c2 = new TCanvas( "c2", "", 800, 800 );
    c2->cd( 1 );
    gCopo13->SetName( "gcopoCC13" );
    gCopo13->Draw( "AP*" );
    gCopo13->Write( );

//    fout->Write( );
    fout->Close( );
} 

