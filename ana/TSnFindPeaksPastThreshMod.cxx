#include "TSnFindPeaksPastThreshMod.h"

#include <TGraph.h>
#include <TObjArray.h>
#include <TSpline.h>
#include <TCanvas.h>
#include <TSystem.h>

#include "TSnPlotBrowser.h"
#include "TSnInterp1DLstSqConvo.h"
#include "NSnConstants.h"

ClassImp(TSnFindPeaksPastThreshMod);

const UInt_t TSnFindPeaksPastThreshMod::kDebugPlots = 100;
const UInt_t TSnFindPeaksPastThreshMod::kDebugPeakFind = 150;


void TSnFindPeaksPastThreshMod::Begin() {
   // check if we've been told to not do anything
   if ( (fDoHiThr==kFALSE) && (fDoLoThr==kFALSE) ) {
      SendError(kAbortAnalysis, "Begin",
                "Not finding either high- or low-side peaks. "
                "Why am I here, then?");
   }
}

void TSnFindPeaksPastThreshMod::Process() {
   
   const TObjArray* data = dynamic_cast<const TObjArray*>(
      FindObjThisEvt(fIntpArNm.Data()));
   const TObjArray* deriv = dynamic_cast<const TObjArray*>(
      FindObjThisEvt(fDervArNm.Data()));
   if ( (data!=0) && (deriv!=0) ) {
      
      TObjArray* peaksAr = new TObjArray(NSnConstants::kNchans);
      peaksAr->SetName(fPkArNm.Data());
      TObjArray* crossAr = new TObjArray(NSnConstants::kNchans);
      crossAr->SetName(fCrsArNm.Data());
      
      const TSnInterp1DLstSqConvo* dat(0), * der(0);
      TGraph* gPeaks(0), * gCrs(0);
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         dat = dynamic_cast<const TSnInterp1DLstSqConvo*>(data->At(ch));
         der = dynamic_cast<const TSnInterp1DLstSqConvo*>(deriv->At(ch));
         const TSpline3* dxy = dat->GetSpline();
         if ( (dat!=0) && (der!=0) && (dxy!=0) ) {

            gPeaks = new TGraph;
            gPeaks->SetName(Form("%sCh%hhu",fPkArNm.Data(),ch));
            gCrs = new TGraph;
            gCrs->SetName(Form("%sCh%hhu",fCrsArNm.Data(),ch));
            
            peaksAr->AddAt(gPeaks, ch);
            crossAr->AddAt(gCrs, ch);
            
            const Int_t n = dxy->GetNp();
            Double_t x(0), y(0), ax(0), ay(0);
            Double_t hris(0), hfal(0), lris(0), lfal(0);
            Bool_t upok(kFALSE), dnok(kFALSE);
            for (Int_t i=0; i<n; ++i) {
               dxy->GetKnot(i, x, y);
               if (fDoHiThr && (i>0)) {
                  dxy->GetKnot(i-1, ax, ay);
                  CheckPeak(x,y,ax,ay, fHiThresh, hris, hfal, upok, 
                            dat, der, gCrs, gPeaks);
                  if (GetVerbosity()>=kDebugPeakFind) {
                     Printf("hi: i=%d, x=%g, y=%g, ax=%g, ay=%g, th=%g, "
                            "hr=%g, hf=%g, ok=%s",
                            i, x, y, ax, ay, fHiThresh, hris, hfal,
                            (upok) ? "true" : "false");
                  }
               }            
               if (fDoLoThr && (i<n-1)) {
                  dxy->GetKnot(i+1, ax, ay);
                  CheckPeak(x,y,ax,ay, fLoThresh, lfal, lris, dnok, 
                            dat, der, gCrs, gPeaks);
                  if (GetVerbosity()>=kDebugPeakFind) {
                     Printf("lo: i=%d, x=%g, y=%g, ax=%g, ay=%g, th=%g, "
                            "hr=%g, hf=%g, ok=%s",
                            i, x, y, ax, ay, fLoThresh, lfal, lris,
                            (dnok) ? "true" : "false");
                  }
               }
            }
            
         } else {
            SendError(kAbortModule, "Process",
                      "Could not get data (%p) and/or derivative (%p) "
                      "and/or data spline (%p) "
                      "for channel (%hhu)",
                      static_cast<const void*>(dat),
                      static_cast<const void*>(der),
                      static_cast<const void*>(dxy),
                      ch);
            break;
         }
      }
      
      AddObjThisEvt(peaksAr);
      AddObjThisEvt(crossAr);


      if (GetVerbosity()>=kDebugPlots) {
         TCanvas* c1 = new TCanvas("c1","c1",900,800);
         UInt_t col(0), row(0);
         TSnPlotBrowser::CalcCanvDivs(col,row,NSnConstants::kNchans);
         c1->Divide(col,row);
         for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            TSnInterp1DLstSqConvo* d = 
               dynamic_cast<TSnInterp1DLstSqConvo*>(data->At(ch));
            gPeaks = dynamic_cast<TGraph*>(peaksAr->At(ch));
            gCrs   = dynamic_cast<TGraph*>(crossAr->At(ch));
            c1->cd(ch+1);
            d->Draw();
            gPeaks->SetMarkerStyle(20);
            gPeaks->SetMarkerColor(kViolet-1);
            gPeaks->Draw("p");
            gCrs->SetMarkerStyle(5);
            gCrs->SetMarkerColor(kRed+1);
            gCrs->Draw("p");
         }
         c1->cd();
         c1->Update();
         TObject* o(0);
         while ( (o = c1->WaitPrimitive())!=0 ) {
            gSystem->ProcessEvents();
         }
         delete c1;
         
      }
      
   } else {
      SendError(kAbortModule, "Process",
                "Could not get data (%s) (%p) and/or derivative (%s) (%p)",
                fIntpArNm.Data(), static_cast<const void*>(data),
                fDervArNm.Data(), static_cast<const void*>(deriv));
   }
      
}

void TSnFindPeaksPastThreshMod::CheckPeak(const Double_t x, const Double_t y,
                                          const Double_t ax, const Double_t ay,
                                          const Double_t thresh,
                                          Double_t& ris, Double_t& fal,
                                          Bool_t& isok,
                                          const TSnInterp1DLstSqConvo* dat,
                                          const TSnInterp1DLstSqConvo* der,
                                          TGraph* gCrs, TGraph* gPeaks) const {
   if ( (y>thresh) && (ay<thresh) ) {
      // pos rising or neg falling
      ris = 0.5*(x+ax);
      isok = kTRUE;
   } else if ( isok && (y<thresh) && (ay>thresh) ) {
      // pos falling or neg rising
      fal = 0.5*(x+ax);
      gCrs->SetPoint( gCrs->GetN(), ris, thresh);
      gCrs->SetPoint( gCrs->GetN(), fal, thresh);
      // peak has derivative 0 in this window
      const Double_t peak = der->FindXAt(0.0, ris, fal);
      gPeaks->SetPoint( gPeaks->GetN(), peak, dat->Eval(peak) );
      isok = kFALSE;
   }
}
