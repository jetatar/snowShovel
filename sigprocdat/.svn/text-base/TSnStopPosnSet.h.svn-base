#ifndef SNS_TSnStopPosnSet
#define SNS_TSnStopPosnSet

#include <TNamed.h>

#include "NSnConstants.h"
class TSnCalWvData;

class TSnStopPosnSet : public TNamed {
 public:
   typedef  UChar_t  SmpNum_t;
   
 private:
   SmpNum_t fFirst[NSnConstants::kNchans]; // the first sample in the stops
   SmpNum_t fWidth;                        // width of each stop in num samples
   Float_t  fQual;                         // quality/confidence parameter for stop fit
   
   void ClearArrays() {
      memset(fFirst, 0, NSnConstants::kNchans * sizeof(SmpNum_t));
   }

   Bool_t AssertBounds(const UChar_t ch) const;
   
 public:
   TSnStopPosnSet() : fWidth(0), fQual(0) { ClearArrays(); }
   TSnStopPosnSet(const Char_t* name, const Char_t* title,
                  const SmpNum_t width=6) :
      TNamed(name, title), fWidth(width), fQual(0) { ClearArrays(); }
   TSnStopPosnSet(const TSnStopPosnSet& x) : 
      TNamed(x), fWidth(x.fWidth), fQual(x.fQual) {
      memmove(fFirst, x.fFirst, NSnConstants::kNchans * sizeof(SmpNum_t));
   }
   virtual ~TSnStopPosnSet() {}
   
   static
   void Swap(TSnStopPosnSet& a, TSnStopPosnSet& b);
   
   TSnStopPosnSet& operator=(TSnStopPosnSet o) {
      // assign using efficient copy from pass by value
      Swap(*this, o);
      return *this;
   }

   virtual void Copy(TObject& o) const;
   
   const SmpNum_t* GetFirstSamples() const { return fFirst; }
         SmpNum_t* GetFirstSamples()       { return fFirst; }
   SmpNum_t        GetFirstSample(const UChar_t ch) const {
      if (ch<NSnConstants::kNchans) {
         return fFirst[ch];
      } else {
         AssertBounds(ch);
         return NSnConstants::kNsamps;
      }
   }
   SmpNum_t        GetWaveBeginSample(const UChar_t ch) const {
      SmpNum_t s = GetFirstSample(ch) + fWidth;
      NSnConstants::WrapSampleNum(s);
      return s;
   }
   SmpNum_t        GetStopWidth() const { return fWidth; }
   Float_t         GetStopQuality() const { return fQual; }
   
   TSnCalWvData*   NewShiftedData(const TSnCalWvData& rw,
                                  const Char_t* name,
                                  const Char_t* title) const;
   
   void            SetFirstSamples(const SmpNum_t* x) {
      // no check is made that the array 'x' is long enough!
      memmove(fFirst, x, NSnConstants::kNchans * sizeof(SmpNum_t));
   }
   void            SetFirstSample(const UChar_t ch, const SmpNum_t x) {
      if (ch<NSnConstants::kNchans) {
         fFirst[ch] = x;
      } else {
         AssertBounds(ch);
      }
   }
   void            SetStopWidth(const SmpNum_t w) { fWidth = w; }
   void            SetStopQuality(const Float_t q) { fQual = q; }

   
   ClassDef(TSnStopPosnSet, 1); // store a set of stop positions, per card
};



#endif // SNS_TSnStopPosnSet
