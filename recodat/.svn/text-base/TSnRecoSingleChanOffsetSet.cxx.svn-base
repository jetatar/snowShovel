#include "TSnRecoSingleChanOffsetSet.h"

#include <TString.h>
#include <TObjArray.h>

#include "NSnConstants.h"
#include "TSnRecoSingleChanOffset.h"

#include <stdexcept>
#include <algorithm>

ClassImp(TSnRecoSingleChanOffsetSet);

TSnRecoSingleChanOffsetSet::TSnRecoSingleChanOffsetSet(const Char_t* name, 
                                                   const Char_t* minimizerTag) :
   TSnRecoResult(name, minimizerTag),
   fSet(new TObjArray(NSnConstants::kNchans)) {
   // normal ctor
   fSet->SetOwner(kTRUE);
   fSet->SetName(Form("%s.",name)); // in case it's put in a tree
}

TSnRecoSingleChanOffsetSet::TSnRecoSingleChanOffsetSet(
   const TSnRecoSingleChanOffsetSet& o) :
   TSnRecoResult(o),
   fSet(0) {
   // deep copy ctor

   if (o.fSet!=0) {
      const Int_t n = o.fSet->GetSize();
      fSet = new TObjArray(n);
      fSet->SetOwner(kTRUE);
      fSet->SetName(Form("%s.",GetName())); // in case it's put in a tree

      TSnRecoSingleChanOffset* c(0);
      for (Int_t i=0; i<n; ++i) {
         c = dynamic_cast<TSnRecoSingleChanOffset*>(o.fSet->At(i));
         if (c!=0) {
            fSet->AddAt(new TSnRecoSingleChanOffset(*c), i);
         }
      }
   }
}

void TSnRecoSingleChanOffsetSet::Swap(TSnRecoSingleChanOffsetSet& a,
                                      TSnRecoSingleChanOffsetSet& b) {
   std::swap(a.fSet, b.fSet);
}

TSnRecoSingleChanOffsetSet::~TSnRecoSingleChanOffsetSet() {
   // dtor: delete the set and its contents
   delete fSet;
}

const TSnRecoSingleChanOffset* TSnRecoSingleChanOffsetSet::GetOffsetsOfChan(
                                                      const UChar_t ch) const {
   if (fSet!=0) {
      return dynamic_cast<const TSnRecoSingleChanOffset*>(fSet->At(ch));
   }
   return 0;
}

TSnRecoSingleChanOffset* TSnRecoSingleChanOffsetSet::GetOffsetsOfChan(
                                                      const UChar_t ch) {
   if (fSet!=0) {
      return dynamic_cast<TSnRecoSingleChanOffset*>(fSet->At(ch));
   }
   return 0;
}

const TSnRecoSingleChanOffset& TSnRecoSingleChanOffsetSet::GetOffsetsOfChanSafe(
                                                      const UChar_t ch) const {
   const TSnRecoSingleChanOffset* x = GetOffsetsOfChan(ch);
   if (x==0) {
      throw std::runtime_error(
         Form("<TSnRecoSingleChanOffsetSet::GetOffsetsOfChanSafe>: "
              "No single channel offset for ch [%hhu] (fSet at %p)",
              ch, static_cast<const void*>(fSet)));
   }
   return *x;
}

TSnRecoSingleChanOffset& TSnRecoSingleChanOffsetSet::GetOffsetsOfChanSafe(
                                                      const UChar_t ch) {

   // call the const version and then do away with the const-ness of the
   // result. this is perfectly fine as long as the TSnRecoSingleChanOffset
   // is never expected to be const in all circumstances (which it better
   // not be anyway since TObjArray has no means to impose such a
   // restriction)
   TSnRecoSingleChanOffset& x = const_cast<TSnRecoSingleChanOffset&>(
      const_cast<const TSnRecoSingleChanOffsetSet*>(this)
      ->GetOffsetsOfChanSafe(ch) );
   return x;
}

void TSnRecoSingleChanOffsetSet::SetOffsetsOfChan(const UChar_t ch,
                                            const TSnRecoSingleChanOffset& co) {
   // copy the single channel offset into this set
   if (fSet!=0) {
      TSnRecoSingleChanOffset* x = GetOffsetsOfChan(ch);
      if (x!=0) {
         fSet->Remove(x);
         delete x;
      }
      fSet->AddAt(new TSnRecoSingleChanOffset(co), ch);
   } else {
      throw std::runtime_error(
         "<TSnRecoSingleChanOffsetSet::SetOffsetsOfChan>: No set available");
   }
}

void TSnRecoSingleChanOffsetSet::Print(Option_t* option) const {
   TSnRecoResult::Print(option);
   TObject* o;
   const Int_t n = fSet->GetSize();
   for (Int_t i=0; i<n; ++i) {
      o = fSet->At(i);
      if (o!=0) {
         o->Print(option);
      }
   }
}

Double_t TSnRecoSingleChanOffsetSet::GetTimeOffset(const UChar_t ch) const {
   return GetOffsetsOfChanSafe(ch).GetTimeOffset();
}

Double_t TSnRecoSingleChanOffsetSet::GetOffsetError(const UChar_t ch) const {
   return GetOffsetsOfChanSafe(ch).GetOffsetError();
}

Double_t TSnRecoSingleChanOffsetSet::GetFitQual(const UChar_t ch) const {
   return GetOffsetsOfChanSafe(ch).GetFitQual();
}
