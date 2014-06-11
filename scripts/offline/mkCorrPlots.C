#if !defined(__CINT__) || defined(__MAKECINT__)

#include "TPad.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TFile.h"
#include "TROOT.h"
#include "TSystem.h"

#include "NSnConstants.h"

#endif


//static const Char_t* infn   = "gr.ici3.cc.thm.root";
//static const Char_t* outfn  = "pl.ici3.cc.thm.root";

TH2F* hCC, *hpcNS, *hpcEW, *hCCAvg;
TH2F* hPrCC[NSnConstants::kNchans];

void mkCorrPlots( const Char_t* infn, const Char_t* outtag )
{
    gSystem->Setenv( "TZ", "UTC" );
    gStyle->SetTimeOffset( 0 );
    gStyle->SetOptStat( 0 );

    TFile* ifl = TFile::Open( infn );

    hCC     = dynamic_cast<TH2F*>( ifl->Get("hCC") );
    hpcNS   = dynamic_cast<TH2F*>( ifl->Get("hpcNS") );
    hpcEW   = dynamic_cast<TH2F*>( ifl->Get("hpcEW") );
    hCCAvg  = dynamic_cast<TH2F*>( ifl->Get("hCCAvg") );

    for( UShort_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        hPrCC[ch] = dynamic_cast<TH2F*>( ifl->Get(Form("hCCvT%i", ch)) );
    }

    TCanvas* c = new TCanvas( "c", "", 800, 400 );
    c->Divide( 2, 1 );
    
    c->cd( 1 );
    c->GetPad(1)->SetLogz( );
    c->GetPad(1)->SetGridx( );
    c->GetPad(1)->SetGridy( );

    hpcNS->Draw( "colz" );
    hpcNS->SetTitle( ";North Ch #Chi;South Ch #Chi" );

    c->cd( 2 );
    c->GetPad(2)->SetLogz( );
    c->GetPad(2)->SetGridx( );
    c->GetPad(2)->SetGridy( );

    hpcEW->Draw( "colz" );
    hpcEW->SetTitle( ";East Ch #Chi;West Ch #Chi" );

    
    TCanvas* c2 = new TCanvas( "c2", "", 800, 800 );
    c2->cd( );
    gPad->SetLogz( );
    gPad->SetLogz( );
    gPad->SetGridx( );
    gPad->SetGridy( );

    hCCAvg->Draw( "colz" );
    hCCAvg->SetTitle( ";Avg #Chi_{NS}; Avg #Chi_{EW}" );

    
    TCanvas* c3 = new TCanvas( "c3", "", 800, 800 );
    c3->Divide( 2, 2 );

    for( UShort_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        c3->cd( ch + 1 );
        gPad->SetLogz( );
        gPad->SetGridy( );        

        hPrCC[ch]->Draw( "colz" );
        hPrCC[ch]->SetTitle( Form(";Time (UTC);Ch%i #Chi", ch) );
        
        hPrCC[ch]->GetXaxis()->SetTimeFormat( "%m/%d" );
        hPrCC[ch]->GetXaxis()->SetTimeDisplay( 1 );
    }

    
    c->Print( Form("%s_%s.gif", outtag, c->GetName()) );
    c2->Print( Form("%s_%s.gif", outtag, c2->GetName()) );
    c3->Print( Form("%s_%s.gif", outtag, c3->GetName()) );
}

