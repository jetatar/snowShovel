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

void TSnCalFFTData::CopyDataWithoutCacheFrom(const TSnCalFFTData& o) {
   memmove(fFFT, o.fFFT, 
           NSnConstants::kNchans * NSnConstants::kNsamps * sizeof(Float_t));
   memmove(fFreq, o.fFreq, TSnCalFFTData::kFftPts * sizeof(Float_t));
}

void TSnCalFFTData::CopyDataFrom(const TSnCalFFTData& o) {
   CopyDataWithoutCacheFrom(o);
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      delete fCache[ch];
      fCache[ch] = 0;
      if (o.fCache[ch]!=0) {
         fCache[ch] = new TGraph(*(o.fCache[ch]));
      }
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
                                    "Invalid frequency bin [%hhu] "
                                    "(max=%hhu).",
                                    point, kFftPts));
   }
}

void TSnCalFFTData::AssertSamp(const UChar_t sm) const {
   if (sm>=NSnConstants::kNsamps) {
      throw std::runtime_error(Form("<TSnCalFFTData::AssertSamp>: "
                                    "Invalid sample number [%hhu] "
                                    "(max=%hhu).",
                                    sm, NSnConstants::kNsamps));
   }
}

const Float_t* TSnCalFFTData::GetFFTRawCoefs(const UChar_t ch) const {
   AssertChan(ch);
   return &(fFFT[ch][0]);
}

Float_t* TSnCalFFTData::GetFFTRawCoefs(const UChar_t ch) {
   AssertChan(ch);
   return &(fFFT[ch][0]);
}

Float_t TSnCalFFTData::GetFrequency(const UChar_t point) const {
   AssertPoint(point);
   return fFreq[point];
}

Float_t TSnCalFFTData::GetFFTCoeff(const UChar_t ch,
                                   const UChar_t sampleBin) const {
   AssertChan(ch);
   AssertSamp(sampleBin);
   return fFFT[ch][sampleBin];
}

void TSnCalFFTData::GetFFTCoeffPair(const UChar_t ch, 
                                    const UChar_t frqBin,
                                    Float_t& evenCoeff,
                                    Float_t& oddCoeff) const {
   // get the even and odd FFT coefficients that make up this frequency bin
   // the coefficients are:
   // 
   // F_k = even_k + odd_k * exp( i 2pi k / N)
   // where k = 0 .. N-1
   
   AssertPoint(frqBin);
   if (frqBin==0) {
      evenCoeff = fFFT[ch][0];
      oddCoeff  = 0;
   } else if (frqBin==(kFftPts-1)) {
      evenCoeff = fFFT[ch][1];
      oddCoeff  = 0;
   } else {
      const Float_t* const f = &(fFFT[ch][frqBin*2]);
      evenCoeff = *f;
      oddCoeff  = *(f+1);
   }
}

TSnCalFFTData::EvenOddPair_t
TSnCalFFTData::GetFFTCoeffPair(const UChar_t ch,
                               const UChar_t frqBin) const {
   Float_t e(0), o(0);
   GetFFTCoeffPair(ch, frqBin, e, o);
   return EvenOddPair_t(e,o);
}

void TSnCalFFTData::SetFFTCoeff(const UChar_t ch, const UChar_t sampleBin,
                                const Float_t coeff) {
   AssertChan(ch);
   AssertSamp(sampleBin);
   fFFT[ch][sampleBin] = coeff;
}

void TSnCalFFTData::SetFFTCoeffPair(const UChar_t ch, const UChar_t frqBin,
                                    const Float_t evenCoeff,
                                    const Float_t oddCoeff) {
   // set the coefficients for the frequency bin. see GetFFTCoeffPair
   // for a definition of even and odd.
   //
   // for the first and last frequency bin, the oddCoeff is ignored (as
   // these frequency bins are purely real)
   
   AssertChan(ch);
   if (frqBin==0) {
      fFFT[ch][0] = evenCoeff;
   } else if (frqBin==(kFftPts-1)) {
      fFFT[ch][1] = evenCoeff;
   } else {
      AssertPoint(frqBin);
      Float_t* const f = &(fFFT[ch][frqBin*2]);
      *f     = evenCoeff;
      *(f+1) = oddCoeff;
   }
}

