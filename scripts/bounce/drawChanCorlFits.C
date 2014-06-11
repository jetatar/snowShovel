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
#include <TSpectrum.h>

#include "TSnPlotBrowser.h"
#include "TSnEventHeader.h"
#include "TSnEventMetadata.h"
#include "TSnCalWvData.h"
#include "NSnConstants.h"
#include "TSnRecoChanOffsets.h"
#include "TSnInterp1DSpline3.h"
#include "TSnChanTimeCorlMod.h"
#include "TSnRecoSingleChanOffset.h"
#include "TSnRecoSingleChanOffsetSet.h"
#include "TSnChanTimeCorlAltEvtsMod.h"
#endif

TChain* datt, * offt;
TCanvas* c1, * c2;
TObjArray* hOffsets;
TFile* outf;

const Int_t chcols[NSnConstants::kNchans] = 
   { kOrange+7, kAzure-2, kViolet-1, kGreen+2 };
const Int_t chmrks[NSnConstants::kNchans] = {7,7,7,7};
const Int_t dtcol = kRed;
const Int_t dtwid = 2;
const Int_t dtsty = 7;
const Int_t shcol = kBlack;

const Float_t datmin = -350, datmax = 350;
const Float_t crlmin = -1.1, crlmax = 1.1;

const Int_t   cdtbins = 100;
const Float_t cdtmin = -5.0, cdtmax = 5.0;

// data tree branches
TSnEventMetadata*    dmd(0);
TSnEventHeader*      dhd(0);
TSnCalWvData*        dat(0);
TSnInterp1DSpline3** dfl(0);
TSnEventMetadata*    amd(0);
TSnEventHeader*      ahd(0);
TSnCalWvData*        adt(0);
// offset tree branches
TSnEventMetadata*           fmd(0);
TSnEventHeader*             fhd(0);
TSnRecoChanOffsets*         fco(0);
TSnRecoSingleChanOffsetSet* fso(0);

void setupDataTree(const Char_t* infn,
                   const Char_t* treenm,
                   const Char_t* datbr="FilteredData.",
                   const Char_t* alttag=NULL) {
   datt = new TChain(treenm);
   datt->Add(infn);
   
   dmd = new TSnEventMetadata;
   dhd = new TSnEventHeader;
   dat = new TSnCalWvData;
   
   datt->SetBranchAddress("EventMetadata.", &dmd);
   datt->SetBranchAddress("EventHeader.", &dhd);
   datt->SetBranchAddress(datbr, &dat);

   if (alttag!=NULL) {
      amd = new TSnEventMetadata;
      ahd = new TSnEventHeader;
      adt = new TSnCalWvData;
      datt->SetBranchAddress(Form("%sEventMetadata.",alttag), &amd);
      datt->SetBranchAddress(Form("%sEventHeader.",alttag), &ahd);
      datt->SetBranchAddress(Form("%s%s",alttag,datbr), &adt);
   }
   
   TString bn;
   dfl = new TSnInterp1DSpline3*[TSnRecoChanOffsets::kChCmbs];
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      if (alttag==NULL) {
         for (UChar_t xc=0; xc<ch; ++xc) {
            bn = TSnChanTimeCorlMod::GetChanCorlInterpName(ch, xc);
            const Int_t di = TSnRecoChanOffsets::IndexFor(ch, xc);
            dfl[di] = new TSnInterp1DSpline3;
            datt->SetBranchAddress(bn.Data(), &(dfl[di]));
         }
      } else {
         bn = TSnChanTimeCorlAltEvtsMod::GetChanCorlInterpName(ch);
         dfl[ch] = new TSnInterp1DSpline3;
         datt->SetBranchAddress(bn.Data(), &(dfl[ch]));
      }
   }
   
}

