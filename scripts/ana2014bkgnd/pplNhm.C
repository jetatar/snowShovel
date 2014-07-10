/* Plots for analysis of Nhm variable (Number of FFT bins above half max) */

#include "TFile.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TH1F.h" 


static const Char_t* stnNum     = "11";

static const Char_t* inflnm     = 
    Form("/w1/jtatar/Analysis/Stn%s/pl.stn%s_ampcalib.root", stnNum, stnNum); 

static const Char_t* outdir    = 
    Form("/data/users/jtatar/PaperPlots/Stn%s/", stnNum);

static const UChar_t kNchans = 3;

void pplNhm( void )
{
    gROOT->LoadMacro( "/data/users/jtatar/plotDefaults/plotDefaults.C" );
    smallHistDefaults( );
    gROOT->ForceStyle( );

    TFile* fin = new TFile( inflnm );

    TH1F* hcomp[kNchans];

    TLegend* leg = new TLegend( 0.77, 0.55, 0.9, 0.9 );
    leg->SetBorderSize( 1 );
    leg->SetFillColor( 0 );
    leg->SetTextFont( titleFont );
    leg->SetTextSize( titleSize );


    for( UChar_t ch = 0; ch < kNchans; ch++ )
    {
        hcomp[ch] = dynamic_cast<TH1F*>(fin->Get(Form("hNHighPks%d", ch)) );

        hcomp[ch]->SetLineStyle( ch + 1 );
        hcomp[ch]->SetLineColor( ch + 6 );
        hcomp[ch]->GetXaxis()->SetRangeUser( -0.5, 25.5 );

        leg->AddEntry( hcomp[ch], Form("Ch%d", ch), "l" );
    }

    TCanvas* c = new TCanvas( "c", "", cwid, chit );    
    c->cd( );
    gPad->SetLogy( 1 );

    hcomp[2]->SetTitle( ";N_{HM}" );

    hcomp[2]->Draw( );
    hcomp[0]->Draw( "SAME" );
    hcomp[1]->Draw( "SAME" );
//    hcomp[3]->Draw( "SAME" );

    leg->Draw( );

    TCanvas* c2 = new TCanvas( "c2", "", cwid, chit );
    c2->cd( );

    hcomp[2]->SetTitle( ";N_{HM}" );

    hcomp[2]->Draw( );
//    hcomp[3]->Draw( "SAME" );
    hcomp[0]->Draw( "SAME" );
    hcomp[1]->Draw( "SAME" );

    leg->Draw( );

    c->Print( Form ("%sAmpCalib_Log_Zoom_NhmNoCuts_SuperImposed.pdf", outdir) );
    c2->Print( Form ("%sAmpCalib_Zoom_NhmNoCuts_SuperImposed.pdf", outdir) );
} 
