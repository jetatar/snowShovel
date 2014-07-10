/* Plots for analysis of Nhm variable (Number of FFT bins above half max) */

#include "TFile.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TH1F.h" 


static const Char_t* stnNum     = "11";

static const Char_t* inthmflnm      = 
    Form("/w1/jtatar/Analysis/Stn%s/pl.stn%s.root", stnNum, stnNum); 
static const Char_t* inminbflnm     = 
    Form("/w1/jtatar/Analysis/Stn%s/pl.stn%s_minbias.root", stnNum, stnNum); 

TString outdir    = 
    Form("/data/users/jtatar/PaperPlots/Stn%s/", stnNum);

static const UChar_t kNchans = 3;

void pplNhm_ThmVsMinb( void )
{
    gROOT->LoadMacro( "/data/users/jtatar/plotDefaults/plotDefaults.C" );
    smallHistDefaults( );
    gROOT->ForceStyle( );

    TFile* finthm   = new TFile( inthmflnm );
    TFile* finminb  = new TFile( inminbflnm );

    TH1F*       hthm[kNchans];
    TH1F*       hminb[kNchans];
    TCanvas*    c[kNchans];
    TCanvas*    c2[kNchans];
    TLegend*    leg[kNchans];

    for( UChar_t ch = 0; ch < kNchans; ch++ )
    {
        hthm[ch]    = dynamic_cast<TH1F*>
                                    (finthm->Get(Form("hNHighPks%d", ch)) );
        hminb[ch]   = dynamic_cast<TH1F*>
                                    (finminb->Get(Form("hNHighPks%d", ch)) );

        hthm[ch]->Scale( 1. / 
                        hthm[ch]->Integral(0, hthm[ch]->GetNbinsX() + 1) );
        hminb[ch]->Scale( 1. / 
                        hminb[ch]->Integral(0, hminb[ch]->GetNbinsX() + 1) );

        leg[ch] = new TLegend( 0.6, 0.75, 0.92, 0.95 );

        leg[ch]->AddEntry( hthm[ch], Form("Ch%d Thermal", ch), "l" );
        leg[ch]->AddEntry( hminb[ch], Form("Ch%d Minbias", ch), "l" );

        leg[ch]->SetBorderSize( 1 );
        leg[ch]->SetFillColor( 0 );
        leg[ch]->SetTextFont( titleFont );
        leg[ch]->SetTextSize( titleSize );

        c[ch]   = new TCanvas( Form("c%d", ch), "", cwid, chit );
        c2[ch]  = new TCanvas( Form("c2_%d", ch), "", cwid, chit );

        c[ch]->cd( ); 
        hminb[ch]->Draw( );
        hthm[ch]->Draw( "SAME" );
        leg[ch]->Draw( );
        
        c2[ch]->cd( );
        gPad->SetLogy( 1 );
        hthm[ch]->Draw( );
        hminb[ch]->Draw( "SAME" );
        leg[ch]->Draw( );

        hthm[ch]->SetTitle( ";N_{HM};Normalized Num. Events" );
        hthm[ch]->SetLineStyle( 1 );
        hthm[ch]->GetXaxis()->SetRangeUser( -0.5, 25.5 );

        hminb[ch]->SetLineStyle( 2 );
        hminb[ch]->GetXaxis()->SetRangeUser( -0.5, 25.5 );

        c[ch]->Print( Form("%sch%d.Zoom_NhmNoCuts_ThmVsMinb.pdf", 
                                                        outdir.Data(), ch) );
        c2[ch]->Print( Form("%sch%d.Log_Zoom_NhmNoCuts_ThmVsMinb.pdf", 
                                                        outdir.Data(), ch) );
    }
} 