void setupOffsetTree(const Char_t* infn,
                     const Char_t* treenm,
                     const Char_t* alttag=NULL) {

   offt = new TChain(treenm);
   offt->Add(infn);
   
   fmd = new TSnEventMetadata;
   fhd = new TSnEventHeader;

   offt->SetBranchAddress("EventMetadata.", &fmd);
   offt->SetBranchAddress("EventHeader.", &fhd);
   if (alttag==NULL) {
      fco = new TSnRecoChanOffsets;
      offt->SetBranchAddress("ChanTimeOffsets.", &fco);
   } else {
      fso = new TSnRecoSingleChanOffsetSet;
      offt->SetBranchAddress("SingleChanTimeOffsets.", &fso);
   }

   offt->BuildIndex("EventMetadata.fRun", "EventHeader.fNum");
   
}

TSnRecoChanOffsets* fillCrossChanCorlHists() {
   
   if ( (datt==0) || (offt==0) ) {
      Error("fillCrossChanCorlHists","Trees not yet set up!");
      return 0;
   }
   
   if (hOffsets==0) {
      hOffsets = new TObjArray(TSnRecoChanOffsets::kChCmbs);
      hOffsets->SetOwner(kTRUE);
   }
   
   const Long64_t n = datt->GetEntries();
   TString hn;
   TH1* h;
   Double_t err(0);
   for (Long64_t i=0; i<n; ++i) {
      
      datt->GetEntry(i);
      const Long64_t oi = offt->GetEntryNumberWithIndex(dmd->GetRunNum(),
                                                        dhd->GetEvtNum());
   
      if (oi>-1) {
         offt->GetEntry(oi);
         for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            for (UChar_t xc=0; xc<ch; ++xc) {
               const UInt_t di = TSnRecoChanOffsets::IndexFor(ch,xc);
               if (hOffsets->At(di)==0) {
                  // make the hist
                  hn = Form("hOffsets_%hhu_%hhu",ch,xc);
                  h = new TH1F(hn.Data(),
                               Form("offsets ch%hhu - ch%hhu; "
                                    "#Deltat ch%hhu-ch%hhu (ns)",ch,xc,ch,xc),
                               cdtbins, cdtmin, cdtmax);
                  hOffsets->AddAt(h, di);
               }
               h = dynamic_cast<TH1*>(hOffsets->At(di));
               err = fco->GetChanOffsetError(ch,xc);
               h->Fill(fco->GetChanOffset(ch,xc), (err>0) ? err : 1.0);
                       
            } // xc
         } // ch
      } // oi
   } // i
   
   TSnRecoChanOffsets* ro = new TSnRecoChanOffsets(*fco);
   ro->SetFitQual(0);
   TString on(ro->GetName());
   if (on.EndsWith(".")) {
      on = on(0, on.Length()-1);
      ro->SetName( on.Data() );
   }
   
   TSpectrum a(10);
   for (UInt_t i=0; i<TSnRecoChanOffsets::kChCmbs; ++i) {
      h = dynamic_cast<TH1*>(hOffsets->At(i));
      a.Search(h, 2, "nodraw");
      const Float_t* x = a.GetPositionX();
      const Float_t* y = a.GetPositionY();
      Float_t maxx(0), maxy(0);
      const Int_t np = a.GetNPeaks();
      for (Int_t p=0; p<np; ++p, ++x, ++y) { // P!=NP
         if ( (p==0) || (*y > maxy) ) {
            maxx = *x;
            maxy = *y;
         }
      }
      ro->SetChanOffset(i, maxx);
      // TODO: something better? what about fit quality?
      ro->SetChanOffsetError(i, h->GetBinError( h->FindBin(maxx) ) );
   }
   //ro->Print();
   
   return ro;
}