void TSnCalFFTData::SetFFTCoeffPair(const UChar_t ch, const UChar_t frqBin,
                                   const TSnCalFFTData::EvenOddPair_t& coeffs) {
   SetFFTCoeffPair(ch, frqBin, coeffs.first, coeffs.second);
}

Float_t TSnCalFFTData::GetFFTMag(const UChar_t ch,
                                 const UChar_t frqBin) const {
   // get the magnitude of the FFT at the specified frequency bin
   //
   // gets the value from the cached graph if available
   if (fCache[ch]!=0) {
      AssertPoint(frqBin);
      return ((fCache[ch])->GetY())[frqBin];
   } else {
      Float_t e(0), o(0);
      GetFFTCoeffPair(ch, frqBin, e, o);
      return TMath::Sqrt( (e*e) + (o*o) );
   }
}

Float_t TSnCalFFTData::GetPhase(const UChar_t ch, const UChar_t frqBin) const {
   // calculate arctan( o / e )
   // uses atan2 ([-pi,pi] in correct quadrant)
   
   Float_t e(0), o(0);
   GetFFTCoeffPair(ch, frqBin, e, o);
   return TMath::ATan2(o, e);
}

Float_t TSnCalFFTData::FrommVToPower(const Float_t v) const {
   return v * v * fToPower;
}

Float_t TSnCalFFTData::FromPowerToDb(const Float_t v,
                                     const Float_t p0) {
   // no check that p0>0 here.
   return 10.0 * TMath::Log10( v / p0 );
}

Float_t TSnCalFFTData::GetFFTMagInPower(const UChar_t ch,
                                        const UChar_t frqBin) const {
   // get this fft amplitude in units of power (V^2/1ohm) instead of mV
   return FrommVToPower( GetFFTMag(ch, frqBin) );
}

Float_t TSnCalFFTData::GetFFTMagInDb(const UChar_t ch, const UChar_t frqBin,
                                     const Float_t refPwr) const {
   AssertRefPower(refPwr);
   return FromPowerToDb( GetFFTMagInPower(ch, frqBin), refPwr );
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

void TSnCalFFTData::BuildFFTGraph(const UChar_t ch) const {
   static const Float_t dt = 
      static_cast<Float_t>(1.0 / NSnConstants::kSampRate);
   delete fCache[ch];
   fCache[ch] = TSnSpectral::NewGraphFromRealFFT( &(fFFT[ch][0]),
                                                  NSnConstants::kNsamps,
                                                  dt );
}

const TGraph& TSnCalFFTData::GetFFTGraph(const UChar_t ch) const {
   AssertChan(ch); 
   if (fCache[ch]==0) {
      BuildFFTGraph(ch);
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
      
      Float_t* const fft = &(fFFT[ch][0]);
      memcpy(fft, dat.GetData(ch), sizeof(Float_t)*NSnConstants::kNsamps);
      TSnSpectral::RealFFT(fft, NSnConstants::kNsamps );
      
      // build the cache
      BuildFFTGraph(ch);
      
      if (ch==0) {
         // set the frequencies
         const Double_t* q = fCache[ch]->GetX();
         const Int_t     n = fCache[ch]->GetN();
         for (UChar_t sm=0; sm<n; ++sm, ++q) {
            fFreq[sm] = static_cast<const Float_t>(*q);
         }
      }
      
   }
   
}

TSnCalWvData*  TSnCalFFTData::NewDataFromInverseFFT() const {
   // make a new TSnCalWvData by inverting this FFT
   // 
   // note that errors are not propagated through the FFT
   
   TString hn(Form("Inv%s",GetName()));
   TSnCalWvData* d = new TSnCalWvData(hn.Data(),
                                      Form("inverted fft (%s)",GetTitle()));
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      Float_t* const dch = d->GetData(ch);
      memcpy(dch, GetFFTRawCoefs(ch), sizeof(Float_t)*NSnConstants::kNsamps);
      TSnSpectral::RealFFT(dch, NSnConstants::kNsamps, kTRUE);
   }
   return d;
}

