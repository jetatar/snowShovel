#include "TSnCalWvData.h"

#include <TGraph.h>
#include <TGraphErrors.h>

#include "TSnRawWaveform.h"

#include <stdexcept>
#include <algorithm>

ClassImp(TSnCalWvData);

void TSnCalWvData::Swap(TSnCalWvData& a, TSnCalWvData& b) {
   std::swap(a.fData, b.fData);
   std::swap(a.fError, b.fError);
}

const Float_t* TSnCalWvData::GetData(const UChar_t ch) const {
   return fData + TSnRawWaveform::GetIndexOf(ch,0);
}

Float_t* TSnCalWvData::GetData(const UChar_t ch) {
   return fData + TSnRawWaveform::GetIndexOf(ch,0);
}

Float_t TSnCalWvData::GetData(const UChar_t ch, const UChar_t s) const {
   return fData[TSnRawWaveform::GetIndexOf(ch,s)];
}

const Float_t* TSnCalWvData::GetError(const UChar_t ch) const {
   return fError + TSnRawWaveform::GetIndexOf(ch,0);
}

Float_t* TSnCalWvData::GetError(const UChar_t ch) {
   return fError + TSnRawWaveform::GetIndexOf(ch,0);
}

Float_t TSnCalWvData::GetError(const UChar_t ch, const UChar_t s) const {
   return fError[TSnRawWaveform::GetIndexOf(ch,s)];
}
   
void TSnCalWvData::SetData(const UChar_t ch, const UChar_t s, const Float_t v) {
   fData[TSnRawWaveform::GetIndexOf(ch,s)] = v;
}

void TSnCalWvData::SetError(const UChar_t ch, const UChar_t s,
                            const Float_t v) {
   fError[TSnRawWaveform::GetIndexOf(ch,s)] = v;
}

TGraph* TSnCalWvData::NewGraphForChan(const UChar_t  ch,
                                      const Bool_t   vsTime,
                                      const Bool_t   withErrors,
                                            Double_t shift,
                                      const Bool_t   doWrap) const {
   // create a new TGraph (if withErrors is false) or a TGraphErrors
   // (if withErrors is true) of the data for the specified channel
   //
   // the x position of the points may be shifted by some amount (in
   // units that correspond with the vsTime option -- either time or
   // samples, accordingly).
   // 
   // if doWrap is true (default), the waveform will be wrapped rather
   // than directly translated
   //
   // the caller is responsible for deleting the graph
   
   TGraph* g(0);
   TGraphErrors* ge(0);
   if (withErrors) {
      ge = new TGraphErrors(NSnConstants::kNsamps);
      g  = ge;
   } else {
      g = new TGraph(NSnConstants::kNsamps);
   }
   g->SetName(Form("%s_ch%hhu",GetName(),ch));
   g->SetTitle(Form("%s (chan %hhu)",GetTitle(),ch));
   const Float_t* const ds = GetData(ch);
   const Float_t* const es = GetError(ch);
   Double_t* const xs = g->GetX();
   Double_t* const ys = g->GetY();
   Double_t* const eys = (withErrors) ? ge->GetEY() : 0;
   const Double_t xmax = (vsTime)
      ? NSnConstants::kNsamps / NSnConstants::kSampRate
      : NSnConstants::kNsamps;
   if (doWrap && (shift<0)) {
      // negative shift with wrapping is the same as a positive shift
      // of (N + shift)
      shift = xmax + shift;
   }
   // need to start the shifted graph array at the left-most element of the
   // x-axis, otherwise the graph will display a line where the points wrap
   const Int_t sbins = static_cast<Int_t>( (vsTime)
                                           ? (shift * NSnConstants::kSampRate)
                                           : shift );
   const UChar_t first = doWrap
      ? (NSnConstants::kNsamps - sbins) % NSnConstants::kNsamps
      : 0;
   // sub time bin offset
   const Double_t offset = shift - ((vsTime) ? sbins / NSnConstants::kSampRate
                                             : sbins);
   const Float_t* d = ds + first;
   const Float_t* e = es + first;
   Double_t*      x = xs;
   Double_t*      y = ys;
   Double_t*     ey = eys;
   for (UChar_t i=0, j=first; 
        i<NSnConstants::kNsamps; ++i, ++j, ++d, ++e, ++x, ++y, ++ey) {
      if (j==NSnConstants::kNsamps) {
         // wrap!
         j = 0; d = ds; e = es;
      }
      *x = offset + static_cast<Double_t>( 
         vsTime ? (static_cast<Float_t>(i)/NSnConstants::kSampRate)
         : i );
      if (doWrap==kFALSE) {
         *x += shift;
      }
      *y = static_cast<Double_t>(*d);
      if (withErrors) {
         *ey = static_cast<Double_t>(*e);
      }
   }
   return g;
}
