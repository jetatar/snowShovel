#include "TFile.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TROOT.h"

//#include "/data/users/jtatar/plotDefaults/plotDefaults.C"

static const Char_t* sigflnm    = 
        "/w2/jtatar/Analysis/Templates/plot75weightedsignalfft.root";
static const Char_t* bkgdflnm   = 
        "/data/users/jtatar/Work/snowShovel/scripts/offline/fftevtgraph.root";
static const Char_t* outflnm    = 
        "/w2/jtatar/Analysis/Templates/mkfftcomparison.root";



void mkFFTComparison( void )
{
    gROOT->LoadMacro( "/data/users/jtatar/plotDefaults/plotDefaults.C" );
    smallHistDefaults( );
    gROOT->ForceStyle( );

    TFile* finsig   = new TFile( sigflnm );
    TGraph* gSig    = (TGraph*)finsig->Get( "gFFTSignal" );    

    TFile* finbkgd  = new TFile( bkgdflnm );
    TGraph* gBkgd   = (TGraph*)finbkgd->Get( "gFFTBkgd" );

    UInt_t npts = gBkgd->GetN( );
    
    Double_t* y = gBkgd->GetY( );
    Double_t* x = gBkgd->GetX( );

    for( UInt_t i = 0; i < npts; i++ )
    {
        y[i] = y[i] * 1.0312e3 / 8.90e2;
        gBkgd->SetPoint( i, x[i], y[i] );
    }

    TCanvas* c = new TCanvas( "c", "", cwid, chit );    
    c->cd( );

    gSig->Draw( "APL" );
    gBkgd->Draw( "PL" );
    gBkgd->SetLineColor( kRed );
    gBkgd->SetLineStyle( 2 );
    gSig->SetTitle( ";Frequency (GHz);C_{F} (Volts);" );

    TLegend* leg = new TLegend( 0.258, 0.742, 0.410, 0.993 );
//    TLegend* leg = new TLegend( .65, .8, .9, .9 );
    leg->SetBorderSize( 1 );
    leg->SetFillColor( 0 );
    leg->SetTextFont( titleFont );
    leg->SetTextSize( titleSize );        

    leg->AddEntry( gSig, "#nu Signal", "l" );
    leg->AddEntry( gBkgd, "Background", "l"  ); 
    leg->Draw( "SAME" );
}
