/* Plots for analysis of Nhm variable (Number of FFT bins above half max) */

#include "TFile.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TH1F.h" 


static const Char_t* stnNum     = "11";
static const UChar_t kNchans    = 3;

static const Char_t* inflnm     = 
    Form("/w1/jtatar/Analysis/Stn%s/pl.stn%s_prunebadevts.root", stnNum, stnNum); 

TString outdir = Form("/data/users/jtatar/PaperPlots/Stn%s/", stnNum);


void pplRejEarlyEvts( void )
{
    gROOT->LoadMacro( "/data/users/jtatar/plotDefaults/plotDefaults.C" );
    smallHistDefaults( );
    gROOT->ForceStyle( );

    TFile* fin = new TFile( inflnm );

    // Distribution of all samples of first events in a sequence.
    TH1F* hdT;
    TH1F* hSamps[kNchans];
    TH2F* hSampsVsDt[kNchans];

    TCanvas* cdT;
    TCanvas* cSamps[kNchans];
    TCanvas* cSampsVsDt[kNchans];
/*
    TLegend* leg = new TLegend( 0.77, 0.55, 0.9, 0.9 );
    leg->SetBorderSize( 1 );
    leg->SetFillColor( 0 );
    leg->SetTextFont( titleFont );
    leg->SetTextSize( titleSize );
*/

    hdT = dynamic_cast<TH1F*>( fin->Get("hdT") );
    cdT = new TCanvas( "cdT", "", cwid, chit );    

    hdT->GetXaxis()->SetRangeUser( -0.5, 300.5 );
    hdT->SetTitle( ";dT (s);Num. of First Events" );

    cdT->cd( );

    gPad->SetLogy( 1 );
    hdT->Draw( );
    cdT->Print( Form("%sdTFirstEvt.pdf", outdir.Data()) );

    for( UChar_t ch = 0; ch < kNchans; ch++ )
    {
        hSamps[ch] = dynamic_cast<TH1F*>( fin->Get(Form("hAdc%d", ch)) );
        cSamps[ch] = new TCanvas( Form("cSampsCh%d", ch), "", cwid, chit );    

        hSampsVsDt[ch] = dynamic_cast<TH2F*>(fin->Get(Form("hdTvsAdc%d", ch)));
        cSampsVsDt[ch] =
                new TCanvas( Form("cSampsVsDtCh%d", ch), "", cwid, chit );
/*
        hSamps[ch]->SetLineStyle( ch + 1 );
        hSamps[ch]->SetLineColor( ch + 6 );
*/
        hSamps[ch]->GetXaxis()->SetRangeUser( -2600, 2600 );
        hSamps[ch]->
            SetTitle( Form(";Ch %d Sample Value (mV);Number of Samples", ch) );

        hSampsVsDt[ch]->GetXaxis()->SetRangeUser( -9.5, 300.5 );
        hSampsVsDt[ch]->GetYaxis()->SetRangeUser( -2600, 2600 );
        hSampsVsDt[ch]->
                    SetTitle( Form(";dT (s); Ch %d Sample Value (mV)", ch) );


        if( ch == 0 )
        {
            cSamps[ch]->cd( );
            gPad->SetLogy( 1 );
            hSamps[ch]->Draw( );

            cSampsVsDt[ch]->cd( );
            gPad->SetLogz( 1 );
            hSampsVsDt[ch]->Draw( "colz" ); 
        }
        else
        {

            cSamps[ch]->cd( );
            gPad->SetLogy( 1 );
            hSamps[ch]->Draw( "SAME" ); 

            cSampsVsDt[ch]->cd( );
            gPad->SetLogz( 1 );
            hSampsVsDt[ch]->Draw( "colz SAME" ); 
        }

//        leg->AddEntry( hSamps[ch], Form("Ch%d", ch), "l" );

        cSamps[ch]->
            Print( Form ("%sCh%dFirstEvtSamps.pdf", outdir.Data(), ch) );
        cSampsVsDt[ch]->
            Print( Form ("%sCh%dFirstEvtSampsVsDt.pdf", outdir.Data(), ch) );
    }
} 

