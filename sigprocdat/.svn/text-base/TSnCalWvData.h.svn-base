#ifndef SNS_TSnCalWvData
#define SNS_TSnCalWvData

#include <TNamed.h>
#include <TArrayF.h>
class TGraph;

#include "NSnConstants.h"

class TSnCalWvData : public TNamed {
 private:
   Float_t     fData[NSnConstants::kTotSamps];  // the calibrated data. use C array so it can be processed by TTree::Draw
   Float_t     fError[NSnConstants::kTotSamps]; // error on the calibrated value. use C array so it can be processed by TTree::Draw
   
 public:
   TSnCalWvData() {
      ClearAll();
   }
   TSnCalWvData(const Char_t* name, const Char_t* title) :
      TNamed(name, title) {
      ClearAll();
   }
   TSnCalWvData(const TSnCalWvData& d) :
      TNamed(d) {
      CopyDataFrom(d);
   }
   virtual ~TSnCalWvData() {}
   
   void ClearAll() {
      memset(fData, 0, NSnConstants::kTotSamps*sizeof(Float_t));
      memset(fError, 0, NSnConstants::kTotSamps*sizeof(Float_t));
   }
   
   void CopyDataFrom(const TSnCalWvData& d) {
      memmove(fData, d.fData, NSnConstants::kTotSamps * sizeof(Float_t));
      memmove(fError, d.fError, NSnConstants::kTotSamps * sizeof(Float_t));
   }
   
   static
   void Swap(TSnCalWvData& a, TSnCalWvData& b);
   
   TSnCalWvData& operator=(TSnCalWvData o) {
      // assign using efficient copy from pass by value
      Swap(*this, o);
      return *this;
   }
   
   const Float_t* GetData() const { return fData; }
         Float_t* GetData()       { return fData; }
   const Float_t* GetData(const UChar_t ch) const;
         Float_t* GetData(const UChar_t ch);
   Float_t        GetData(const UChar_t ch, const UChar_t s) const;

   const Float_t* GetError() const { return fError; }
         Float_t* GetError()       { return fError; }
   const Float_t* GetError(const UChar_t ch) const;
         Float_t* GetError(const UChar_t ch);
   Float_t        GetError(const UChar_t ch, const UChar_t s) const;
   
   void           SetData(const UChar_t ch, const UChar_t s, const Float_t v);
   
   void           SetError(const UChar_t ch, const UChar_t s, const Float_t v);
   
   virtual TGraph* NewGraphForChan(const UChar_t  ch,
                                   const Bool_t   vsTime=kFALSE,
                                   const Bool_t   withErrors=kTRUE,
                                         Double_t shift=0,
                                   const Bool_t   doWrap=kTRUE) const;
   
   ClassDef(TSnCalWvData, 2); // calibrated waveform data
};

#endif // SNS_TSnCalWvData
