#include "TSnCalFFTData.h"

#include <TMath.h>
#include <TGraph.h>
#include <TArrayF.h>

#include "TSnSpectral.h"
#include "TSnCalWvData.h"

#include <stdexcept>
#include <algorithm>

ClassImp(TSnCalFFTData);

TSnCalFFTData::~TSnCalFFTData() {
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      delete fCache[ch];
   }  
}

void TSnCalFFTData::Swap(TSnCalFFTData& a, TSnCalFFTData& b) {
   std::swap(a.fFFT, b.fFFT);
   std::swap(a.fFreq, b.fFreq);
   std::swap(a.fCache, b.fCache);
}

void TSnCalFFTData::CopyDataFrom(const TSnCalFFTData& o) {
   memmove(fFFT, o.fFFT, 
           NSnConstants::kNchans * TSnCalFFTData::kFftPts * sizeof(Float_t));
   memmove(fFreq, o.fFreq, TSnCalFFTData::kFftPts);
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      delete fCache[ch];
      fCache[ch] = new TGraph(*(o.fCache[ch]));
   }
}

void TSnCalFFTData::ClearCache() {
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      delete fCache[ch]; 
      fCache[ch]=0;
   }
}

void TSnCalFFTData::AssertChan(const UChar_t ch) const {
   if (ch>=NSnConstants::kNchans) {
      throw std::runtime_error(Form("<TSnCalFFTData::AssertChan>: "
                                    "Invalid channel number [%hhu] "
                                    "(max=%hhu).",
                                    ch, NSnConstants::kNchans));
   }
}

void TSnCalFFTData::AssertPoint(const UChar_t point) const {
   if (point>=kFftPts) {
      throw std::runtime_error(Form("<TSnCalFFTData::AssertPoint>: "
                                    "Invalid point number [%hhu] "
                                    "(max=%hhu).",
                                    point, kFftPts));
   }
}

const Float_t* TSnCalFFTData::GetFFTData(const UChar_t ch) const {
   AssertChan(ch);
   return &(fFFT[ch][0]);
}

Float_t* TSnCalFFTData::GetFFTData(const UChar_t ch) {
   AssertChan(ch);
   return &(fFFT[ch][0]);
}

Float_t TSnCalFFTData::GetFreqData(const UChar_t point) const {
   AssertPoint(point);
   return fFreq[point];
}

Float_t TSnCalFFTData::GetFFTData(const UChar_t ch,
                                  const UChar_t point) const {
   const Float_t* fc = GetFFTData(ch);
   AssertPoint(point);
   return fc[point];
}

Float_t TSnCalFFTData::FrommVToPower(const Float_t v) {
   return v * v * 1.0e-6; // mV^2 to V^2
}

Float_t TSnCalFFTData::FromPowerToDb(const Float_t v,
                                     const Float_t p0) {
   // no check that p0>0 here.
   return 10.0 * TMath::Log10( v / p0 );
}

Float_t TSnCalFFTData::GetFFTInPower(const UChar_t ch,
                                     const UChar_t point) const {
   // get this fft amplitude in units of power (V^2/1ohm) instead of mV
   return FrommVToPower( GetFFTData(ch, point) );
}

Float_t TSnCalFFTData::GetFFTInDb(const UChar_t ch, const UChar_t point,
                                  const Float_t p0) const {
   AssertRefPower(p0);
   return FromPowerToDb( GetFFTData(ch, point), p0 );
}

void TSnCalFFTData::AssertRefPower(const Float_t p0) const {
   if (p0<=0) {
      throw std::runtime_error(Form("<TSnCalFFTData::AssertRefPower>: "
                                    "Invalid reference power [%g]. "
                                    "Try one of the 'InPower' functions "
                                    " if units of dB are not needed.",
                                    p0));
   }
}

void TSnCalFFTData::BuildFFTGraph(const UChar_t ch, TArrayF& fft)  {
   fCache[ch] = TSnSpectral::NewGraphFromRealFFT( fft.GetArray(),
                                                  fft.GetSize(),
                         static_cast<Float_t>(1.0 / NSnConstants::kSampRate) );
}

const TGraph& TSnCalFFTData::GetFFTGraph(const UChar_t ch) const {
   AssertChan(ch); 
   if (fCache[ch]==0) {
      throw std::runtime_error(Form("<TSnCalFFTData::GetFFTGraph>: "
                                    "No cached FFT graph for channel [%hhu].",
                                    ch));
   }
   return *(fCache[ch]);
}

TGraph* TSnCalFFTData::NewFFTGraph(const UChar_t ch) const {
   AssertChan(ch); 
   return new TGraph( GetFFTGraph(ch) );
}

TGraph* TSnCalFFTData::NewFFTGraphPowOrDb(const UChar_t ch,
                                          const Float_t p0) const {
   // assumes the cached graphs have units of mV
   
   TGraph* g = NewFFTGraph(ch);
   const Int_t n = g->GetN();
   Double_t* y = g->GetY();
   for (Int_t i=0; i<n; ++i, ++y) {
      *y = FrommVToPower(*y); // mV^2 to V^2
      if (p0>0) {
         *y = FromPowerToDb(*y, p0);
      }
   }
   return g;
}

TGraph* TSnCalFFTData::NewFFTGraphInPower(const UChar_t ch) const {
   // assumes the cached graphs have units of mV
   //
   // get an FFT graph with units of power (V^2/1ohm) on the y-axis
   //
   // see TSnSpectral::NewGraphFromRealFFT -- it returns quad sum of
   // fft coefficients => square them for power

   return NewFFTGraphPowOrDb(ch, 0);
}

TGraph* TSnCalFFTData::NewFFTGraphInDb(const UChar_t ch, 
                                       const Float_t p0) const {
   // first gets the FFT in units of power (V^2/1ohm),
   // then scales by 10log10(power/p0)
   //
   // assumes the cached graphs have units of mV
   // see TSnSpectral::NewGraphFromRealFFT -- it returns quad sum of
   // fft coefficients => square them for power

   AssertRefPower(p0);
   return NewFFTGraphPowOrDb(ch, p0);
}
   
void TSnCalFFTData::SetFFTFor(const TSnCalWvData& dat) {

   ClearAll();

   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      
      TArrayF df( NSnConstants::kNsamps, dat.GetData(ch) );
      
      TSnSpectral::RealFFT( df.GetArray(), NSnConstants::kNsamps );
      
      // build the cache
      BuildFFTGraph(ch, df);
      
      // copy the FFT
      const Double_t* q = fCache[ch]->GetX();
      const Double_t* f = fCache[ch]->GetY();
      const Int_t     n = fCache[ch]->GetN();
      Float_t*        e = &(fFFT[ch][0]);
      for (UChar_t sm=0; sm<n; ++sm, ++q, ++f, ++e) {
         *e = static_cast<const Float_t>(*f);
         if (ch==0) {
            fFreq[sm] = static_cast<const Float_t>(*q);
         }
      }
      
   }
   
}
