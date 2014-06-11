#include "TSnStopPosnSet.h"

#include "TSnCalWvData.h"

#include <algorithm>
#include <stdexcept>

ClassImp(TSnStopPosnSet);

void TSnStopPosnSet::Swap(TSnStopPosnSet& a, TSnStopPosnSet& b) {
   std::swap(a.fFirst, b.fFirst);
   std::swap(a.fWidth, b.fWidth);
   std::swap(a.fQual,  b.fQual);
}

void TSnStopPosnSet::Copy(TObject& o) const {
   TSnStopPosnSet& s = dynamic_cast<TSnStopPosnSet&>(o);
   s = *this;
}

Bool_t TSnStopPosnSet::AssertBounds(const UChar_t ch) const {
   const Bool_t x = ch < NSnConstants::kNchans;
   if (x==kFALSE) {
      throw std::runtime_error(Form("<TSnStopPosnSet::AssertBounds>: "
                                    "Channel [%hhu] out of bounds.", ch));
   }
   return x;
}

TSnCalWvData* TSnStopPosnSet::NewShiftedData(const TSnCalWvData& rw,
                                             const Char_t* name,
                                             const Char_t* title) const {
   TSnCalWvData* d = new TSnCalWvData(name, title);
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {

      const SmpNum_t beg = GetWaveBeginSample(ch);

      const Float_t* ind = rw.GetData(ch);
            Float_t* oud = d->GetData(ch);
      std::rotate_copy(ind, ind + beg, ind + NSnConstants::kNsamps, oud);

      const Float_t* ine = rw.GetError(ch);
            Float_t* oue = d->GetError(ch);
      std::rotate_copy(ine, ine + beg, ine + NSnConstants::kNsamps, oue);
      
   }
   return d;
}

