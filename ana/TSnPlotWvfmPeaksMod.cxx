#include "TSnPlotWvfmPeaksMod.h"

#include <TObjArray.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TNtuple.h>
#include <TFitResultPtr.h>
#include <TFitResult.h>
#include <TF1.h>

#include "NSnConstants.h"

ClassImp(TSnPlotWvfmPeaksMod);

TSnPlotWvfmPeaksMod::~TSnPlotWvfmPeaksMod() {
   
   // in case an exception was thrown and they weren't cleaned somehow
   CleanArrays();
   delete fNt;
}

void TSnPlotWvfmPeaksMod::CleanArrays() {
   delete fNpeaks; fNpeaks=0;
   delete fPkDistVsPkNum; fPkDistVsPkNum=0;
   delete fPkDistVsPosn; fPkDistVsPosn=0;
   delete fPkAmpVsPosn; fPkAmpVsPosn=0;
   delete fPkAmpVsPkNum; fPkAmpVsPkNum=0;
   delete fPkAmpDiffVsPosn; fPkAmpDiffVsPosn=0;
   delete fPkAmpDiffVsPkNum; fPkAmpDiffVsPkNum=0;
   delete fPkAmpRatioVsPosn; fPkAmpRatioVsPosn=0;
   delete fPkAmpRatioVsPkNum; fPkAmpRatioVsPkNum=0;
   delete fPkDistVsPkNumSlope; fPkDistVsPkNumSlope=0;
   delete fPkDistVsPkNumSlopeVsNpks; fPkDistVsPkNumSlopeVsNpks=0;
}

