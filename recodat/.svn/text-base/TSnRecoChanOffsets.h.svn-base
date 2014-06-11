#ifndef SNS_TSnRecoChanOffsets
#define SNS_TSnRecoChanOffsets

#include <TArrayD.h>

#include "TSnRecoResult.h"

class TSnRecoChanOffsets : public TSnRecoResult {
 public:
   static const UInt_t  kChCmbs;    // number of channel combinations

 private:
   TArrayD      fDts; // channel time offests
   TArrayD      fErr; // channel time offests
   Double_t     fLik; // the fit quality paramter (chi2, log likelihood, etc)
   
 public:
   TSnRecoChanOffsets() : fLik(0) {}
   TSnRecoChanOffsets(const Char_t* name, const Char_t* minimizerTag);
   TSnRecoChanOffsets(const Char_t* name, const Char_t* minimizerTag,
                      const Double_t* os, const Double_t* err=0,
                      const Double_t qual=0);
   virtual ~TSnRecoChanOffsets() {}

   virtual void CopyFrom(const TSnRecoResult& x) {
      // this will throw a std::bad_cast if x is not a TSnRecoChanOffsets
      *this = dynamic_cast<const TSnRecoChanOffsets&>(x);
   }
   
   static
   UInt_t         IndexFor(const UChar_t up, const Char_t lo);

   Double_t       GetFitQual() const { return fLik; }
   const TArrayD& GetChanOffsets() const { return fDts; }
   Double_t       GetChanOffset(const UChar_t c1, const UChar_t c2) const;
   const TArrayD& GetChanOffsetErrors() const { return fErr; }
   Double_t       GetChanOffsetError(const UChar_t c1, const UChar_t c2) const;
   
   void           SetFitQual(const Double_t q) { fLik = q; }
   void           SetChanOffset(const UChar_t c1, const UChar_t c2,
                                const Double_t o);
   void           SetChanOffset(const UInt_t idx, const Double_t o)
      { fDts.SetAt(o,idx); }
   void           SetChanOffsets(const Double_t* os) {
      // set all the channel offsets.
      //
      // the array "os" is assumed to be long enough
      // but this is NOT checked!
      fDts.Set(kChCmbs, os);
   }
   void           SetChanOffsetError(const UChar_t c1, const UChar_t c2,
                                     const Double_t e);
   void           SetChanOffsetError(const UInt_t idx, const Double_t e)
      { fErr.SetAt(e,idx); }
   void           SetChanOffsetErrors(const Double_t* es) {
      // set all the channel offsets.
      //
      // the array "os" is assumed to be long enough
      // but this is NOT checked!
      fErr.Set(kChCmbs, es);
   }
   
   virtual void Print(Option_t* option = "") const;
   
   
   ClassDef(TSnRecoChanOffsets, 1); // reconstructed channel time offsets among combinations of channels in an event
};

#endif // SNS_TSnRecoChanOffsets