void drawChanCorlFits(const Char_t* infn,
                      const Char_t* alttag=NULL,
                      const Char_t* datbr="FilteredData.",
                      const Char_t* dattnm="CorlFilt",
                      const Char_t* offtnm="FitOffsets") {

   gStyle->SetOptStat(0);
   
   setupDataTree(infn, dattnm, datbr, alttag);
   setupOffsetTree(infn, offtnm, alttag);
   
   const Long64_t n = datt->GetEntries();

   c2 = new TCanvas("c2","c2",1400,900);
   TString hn;
   hOffsets = new TObjArray(TSnRecoChanOffsets::kChCmbs);
   TSnRecoChanOffsets* outro(0);
   if (alttag==NULL) {
      //fco->Print();
      outro = fillCrossChanCorlHists();
      Printf("peak deltaTs:");
      outro->Print();
   } else {
      //fso->Print();
   }
   TString sinfn(infn);
   const Ssiz_t sl = sinfn.Last('/')+1;
   TString pinfn = sinfn(sl, sinfn.Length()-sl);
   TString dinfn = sinfn(0, sl);
   TString outfn = Form("%sAveCorl.%s",dinfn.Data(),pinfn.Data());
   outf = TFile::Open(outfn.Data(), "recreate");
   if (alttag==NULL) {
      UInt_t cols, rows;
      TSnPlotBrowser::CalcCanvDivs(cols, rows, TSnRecoChanOffsets::kChCmbs);
      c2->Divide(cols, rows);
      TH1* h;
      for (UInt_t i=0; i<TSnRecoChanOffsets::kChCmbs; ++i) {
         c2->cd(i+1);
         //c2->GetPad(i+1)->SetLogy();
         h = dynamic_cast<TH1*>(hOffsets->At(i));
         h->Draw();
      }
      c2->cd();
      c2->Update();
      outf->cd();
      outro->Write();
      delete outf; outf=0;
   } else {

   }
   
   
   c1 = new TCanvas("c1","c1",1400,900);
   
   for (Long64_t i=0; i<n; ++i) {
      
      datt->GetEntry(i);
      const Long64_t oi = offt->GetEntryNumberWithIndex(dmd->GetRunNum(),
                                                        dhd->GetEvtNum());
      if (oi>-1) {
         offt->GetEntry(oi);

         TSnPlotBrowser* pb = new TSnPlotBrowser("pb");
         pb->SetCanvas(c1);
         TObjArray plots;
         plots.SetOwner(kTRUE);
         TGraph* gd, * gs, * go;
         TLine* ln;
         for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            gd = dat->NewGraphForChan(ch, kTRUE, kFALSE);
            gd->SetLineColor(chcols[ch]);
            gd->SetMarkerColor(chcols[ch]);
            gd->SetMarkerStyle(chmrks[ch]);
            gd->SetMinimum(datmin);
            gd->SetMaximum(datmax);
            gd->SetTitle(Form("run=%u, evt=%u, chan=%hhd",
                              dmd->GetRunNum(), dhd->GetEvtNum(), ch));
            plots.Add(gd);
            pb->AddPlotToSet("Indivual Chans", gd, "apl", ch+1);
            pb->AddPlotToSet("Chans Overlayed", gd, 
                             (ch==0) ? "apl" : "pl", 1);
            if (alttag==NULL) {
               //fco->Print();
               for (UChar_t xc=0; xc<ch; ++xc) {
                  const Int_t di = TSnRecoChanOffsets::IndexFor(ch, xc);
                  
                  TH1F* h = new TH1F(Form("h_ch%hhu_xc%hhu",ch,xc),
                                     dfl[di]->GetSpline()->GetTitle(),
                                     100,
                                     dfl[di]->GetSpline()->GetXmin(),
                                     dfl[di]->GetSpline()->GetXmax());
                  h->SetMinimum(crlmin);
                  h->SetMaximum(crlmax);
                  plots.Add(h);
                  pb->AddPlotToSet("Correlations", h, "axis", di+1);
                  pb->AddPlotToSet("Correlations", dfl[di]->GetSpline(),
                                   "cp same", di+1);
               
                  const Double_t dt  = fco->GetChanOffset(ch,xc);
                  const Double_t min = 
                     dfl[di]->Eval( dfl[di]->FindXAtMinimum() );
                  const Double_t max = 
                     dfl[di]->Eval( dfl[di]->FindXAtMaximum() );
                  ln = new TLine(dt, min, dt, max);
                  ln->SetLineColor(dtcol);
                  ln->SetLineStyle(dtsty);
                  ln->SetLineWidth(dtwid);
                  plots.Add(ln);
                  pb->AddPlotToSet("Correlations", ln, "same", di+1);
               
                  gs = dat->NewGraphForChan(xc, kTRUE, kFALSE,
                                            fco->GetChanOffset(ch, xc),
                                            kFALSE);
                  gs->SetLineColor(chcols[xc]);
                  gs->SetMarkerColor(chcols[xc]);
                  gs->SetMarkerStyle(chmrks[xc]);
                  gs->SetMinimum(datmin);
                  gs->SetMaximum(datmax);
                  plots.Add(gs);
                  pb->AddPlotToSet("Fitted Shifts", gd, "apl", di+1);
                  pb->AddPlotToSet("Fitted Shifts", gs, "pl",  di+1);
               
                  go = dat->NewGraphForChan(xc, kTRUE, kFALSE);
                  go->SetLineColor(shcol);
                  go->SetMarkerColor(shcol);
                  go->SetMarkerStyle(chmrks[xc]);
                  go->SetMinimum(datmin);
                  go->SetMaximum(datmax);
                  plots.Add(go);
                  pb->AddPlotToSet("Check Shifts", go, "apl", di+1);
                  pb->AddPlotToSet("Check Shifts", gs, "pl",  di+1);
               
               } // xc loop
            } else {
               //fso->Print();
               TH1F* h = new TH1F(Form("h_ch%hhu",ch),
                                  dfl[ch]->GetSpline()->GetTitle(),
                                  100,
                                  dfl[ch]->GetSpline()->GetXmin(),
                                  dfl[ch]->GetSpline()->GetXmax());
               h->SetMinimum(crlmin);
               h->SetMaximum(crlmax);
               plots.Add(h);
               pb->AddPlotToSet("Correlations", h, "axis", ch+1);
               pb->AddPlotToSet("Correlations",
                                dfl[ch]->GetSpline(),
                                "cp same", ch+1);
               const Double_t dt  = fso->GetTimeOffset(ch);
               const Double_t min = 
                  dfl[ch]->Eval( dfl[ch]->FindXAtMinimum() );
               const Double_t max = 
                  dfl[ch]->Eval( dfl[ch]->FindXAtMaximum() );
               ln = new TLine(dt, min, dt, max);
               ln->SetLineColor(dtcol);
               ln->SetLineStyle(dtsty);
               ln->SetLineWidth(dtwid);
               plots.Add(ln);
               pb->AddPlotToSet("Correlations", ln, "same", ch+1);
               
               gs = adt->NewGraphForChan(ch, kTRUE, kFALSE, dt, kFALSE);
               gs->SetLineColor(shcol);
               gs->SetMarkerColor(shcol);
               gs->SetMarkerStyle(chmrks[ch]);
               gs->SetMinimum(datmin);
               gs->SetMaximum(datmax);
               gd->SetTitle(Form("chan%hhd: color = run%u evt%u, "
                                 "black = run%u evt%u",
                                 ch, dmd->GetRunNum(), dhd->GetEvtNum(),
                                 amd->GetRunNum(), ahd->GetEvtNum()));
               pb->AddPlotToSet("Fitted Shifts", gd, "apl", ch+1);
               pb->AddPlotToSet("Fitted Shifts", gs,  "pl", ch+1);

            }
         } // ch loop
         
         pb->DrawButtonBar();
         pb->DrawPlots("Indivual Chans");
         TObject* o(0);
         while ( (o = c1->WaitPrimitive())!=0 ) {
            gSystem->ProcessEvents();
         }
         
         delete pb;
      }
   }
   
}


