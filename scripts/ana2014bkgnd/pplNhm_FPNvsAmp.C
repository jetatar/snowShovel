/* 
    Plots for analysis of Nhm variable (Number of FFT bins above half max).
    Here we compare the difference between Nhm variable when we use FPNSubData
    vs AmpOutData (FPN and Amplifier gain calibrated out).
 */

#include "TFile.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TH1F.h" 


static const Char_t* stnNum     = "10";
static const UChar_t kNchans    = 4;

static const Char_t* infpnflnm  = 
    Form("/w1/jtatar/Analysis/Stn%s/pl.stn%s.root", 
                                                            stnNum, stnNum); 
static const Char_t* inampflnm  = 
    Form("/w1/jtatar/Analysis/Stn%s/pl.stn%s_ampcalib.root", 
                                                            stnNum, stnNum); 

TString outdir    = Form("/data/users/jtatar/PaperPlots/Stn%s/", stnNum);


void pplNhm_FPNvsAmp( void )
{
    gROOT->LoadMacro( "/data/users/jtatar/plotDefaults/plotDefaults.C" );
    smallHistDefaults( );
    gROOT->ForceStyle( );

    TFile* finfpn = new TFile( infpnflnm );
    TFile* finamp = new TFile( inampflnm );

    TH1F*       hfpn[kNchans];
    TH1F*       hamp[kNchans];
    TCanvas*    c[kNchans];
    TCanvas*    c2[kNchans];
    TLegend*    leg[kNchans];

    for( UChar_t ch = 0; ch < kNchans; ch++ )
    {
        hfpn[ch]    = dynamic_cast<TH1F*>
                                    (finfpn->Get(Form("hNHighPks%d", ch)) );
        hamp[ch]   = dynamic_cast<TH1F*>
                                    (finamp->Get(Form("hNHighPks%d", ch)) );

        hfpn[ch]->Scale( 1. / 
                        hfpn[ch]->Integral(0, hfpn[ch]->GetNbinsX() + 1) );
        hamp[ch]->Scale( 1. / 
                        hamp[ch]->Integral(0, hamp[ch]->GetNbinsX() + 1) );

        leg[ch] = new TLegend( 0.6, 0.75, 0.95, 0.95 );

        leg[ch]->AddEntry( hfpn[ch], Form("Ch%d Thm FPN", ch), "l" );
        leg[ch]->AddEntry( hamp[ch], Form("Ch%d Thm Amp", ch), "l" );

        leg[ch]->SetBorderSize( 1 );
        leg[ch]->SetFillColor( 0 );
        leg[ch]->SetTextFont( titleFont );
        leg[ch]->SetTextSize( titleSize );

        c[ch]   = new TCanvas( Form("c%d", ch), "", cwid, chit );
        c2[ch]  = new TCanvas( Form("c2_%d", ch), "", cwid, chit );

        c[ch]->cd( ); 
        hamp[ch]->Draw( );
        hfpn[ch]->Draw( "SAME" );
        leg[ch]->Draw( );
        
        c2[ch]->cd( );
        gPad->SetLogy( 1 );
        hfpn[ch]->Draw( );
        hamp[ch]->Draw( "SAME" );
        leg[ch]->Draw( );

        hfpn[ch]->SetTitle( ";N_{HM};Normalized Num. Events" );
        hfpn[ch]->SetLineStyle( 1 );
        hfpn[ch]->GetXaxis()->SetRangeUser( -0.5, 25.5 );

        hamp[ch]->SetLineStyle( 2 );
        hamp[ch]->GetXaxis()->SetRangeUser( -0.5, 25.5 );

        c[ch]->Print( Form("%sch%d.Thm_Zoom_NhmNoCuts_FpnVsAmp.pdf", 
                                                        outdir.Data(), ch) );
        c2[ch]->Print( Form("%sch%d.Thm_Log_Zoom_NhmNoCuts_FpnVsAmp.pdf", 
                                                        outdir.Data(), ch) );
    }
} 
