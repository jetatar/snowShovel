#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TChain.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TLine.h>
#include <TGraph.h>
#include <TObjArray.h>
#include <TSpline.h>
#include <TH1.h>
#include <TStyle.h>
#include <TFile.h>
#include <TMath.h>
#include <TBox.h>
#include <TLatex.h>

#include "TSnPlotBrowser.h"
#include "TSnEventHeader.h"
#include "TSnEventMetadata.h"
#include "TSnCalWvData.h"
#include "NSnConstants.h"
#include "TSnStopPosnSet.h"
#endif

TChain* st;
TCanvas* c1, * c2, * c3;

TObjArray* hStops;
TObjArray* plots;

const Int_t chcols[NSnConstants::kNchans] = 
   { kOrange+7, kAzure-2, kViolet-1, kGreen+2 };
const Int_t chmrks[NSnConstants::kNchans] = {7,7,7,7};
const Int_t scol = kRed+1;
const Int_t sfil = 3003;

const Int_t cwid=500, chit=400;

const Float_t dmin = -350, dmax=350;

const Float_t syscale = 1.1 / 8.0; // stop dist y max = (entries / 8 slots)*1.1

void drawStopShifts(const Char_t* infn,
                    const Char_t* treen="StopShift",
                    const Char_t* datbr="AmpOutData.",
                    const Char_t* shfbr="ShiftedAmpOutData.") {
   
   gStyle->SetOptStat(0);
   
   st = new TChain(treen);
   st->Add(infn);
   
   TSnEventMetadata* evm = new TSnEventMetadata;
   TSnEventHeader*   hdr = new TSnEventHeader;
   TSnCalWvData*     dat = new TSnCalWvData;
   TSnStopPosnSet*   sps = new TSnStopPosnSet;
   TSnCalWvData*     shf = new TSnCalWvData;
   
   st->SetBranchAddress("EventMetadata.", &evm);
   st->SetBranchAddress("EventHeader.", &hdr);
   st->SetBranchAddress(datbr, &dat);
   st->SetBranchAddress("StopPositions.", &sps);
   st->SetBranchAddress(shfbr, &shf);

   const Long64_t n = st->GetEntries();
   
   TH1* h;
   TString hn;
   hStops = new TObjArray(NSnConstants::kNchans);
   hStops->SetOwner(kTRUE);
   
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      hn = Form("hStopPos_ch%hhu",ch);
      h = new TH1F(hn.Data(), ";first stop stample",
                   NSnConstants::kNsamps, -0.5, NSnConstants::kNsamps-0.5);
      h->SetMaximum(n * syscale);
      h->SetLineColor(chcols[ch]);
      h->SetMarkerColor(chcols[ch]);
      h->SetMarkerStyle(chmrks[ch]);
      hStops->AddAt(h, ch);
   }
   
   for (Long64_t i=0; i<n; ++i) {
      st->GetEntry(i);
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         h = static_cast<TH1*>(hStops->At(ch));
         h->Fill( sps->GetFirstSample(ch) );
      }
   }

   c1 = new TCanvas("c1","c1 - stop posn",cwid*1.5,chit*1.5);
   c2 = new TCanvas("c2","c2 - stop posn overlay",cwid,chit);
   UInt_t cols,rows;
   TSnPlotBrowser::CalcCanvDivs(cols, rows, NSnConstants::kNchans);
   c1->Divide(cols,rows);
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      h = static_cast<TH1*>(hStops->At(ch));
      c1->cd(ch+1);
      h->Draw();
      c2->cd();
      h->Draw( (ch==0) ? "" : "same" );
   }
   c1->cd();
   c1->Update();
   c2->cd();
   c2->Update();
   
   plots = new TObjArray;
   plots->SetOwner(kTRUE);
   
   c3 = new TCanvas("c3","c3 - waveforms",cwid*2,chit*2);
   for (Long64_t i=0; i<n; ++i) {
      st->GetEntry(i);
      
      TSnPlotBrowser* pb = new TSnPlotBrowser("pb");
      pb->SetCanvas(c3);
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         // unshifted
         hn = Form("hUnshiftedAxis_ch%hhu",ch);
         h = new TH1F(hn.Data(),";sample;amplitude (mV)",
                      NSnConstants::kNsamps, -0.5, NSnConstants::kNsamps-0.5);
         h->SetMinimum(dmin);
         h->SetMaximum(dmax);
         plots->Add(h);
         pb->AddPlotToSet("Unshifted", h, "axis", ch+1);
         TGraph* g = dat->NewGraphForChan(ch);
         g->SetLineColor(chcols[ch]);
         g->SetMarkerColor(chcols[ch]);
         g->SetMarkerStyle(chmrks[ch]);
         plots->Add(g);
         pb->AddPlotToSet("Unshifted", g, "pl", ch+1);
         TBox* b = new TBox(sps->GetFirstSample(ch)-0.5, dmin,
                            sps->GetWaveBeginSample(ch)-0.5, dmax);
         b->SetLineColor(scol);
         b->SetFillColor(scol);
         b->SetFillStyle(sfil);
         pb->AddPlotToSet("Unshifted", b, "", ch+1);
         TLatex* l = new TLatex(0.5,0.93,Form("Stop @ %d",
                  static_cast<Int_t>(sps->GetFirstSample(ch))));
         l->SetNDC();
         l->SetTextColor(scol);
         plots->Add(l);
         pb->AddPlotToSet("Unshifted", l, "", ch+1);
         
         // shifted
         hn = Form("hShiftedAxis_ch%hhu",ch);
         h = new TH1F(hn.Data(),";sample;amplitude (mV)",
                      NSnConstants::kNsamps, -0.5, NSnConstants::kNsamps-0.5);
         h->SetMinimum(dmin);
         h->SetMaximum(dmax);
         plots->Add(h);
         pb->AddPlotToSet("Shifted", h, "axis", ch+1);
         TGraph* gs = shf->NewGraphForChan(ch);
         gs->SetLineColor(chcols[ch]);
         gs->SetMarkerColor(chcols[ch]);
         gs->SetMarkerStyle(chmrks[ch]);
         plots->Add(gs);
         pb->AddPlotToSet("Shifted", gs, "pl", ch+1);
         
      }

      pb->DrawButtonBar();
      pb->DrawPlots("Unshifted");
      TObject* o(0);
      while ( (o = c3->WaitPrimitive())!=0 ) {
         gSystem->ProcessEvents();
      }
         
      delete pb;
      plots->Clear();
      
   }
   
}


