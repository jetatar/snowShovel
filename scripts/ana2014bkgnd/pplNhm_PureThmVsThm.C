/* 
    N_{HM} is plotted for pure thermal events (CC < 0.45) and thermal events with large CC (>= 0.45) due to antropogenic noise.
*/

#include "TFile.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TH1F.h" 


static const Char_t* stnNum     = "3";
static const UChar_t kNchans    = 4;

static const Char_t* inlrgflnm  = 
    Form("/w1/jtatar/Analysis/Stn%s/pl.stn%s_NhmLrgCC.root", 
                                                            stnNum, stnNum); 
static const Char_t* insmlflnm  = 
    Form("/w1/jtatar/Analysis/Stn%s/pl.stn%s_NhmSmallCC.root", 
                                                            stnNum, stnNum); 
static const Char_t* inminbflnm =
    Form("/w1/jtatar/Analysis/Stn%s/pl.stn%s_ampcalib_minbias.root",
                                                            stnNum, stnNum);
static const Char_t* innoiseflnm = 
    "/w1/jtatar/Analysis/Stn0/pl.stn0_NhmwithStn11CovMatrix.root";

TString outdir    = Form("/data/users/jtatar/PaperPlots/Stn%s/", stnNum);


void pplNhm_PureThmVsThm( void )
{
    gROOT->LoadMacro( "/data/users/jtatar/plotDefaults/plotDefaults.C" );
    smallHistDefaults( );
    gROOT->ForceStyle( );

    TFile* finlrg   = new TFile( inlrgflnm );
    TFile* finsml   = new TFile( insmlflnm );
    TFile* finminb  = new TFile( inminbflnm );
    TFile* finnoise = new TFile( innoiseflnm );

    TH1F*       hlrg[kNchans];
    TH1F*       hsml[kNchans];
    TH1F*       hminb[kNchans];
    TH1F*       hnoise[kNchans];

    TCanvas*    c[kNchans];
    TCanvas*    c2[kNchans];
    TLegend*    leg[kNchans];


    for( UChar_t ch = 0; ch < 1; ch++ )
    {
        hlrg[ch]    = dynamic_cast<TH1F*>
                                    (finlrg->Get("hNHighPksAllCh") );
        hsml[ch]    = dynamic_cast<TH1F*>
                                    (finsml->Get("hNHighPksAllCh") );
        hminb[ch]   = dynamic_cast<TH1F*>
                                    (finminb->Get("hNHighPksAllCh") );
        hnoise[ch]  = dynamic_cast<TH1F*>
                                    (finnoise->Get("hNHighPksAllCh") );

        hlrg[ch]->Scale( 1. / 
                        hlrg[ch]->Integral(0, hlrg[ch]->GetNbinsX() + 1) );
        hsml[ch]->Scale( 1. / 
                        hsml[ch]->Integral(0, hsml[ch]->GetNbinsX() + 1) );
        hminb[ch]->Scale( 1./ 
                        hminb[ch]->Integral(0, hminb[ch]->GetNbinsX() + 1) );
        hnoise[ch]->Scale( 1./ 
                        hnoise[ch]->Integral(0, hnoise[ch]->GetNbinsX() + 1) );

        leg[ch] = new TLegend( 0.57, 0.75, 0.95, 0.95 );

        leg[ch]->AddEntry( hlrg[ch], "Thm CC #geq 0.45", "l" );
        leg[ch]->AddEntry( hsml[ch], "Thm CC < 0.45", "l" );
        leg[ch]->AddEntry( hminb[ch], "MinB All", "l" );
        leg[ch]->AddEntry( hnoise[ch], "Sim. Noise", "l" );

        leg[ch]->SetBorderSize( 1 );
        leg[ch]->SetFillColor( 0 );
        leg[ch]->SetTextFont( titleFont );
        leg[ch]->SetTextSize( titleSize );

        c[ch]   = new TCanvas( Form("c%d", ch), "", cwid, chit );
        c2[ch]  = new TCanvas( Form("c2_%d", ch), "", cwid, chit );

        c[ch]->cd( ); 
        hlrg[ch]->Draw( );
        hsml[ch]->Draw( "SAME" );
        hminb[ch]->Draw( "SAME" );
        hnoise[ch]->Draw( "SAME" );
        leg[ch]->Draw( );
        
        c2[ch]->cd( );
        gPad->SetLogy( 1 );
        hlrg[ch]->Draw( );
        hsml[ch]->Draw( "SAME" );
        hminb[ch]->Draw( "SAME" );
        hnoise[ch]->Draw( "SAME" );
        leg[ch]->Draw( );

        hsml[ch]->SetTitle( ";N_{HM};Normalized Num. Events" );
        hsml[ch]->SetLineStyle( 1 );
        hsml[ch]->GetXaxis()->SetRangeUser( -0.5, 27.5 );

        hlrg[ch]->SetLineStyle( 2 );
        hlrg[ch]->SetTitle( ";N_{HM};Normalized Num. Events" );
        hlrg[ch]->GetXaxis()->SetRangeUser( -0.5, 27.5 );

        hminb[ch]->SetLineStyle( 3 );
        hminb[ch]->SetTitle( ";N_{HM};Normalized Num. Events" );
        hminb[ch]->GetXaxis()->SetRangeUser( -0.5, 27.5 );

        hnoise[ch]->SetLineStyle( 4 );
        hnoise[ch]->SetTitle( ";N_{HM};Normalized Num. Events" );
        hnoise[ch]->GetXaxis()->SetRangeUser( -0.5, 27.5 );

        c[ch]->Print( Form("%sNhmPureThmVsThmWithStn11CovMatrix.pdf", 
                                                            outdir.Data()) );

        c2[ch]->Print( Form("%sLogNhmPureThmVsThmWithStn11CovMatrix.pdf", 
                                                            outdir.Data()) );
    }
} 
