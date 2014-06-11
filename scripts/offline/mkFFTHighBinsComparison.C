#include "TFile.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TROOT.h"

//#include "/data/users/jtatar/plotDefaults/plotDefaults.C"

static const Char_t* sigflnm    = 
        "/w2/jtatar/Analysis/Templates/plot75signalfft.root";
static const Char_t* bkgdflnm   = 
        "/w2/jtatar/stn3.fftstudies.root";
static const Char_t* outflnm    = 
        "/w2/jtatar/Analysis/Templates/mkffthighbins.root";



void mkFFTHighBinsComparison( void )
{
    gROOT->LoadMacro( "/data/users/jtatar/plotDefaults/plotDefaults.C" );
    smallHistDefaults( );
    gROOT->ForceStyle( );

    TFile* finsig   = new TFile( sigflnm );
    TH1F* hSig      = (TH1F*)finsig->Get( "hnHighBins" );    

    TFile* finbkgd  = new TFile( bkgdflnm );
    TH1F* hBkgd     = (TH1F*)finbkgd->Get( "hNHighPksAllCh" );

    hSig->Scale( 1. / hSig->Integral(0, hSig->GetNbinsX() + 1) );
    hBkgd->Scale( 1. / hBkgd->Integral(0, hBkgd->GetNbinsX() + 1) );

    TCanvas* c = new TCanvas( "c", "", cwid, chit );    
    c->cd( );
//    gPad->SetLogy( 1 );

    hSig->Draw( );
    hBkgd->Draw( "SAME" );
    hBkgd->SetLineColor( kRed );
    hBkgd->SetLineStyle( 2 );
    hSig->SetTitle( ";N_{HM};Entries;" );
    hSig->GetXaxis()->SetRangeUser( 0, 35 );
    hBkgd->GetXaxis()->SetRangeUser( 0, 35 );

    TLegend* leg = new TLegend( 0.258, 0.742, 0.410, 0.993 );
//    TLegend* leg = new TLegend( .65, .8, .9, .9 );
    leg->SetBorderSize( 1 );
    leg->SetFillColor( 0 );
    leg->SetTextFont( titleFont );
    leg->SetTextSize( titleSize );        

    leg->AddEntry( hSig, "#nu Signal", "l" );
    leg->AddEntry( hBkgd, "Background", "l"  ); 
    leg->Draw( "SAME" );
}