void TSnPlotWvfmPeaksMod::MakePlots() {
   TString hn;
   
   Int_t tbins(NSnConstants::kNsamps);
   Float_t tmin(-0.5), tmax(NSnConstants::kNsamps-0.5);
   TString tunits = "samples";
   if (fVsTime) {
      tmin /= NSnConstants::kSampRate;
      tmax /= NSnConstants::kSampRate;
      tunits = "ns";
   }
   Int_t dtbins(tbins);
   Float_t dtmin(tmin), dtmax(tmax);
   
   TH1* h(0);
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      hn = Form("hNpeaksCh%s_ch%hhu",fPlotSuf.Data(),ch);
      h = new TH1F(hn.Data(),
                   "Number of peaks;num peaks;events",
                   fNpkbins, fNpkmin, fNpkmax);
      AddOutput(h);
      fNpeaks->AddAt(h, ch);
      
      hn = Form("hPkDistVsPkNum%s_%hhu",fPlotSuf.Data(),ch);
      h = new TH2F(hn.Data(),
                   Form("consec peak dist vs later peak number"
                        ";number of later peak"
                        ";consec peak dist (%s)", tunits.Data()),
                   fNpkbins, fNpkmin, fNpkmax,
                   dtbins, dtmin, dtmax);
      AddOutput(h);
      fPkDistVsPkNum->AddAt(h, ch);
      
      hn = Form("hPkDistVsPosn%s_%hhu",fPlotSuf.Data(),ch);
      h = new TH2F(hn.Data(),
                   Form("consec peak dist vs later peak position"
                        ";later peak posn (%s)"
                        ";consec peak dist (%s)",
                        tunits.Data(), tunits.Data()),
                   tbins, tmin, tmax,
                   dtbins, dtmin, dtmax);
      AddOutput(h);
      fPkDistVsPosn->AddAt(h, ch);
      
      hn = Form("hPkAmpVsPosn%s_%hhu",fPlotSuf.Data(),ch);
      h = new TH2F(hn.Data(),
                   Form("peak amplitude vs peak position"
                        ";peak posn (%s);peak amplitude (%s)",
                        tunits.Data(), fAmpUnits.Data()),
                   tbins, tmin, tmax,
                   fAmpbins, fAmpmin, fAmpmax);
      AddOutput(h);
      fPkAmpVsPosn->AddAt(h, ch);
      
      hn = Form("hPkAmpVsPkNum%s_%hhu",fPlotSuf.Data(),ch);
      h = new TH2F(hn.Data(),
                   Form("peak amplitude vs peak number"
                        ";peak number;peak amplitude (%s)",
                        fAmpUnits.Data()),
                   fNpkbins, fNpkmin, fNpkmax,
                   fAmpbins, fAmpmin, fAmpmax);
      AddOutput(h);
      fPkAmpVsPkNum->AddAt(h, ch);
      
      hn = Form("hPkAmpDiffVsPosn%s_%hhu",fPlotSuf.Data(),ch);
      h = new TH2F(hn.Data(),
                   Form("consec peak amp diff vs "
                        "later peak posn"
                        ";later peak posn (%s)"
                        ";consec peak amp diff (%s)",
                        tunits.Data(), fAmpUnits.Data()),
                   tbins, tmin, tmax,
                   fdAmpbins, fdAmpmin, fdAmpmax);
      AddOutput(h);
      fPkAmpDiffVsPosn->AddAt(h, ch);
      
      hn = Form("hPkAmpDiffVsPkNum%s_%hhu",fPlotSuf.Data(),ch);
      h = new TH2F(hn.Data(),
                   Form("consec peak amp diff vs "
                        "later peak number"
                        ";number of later peak"
                        ";consec peak amp diff (%s)",
                        fAmpUnits.Data()),
                   fNpkbins, fNpkmin, fNpkmax,
                   fdAmpbins, fdAmpmin, fdAmpmax);
      AddOutput(h);
      fPkAmpDiffVsPkNum->AddAt(h, ch);
      
      hn = Form("hPkAmpRatioVsPosn%s_%hhu",fPlotSuf.Data(),ch);
      h = new TH2F(hn.Data(),
                   Form("consec peak amp ratio vs later peak posn "
                        ";later peak posn (%s)"
                        ";consec peak amp diff (%s)",
                        tunits.Data(), fAmpUnits.Data()),
                   tbins, tmin, tmax,
                   fAmpRatbins, fAmpRatmin, fAmpRatmax);
      AddOutput(h);
      fPkAmpRatioVsPosn->AddAt(h, ch);
      
      hn = Form("hPkAmpRatioVsPkNum%s_%hhu",fPlotSuf.Data(),ch);
      h = new TH2F(hn.Data(),
                   "consec peak amp ratio vs later peak number"
                   ";number of later peak;consec peak amp ratio",
                   fNpkbins, fNpkmin, fNpkmax,
                   fAmpRatbins, fAmpRatmin, fAmpRatmax);
      AddOutput(h);
      fPkAmpRatioVsPkNum->AddAt(h, ch);
      
      hn = Form("hPkDistVsPkNumSlope%s_%hhu",fPlotSuf.Data(),ch);
      h = new TH1F(hn.Data(),
                   Form("slope of consec peak dist vs later peak number"
                        ";slope (%s/later peak num);events",tunits.Data()),
                   fPdPnSlpBins, fPdPnSlpMin, fPdPnSlpMax);
      AddOutput(h);
      fPkDistVsPkNumSlope->AddAt(h, ch);
      
      hn = Form("hPkDistVsPkNumSlopeVsNpks%s_%hhu",fPlotSuf.Data(),ch);
      h = new TH2F(hn.Data(),
                   Form("slope of consec peak dist vs later peak num VS "
                        "num of peaks;number of peaks"
                        ";slope (%s/later peak num)",tunits.Data()),
                   fNpkbins, fNpkmin, fNpkmax,
                   fPdPnSlpBins, fPdPnSlpMin, fPdPnSlpMax);
      AddOutput(h);
      fPkDistVsPkNumSlopeVsNpks->AddAt(h, ch);
   }
   
}

void TSnPlotWvfmPeaksMod::SlaveBegin() {
   
   fNpeaks = new TObjArray(NSnConstants::kNchans);
   fPkDistVsPkNum = new TObjArray(NSnConstants::kNchans);
   fPkDistVsPosn = new TObjArray(NSnConstants::kNchans);
   fPkAmpVsPosn = new TObjArray(NSnConstants::kNchans);
   fPkAmpVsPkNum = new TObjArray(NSnConstants::kNchans);
   fPkAmpDiffVsPosn = new TObjArray(NSnConstants::kNchans);
   fPkAmpDiffVsPkNum = new TObjArray(NSnConstants::kNchans);
   fPkAmpRatioVsPosn = new TObjArray(NSnConstants::kNchans);
   fPkAmpRatioVsPkNum = new TObjArray(NSnConstants::kNchans);
   fPkDistVsPkNumSlope = new TObjArray(NSnConstants::kNchans);
   fPkDistVsPkNumSlopeVsNpks = new TObjArray(NSnConstants::kNchans);
   
   MakePlots();
   
   if (fMakePeakTree) {
      fNt = new TNtuple("PeakProps","peak properties",
                        "ch:peaknum:x:y:dx:dy:ry:evt");
      AddOutput(fNt);
   }
}

