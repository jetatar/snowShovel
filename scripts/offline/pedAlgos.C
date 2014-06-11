#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TString.h>
#include <TMath.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TError.h>
#include <TGraphErrors.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TProfile.h>

#include <NSnConstants.h>
#endif

void stepFromPeak(const TH1& h, const Float_t peakFrc,
                  Int_t& lo, Int_t& hi,
                  const Bool_t ignoreExtremeAdcs=kFALSE) {
   // step down from the max bin until the specified fraction
   // of the peak is reached. return the hi & lo bins
   
   if (ignoreExtremeAdcs) {
      h.GetXaxis()->SetRangeUser(1,4094);
   }
   const Int_t maxbin = h.GetMaximumBin();
   const Float_t cut  = h.GetBinContent(maxbin) * peakFrc;
   //Printf("maxc=%g, frc=%g, cut=%g", h.GetBinContent(maxbin), peakFrc, cut);
   hi=maxbin; lo=maxbin;
   for (Int_t i=maxbin+1; i<=h.GetNbinsX(); i++) {
      //Printf("hi: cont(%d)=%g",i,h.GetBinContent(i));
      if (h.GetBinContent(i) > cut) {
         hi=i;
      } else {
         break;
      }
   }
   for (Int_t i=maxbin-1; i>0; i--) {
      //Printf("lo: cont(%d)=%g",i,h.GetBinContent(i));
      if (h.GetBinContent(i) > cut) {
         lo=i;
      } else {
         break;
      }
   }
}

void findMeanStepFromPeak(const TH1& h, const Float_t peakFrc,
                          Double_t& mean, Double_t& rms, Double_t& n,
                          const Bool_t ignoreExtremeAdcs=kFALSE) {
   Int_t lo, hi;
   stepFromPeak(h, peakFrc,
                lo, hi,
                ignoreExtremeAdcs);
   h.GetXaxis()->SetRange(lo,hi);
   mean = h.GetMean();
   rms  = h.GetRMS();
   n    = h.Integral(lo,hi);
   h.GetXaxis()->SetRange();
}

void fitGausStepFromPeak(TH1& h, const Float_t peakFrc,
                         Double_t& mean, Double_t& rms, Double_t& n,
                         const Bool_t ignoreExtremeAdcs=kFALSE) {
   Int_t lo, hi;
   stepFromPeak(h, peakFrc,
                lo, hi,
                ignoreExtremeAdcs);
   h.GetXaxis()->SetRange(lo,hi);
   TF1* fit = new TF1("fit","gaus",
                      h.GetXaxis()->GetBinLowEdge(lo),
                      h.GetXaxis()->GetBinLowEdge(hi)+
                      h.GetXaxis()->GetBinWidth(hi));
   const Int_t maxbin = h.GetMaximumBin();
   fit->SetParameters(h.GetBinContent(maxbin),
                      h.GetBinCenter(maxbin),
                      h.GetBinCenter(hi)-h.GetBinCenter(maxbin));
   h.Fit(fit, "NRBQ");
   mean = fit->GetParameter(1);
   rms  = fit->GetParameter(2);
   n    = h.Integral(lo,hi);
   h.GetXaxis()->SetRange();
   delete fit;
}


