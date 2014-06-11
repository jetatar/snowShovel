#include "TSnRecoChanOffsets.h"

#include <TMath.h>

#include "NSnConstants.h"

#include <stdexcept>

ClassImp(TSnRecoChanOffsets);

// just calculate this once
const UInt_t TSnRecoChanOffsets::kChCmbs = 
   TMath::Binomial(NSnConstants::kNchans, 2);

TSnRecoChanOffsets::TSnRecoChanOffsets(const Char_t* name,
                                       const Char_t* minimizerTag) :
   TSnRecoResult(name, minimizerTag),
   fDts(kChCmbs),
   fErr(kChCmbs),
   fLik(0) {
}

TSnRecoChanOffsets::TSnRecoChanOffsets(const Char_t* name,
                                       const Char_t* minimizerTag,
                                       const Double_t* os,
                                       const Double_t* err,
                                       const Double_t qual) :
   TSnRecoResult(name, minimizerTag),
   fDts(kChCmbs),
   fErr(kChCmbs),
   fLik(qual) {
   // a buffer overflow will occurr if os does not contain at
   // least kChCmbs elements
   if (os!=0) {
      fDts.Set(kChCmbs, os);
   }
   if (err!=0) {
      fErr.Set(kChCmbs, err);
   }
}

UInt_t   TSnRecoChanOffsets::IndexFor(const UChar_t up, const Char_t lo) {
   // up = bigger chan num
   // lo = lower chan num
   //
   // ordering the parameters like so:
   // dts[0] = ch1 - ch0        dts[3] = ch3 - ch0
   // dts[1] = ch2 - ch0        dts[4] = ch3 - ch1
   // dts[2] = ch2 - ch1        dts[5] = ch3 - ch2
   // (i.e. ordered i,j for i=0..3, j<i)
   //
   // the index of chX - chY (where X>Y) is then
   // dt10 + dt20 + dt21 + ... + dt(X-1)    + dtX0 + dtX1 ... + dtXY
   // = (0 +    1 +    2 + ... +    X-1)         + Y
   // = ( X*(X-1)/2 ) + Y
   if (up>lo) {
      return ((up*(up-1)/2)+lo);
   } else {
      throw std::runtime_error(Form("<TSnRecoChanOffsets::IndexFor>: "
                                    "IndexFor(%hhu, %hhu) invalid. Require "
                                    "first channel to be greater than second.",
                                    up, lo));
   }
   return 0;
}

Double_t TSnRecoChanOffsets::GetChanOffset(const UChar_t c1, 
                                           const UChar_t c2) const {
   // return t_c1 - t_c2
   
   if (c1!=c2) {
      const UChar_t up = TMath::Max(c1,c2);
      const UChar_t lo = TMath::Min(c1,c2);
      const Double_t d = fDts.At( IndexFor(up, lo) );
      return (c1==up) ? d : -d;
   }
   return 0;
}

Double_t TSnRecoChanOffsets::GetChanOffsetError(const UChar_t c1, 
                                                const UChar_t c2) const {
   // return error for t_c1 - t_c2
   
   if (c1!=c2) {
      const UChar_t up = TMath::Max(c1,c2);
      const UChar_t lo = TMath::Min(c1,c2);
      return fErr.At( IndexFor(up, lo) );
   }
   return 0;
}

void TSnRecoChanOffsets::SetChanOffset(const UChar_t c1, const UChar_t c2,
                                       const Double_t o) {
   if (c1!=c2) {
      const UChar_t up = TMath::Max(c1,c2);
      const UChar_t lo = TMath::Min(c1,c2);
      fDts.SetAt( (c1==up) ? o : -o,
                  IndexFor(up, lo) );
   } else {
      throw std::runtime_error(Form("<TSnRecoChanOffsets::SetChanOffset>: "
                                    "Cannot set channel offset between "
                                    "identical channels (%hhu,%hhu).",
                                    c1,c2));
   }
}

void TSnRecoChanOffsets::SetChanOffsetError(const UChar_t c1, const UChar_t c2,
                                            const Double_t e) {
   if (c1!=c2) {
      const UChar_t up = TMath::Max(c1,c2);
      const UChar_t lo = TMath::Min(c1,c2);
      fErr.SetAt( e, IndexFor(up, lo) );
   } else {
      throw std::runtime_error(Form("<TSnRecoChanOffsets::SetChanOffsetError>: "
                                    "Cannot set error on channel offset between "
                                    "identical channels (%hhu,%hhu).",
                                    c1,c2));
   }
}

void TSnRecoChanOffsets::Print(Option_t* option) const {
   TSnRecoResult::Print(option);
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      for (UChar_t xc=0; xc<ch; ++xc) {
         Printf("t_%hhu - t_%hhu         = %g",ch,xc,GetChanOffset(ch,xc));
         Printf("error (ch%hhu - ch%hhu) = %g",ch,xc,GetChanOffsetError(ch,xc));
      }
   }
   Printf("Fit Quality       = %g", fLik);
}

