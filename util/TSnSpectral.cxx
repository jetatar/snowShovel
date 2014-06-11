#include <TSnSpectral.h>

#include <TGraph.h>

ClassImp(TSnSpectral);

const Double_t TSnSpectral::kTwoPi   = TMath::TwoPi(); // TMath doesn't cache it
const Double_t TSnSpectral::kEpsilon = 1e-8;
const UInt_t   TSnSpectral::kMACC    = 4; // num interpolation points per 1/4 cycle
                                          // of highest frequency

TGraph* TSnSpectral::NewGraph(const Int_t i) {
   return (i>0) ? (new TGraph(i)) : (new TGraph);
}

void TSnSpectral::SetPoint(TGraph* gr, const Int_t i,
                           const Double_t x, const Double_t y) {
   if (gr!=0) {
      gr->SetPoint(i, x, y);
   } else {
      throw std::invalid_argument("<TSnSpectral::SetPoint>: "
                                  "no graph provided (null pointer)");
   }
}

void TSnSpectral::GetRealFFTFreqBins(const UInt_t n, const Double_t sampDt,
                                     Int_t& nFreqBins,
                                     Double_t& freqMin, Double_t& freqMax) {
   // given the number of samples (n) and the time distance between the
   // samples (sampDt), set the values of nFreqBins, freqMin and freqMax
   // to correspond to the frequency binning. units are 1/(units of sampDt).
   
   const UInt_t nhsmp = n >> 1; // n/2
   const Double_t fi = 1.0 / static_cast<Double_t>(sampDt*n);
   // frequencies at bin centers
   freqMin = -0.5*fi;
   freqMax = (nhsmp+0.5)*fi;
   nFreqBins = nhsmp + 1;
}

