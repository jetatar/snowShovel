#include "TSnMath.h"

#include <TH1.h>
#include <TF1.h>
#include <TTimeStamp.h>

ClassImp(TSnMath);

const Double_t TSnMath::kSecPerDay = 3600.00*24.00;

void TSnMath::StepFromPeak(const TH1& h, const Float_t peakFrc,
                           Int_t& lo, Int_t& hi) {
   // step down from the max bin until the specified fraction
   // of the peak is reached. set the hi & lo bins

   const Int_t maxbin = h.GetMaximumBin();
   const Float_t cut  = h.GetBinContent(maxbin) * peakFrc;
   //Printf("maxc=%g, frc=%g, cut=%g", h.GetBinContent(maxbin), peakFrc, cut);
   hi=maxbin; lo=maxbin;
   for (Int_t i=maxbin+1; i<=h.GetNbinsX(); ++i) {
      //Printf("hi: cont(%d)=%g",i,h.GetBinContent(i));
      if (h.GetBinContent(i) > cut) {
         hi=i;
      } else {
         break;
      }
   }
   for (Int_t i=maxbin-1; i>0; --i) {
      //Printf("lo: cont(%d)=%g",i,h.GetBinContent(i));
      if (h.GetBinContent(i) > cut) {
         lo=i;
      } else {
         break;
      }
   }
}

void TSnMath::FindMeanRmsSteppingFromPeak(const TH1& h, const Float_t peakFrc,
                                          Double_t& mean, Double_t& rms,
                                          Double_t& n) {
   // calculate the mean and rms in the range of bins found by
   // StepFromPeak. n will be set to the integral in this range
   const Int_t olo=h.GetXaxis()->GetFirst();
   const Int_t ohi=h.GetXaxis()->GetLast();
   Int_t lo(1), hi(1);
   StepFromPeak(h, peakFrc, lo, hi);
   h.GetXaxis()->SetRange(lo,hi);
   mean = h.GetMean();
   rms  = h.GetRMS();
   n    = h.Integral(lo,hi);
   h.GetXaxis()->SetRange(olo,ohi);
}

void TSnMath::FindGausMeanRmsSteppingFromPeak(TH1& h, const Float_t peakFrc,
                                              Double_t& mean, Double_t& rms,
                                              Double_t& n) {
   // fit a gaussian to get the mean and rms in the range of bins found by
   // StepFromPeak. n will be set to the integral in this range
   const Int_t olo=h.GetXaxis()->GetFirst();
   const Int_t ohi=h.GetXaxis()->GetLast();
   Int_t lo(1), hi(1);
   StepFromPeak(h, peakFrc, lo, hi);
   h.GetXaxis()->SetRange(lo,hi);
      
   TF1 fit("fit","gaus",
           h.GetXaxis()->GetBinLowEdge(lo),
           h.GetXaxis()->GetBinLowEdge(hi)+
           h.GetXaxis()->GetBinWidth(hi));
   const Int_t maxbin = h.GetMaximumBin();
   fit.SetParameters(h.GetBinContent(maxbin),
                     h.GetBinCenter(maxbin),
                     (h.GetBinCenter(hi)+h.GetBinCenter(lo))*0.5);
   h.Fit(&fit, "NRBQ");
   mean = fit.GetParameter(1);
   rms  = fit.GetParameter(2);
   n    = h.Integral(lo,hi);
   h.GetXaxis()->SetRange(olo,ohi);
}

void TSnMath::GetNiceDateAxisFor(const UInt_t startTime,
                                 const UInt_t finishTime,
                                 Int_t& nbins,
                                 Double_t& axmin, Double_t& axmax) {
   // given start and finish times (unix values), calculate axis limits
   // and number of bins such that:
   //   * 1 bin per day
   //   * bin edges at midnight
   //   * startTime >= axmin
   //   * finishTime <= axmax
   //
   // unix value times may be obtained from TDatime::Convert or
   // TTimeStamp::GetSec
   
   const TTimeStamp tslo(startTime);
   const TTimeStamp tshi(static_cast<UInt_t>( finishTime + 
                        (0.08*static_cast<Double_t>(finishTime-startTime))) );
   UInt_t year,month,day;
   tslo.GetDate(kTRUE,0,&year,&month,&day);
   const TTimeStamp dmin(year, month, day, 0, 0, 0);
   tshi.GetDate(kTRUE,0,&year,&month,&day);
   const TTimeStamp dmax(year, month, day, 0, 0, 0);
   axmin = static_cast<Double_t>(dmin.GetSec());
   axmax = static_cast<Double_t>(dmax.GetSec() + kSecPerDay);
   nbins = TMath::CeilNint((axmax-axmin)/kSecPerDay);
}


