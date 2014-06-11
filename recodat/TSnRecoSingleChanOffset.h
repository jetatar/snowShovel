#ifndef SNS_TSnRecoSingleChanOffset
#define SNS_TSnRecoSingleChanOffset

#include "TSnRecoResult.h"

class TSnRecoSingleChanOffset : public TSnRecoResult {
 private:
   Double_t     fOff; // channel time offest
   Double_t     fErr; // channel time offest error
   Double_t     fLik; // the fit quality paramter (chi2, log likelihood, etc)
   
 public:
   TSnRecoSingleChanOffset() : fOff(0), fErr(0), fLik(0) {}
   TSnRecoSingleChanOffset(const Char_t* name, const Char_t* minimizerTag) :
      TSnRecoResult(name, minimizerTag),
      fOff(0), fErr(0), fLik(0) {}
   TSnRecoSingleChanOffset(const Char_t* name, const Char_t* minimizerTag,
                           const Double_t off, const Double_t err, 
                           const Double_t qual) :
      TSnRecoResult(name, minimizerTag),
      fOff(off), fErr(err), fLik(qual) {}
   virtual ~TSnRecoSingleChanOffset() {}
   
   virtual void CopyFrom(const TSnRecoResult& x) {
      // this will throw a std::bad_cast if x is not a TSnRecoSingleChanOffset
      *this = dynamic_cast<const TSnRecoSingleChanOffset&>(x);
   }
   
   Double_t       GetFitQual() const { return fLik; }
   Double_t       GetTimeOffset() const { return fOff; }
   Double_t       GetOffsetError() const { return fErr; }

   void           SetFitQual(const Double_t q) { fLik = q; }
   void           SetTimeOffset(const Double_t o) { fOff = o; }
   void           SetOffsetError(const Double_t e) { fErr = e; }

   virtual void Print(Option_t* option = "") const;
   
   ClassDef(TSnRecoSingleChanOffset, 1); // one time offset per channel (useful for offets between events)
};

#endif // SNS_TSnRecoSingleChanOffset