void TSnPlotWvfmPeaksMod::Process() {
   
   const TObjArray* peaksAr = dynamic_cast<const TObjArray*>(
      FindObjThisEvt(fPkArNm.Data()));
   if (peaksAr!=0) {
      
      const TGraph* gPeaks(0);
      TH1* h(0);
      TH2* h2(0);
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         gPeaks = dynamic_cast<const TGraph*>(peaksAr->At(ch));
         if (gPeaks!=0) {
            
            TGraph gPkDistVsPkNum;
            
            const Int_t np = gPeaks->GetN();
            
            h = dynamic_cast<TH1*>(fNpeaks->At(ch));
            h->Fill(np);
            
            const Int_t start = 1;
            const Double_t* x = gPeaks->GetX() + start;
            const Double_t* y = gPeaks->GetY() + start;
            for (Int_t i=start; i<np; ++i, ++x, ++y) {
               const Double_t dx = *x - *(x-1);
               const Double_t dy = *y - *(y-1);
               const Double_t ry = *(y-1)>1e-10 ? (*y/(*(y-1))) : 0.0;
               
               h2 = dynamic_cast<TH2*>(fPkDistVsPosn->At(ch));
               h2->Fill(*x, dx);
               h2 = dynamic_cast<TH2*>(fPkDistVsPkNum->At(ch));
               h2->Fill(i, dx);
               
               gPkDistVsPkNum.SetPoint( gPkDistVsPkNum.GetN(), i, dx );
               
               h2 = dynamic_cast<TH2*>(fPkAmpVsPosn->At(ch));
               h2->Fill(*x, *y);
               h2 = dynamic_cast<TH2*>(fPkAmpVsPkNum->At(ch));
               h2->Fill(i, *y);

               h2 = dynamic_cast<TH2*>(fPkAmpDiffVsPosn->At(ch));
               h2->Fill(*x, dy);
               h2 = dynamic_cast<TH2*>(fPkAmpDiffVsPkNum->At(ch));
               h2->Fill(i, dy);
               
               h2 = dynamic_cast<TH2*>(fPkAmpRatioVsPosn->At(ch));
               h2->Fill(*x, ry);
               h2 = dynamic_cast<TH2*>(fPkAmpRatioVsPkNum->At(ch));
               h2->Fill(i, ry);
               
               if (fMakePeakTree) {
                  fNt->Fill(ch, i, *x, *y, dx, dy, ry,
                            GetSelector()->GetCurEvt());
               }
            }
            
            Double_t slope(-999);
            if (gPkDistVsPkNum.GetN() > 1) {
               TFitResultPtr r = gPkDistVsPkNum.Fit("pol1","Q0S","",0.5,5.5);
               TF1* f = gPkDistVsPkNum.GetFunction("pol1");
               if ( (f!=0) && (r->IsValid()) ) {
                  slope = f->GetParameter(1);
               }
            }
            h = dynamic_cast<TH1*>(fPkDistVsPkNumSlope->At(ch));
            h->Fill(slope);
            h2 = dynamic_cast<TH2*>(fPkDistVsPkNumSlopeVsNpks->At(ch));
            h2->Fill(np, slope);
            
         } else {
            SendError(kAbortModule, "Process",
                      "Could not get peak graph for channel [%hhu].", ch);
            break;
         }
      }
      
   } else {
      SendError(kAbortModule, "Process",
                "Could not find peaks array named [%s] in event.",
                fPkArNm.Data());
   }
}

void TSnPlotWvfmPeaksMod::SlaveTerminate() {
   CleanArrays();
}
