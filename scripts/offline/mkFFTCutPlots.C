#if !defined(__CINT__) || defined(__MAKECINT__)

#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TStyle.h"

#endif


static const Char_t* infn   = "nusignonoise.root";
static const Char_t* outfn  = "pl.nusignonoisevsang.root";

const UInt_t cols[8] = { kRed, kMagenta + 1, kViolet + 7, kCyan + 2, 
                         kSpring - 1, kOrange - 2, kBlack, kGray };


void mkFFTCutPlots( void )
{
    gROOT->SetBatch( kTRUE );
    gStyle->SetOptStat( 0 );

    TFile* ifl = TFile::Open( infn );
    
    TFile* fout = TFile::Open( outfn, "recreate" );
    fout->cd( );

    TH1F* hEAng[8];
    TH1F* hHAng[7];

    for( UChar_t n = 0; n < 8; n++ )
    {
        hEAng[n] = dynamic_cast<TH1F*>
                            ( ifl->Get(Form("hNHighPksVsEAng%d", n*10)) );
    }

    for( UChar_t n = 0; n < 7; n++ )
    {
        hHAng[n] = dynamic_cast<TH1F*>
                            ( ifl->Get(Form("hNHighPksVsHAng%d", n*10)) );

        if( hHAng[n] == 0 )
        {
            Printf( "Couldn't find object: %s", 
                                            Form("hNHighPksVsHAng%d", n*10) );
        }
    }

    TCanvas* c = new TCanvas( "c", "", 800, 800 );

    c->cd( );
    gPad->SetLogy( 1 );

    TLegend* leg = new TLegend( 0.258, 0.742, 0.410, 0.993 );
    leg->SetBorderSize( 0 );
    leg->SetFillColor( 0 );

    for( UChar_t n = 0; n < 8; n++ )
    {
        if( n != 0 )
        {
            hEAng[n]->Draw( "SAME" );
        }
        else
        {
            hEAng[n]->Draw( );
        }

        hEAng[n]->SetLineColor( cols[n] );

        leg->AddEntry( hEAng[n], Form("%d deg wrt E-plane", n*10) );
    }

    leg->Draw( );

    TCanvas* c2 = new TCanvas( "c2", "", 800, 800 );
    c2->cd( );
    gPad->SetLogy( 1 );

    TLegend* leg2 = new TLegend( 0.258, 0.742, 0.410, 0.993 );
    leg2->SetBorderSize( 0 );
    leg2->SetFillColor( 0 );

    for( UChar_t n = 0; n < 7; n++ )
    {
        if( n != 0 )
        {
            hHAng[n]->Draw( "SAME" );
        }
        else
        {
            hHAng[n]->Draw( );
        }

        hHAng[n]->SetLineColor( cols[n] );

        leg2->AddEntry( hHAng[n], Form("%d deg wrt H-plane", n*10) );
    }

    leg2->Draw( );
    
    c->Write( );
    c2->Write( );

    fout->Write( );
    fout->Close( );

    ifl->Close( );
    delete ifl;
}
