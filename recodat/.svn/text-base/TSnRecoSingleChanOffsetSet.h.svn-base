#ifndef SNS_TSnRecoSingleChanOffsetSet
#define SNS_TSnRecoSingleChanOffsetSet

class TObjArray;

#include "TSnRecoResult.h"
class TSnSaveRecoResultMod;
class TSnRecoSingleChanOffset;

class TSnRecoSingleChanOffsetSet : public TSnRecoResult {
 private:
   TObjArray*   fSet; //-> the collection of TSnRecoSingleChanOffset objs (owner)
   
 public:
   TSnRecoSingleChanOffsetSet() : fSet(0) {}
   TSnRecoSingleChanOffsetSet(const Char_t* name, const Char_t* minimizerTag);
   TSnRecoSingleChanOffsetSet(const TSnRecoSingleChanOffsetSet& o);
   virtual ~TSnRecoSingleChanOffsetSet();
   
   static
   void Swap(TSnRecoSingleChanOffsetSet& a, TSnRecoSingleChanOffsetSet& b);
   
   TSnRecoSingleChanOffsetSet& operator=(TSnRecoSingleChanOffsetSet x) {
      // assign using efficient copy from pass by value
      Swap(*this, x);
      return *this;
   }

   virtual void CopyFrom(const TSnRecoResult& x) {
      // this will throw a std::bad_cast if x is not a TSnRecoSingleChanOffsetSet
      *this = dynamic_cast<const TSnRecoSingleChanOffsetSet&>(x);
   }

   const TObjArray* GetAllChanOffsets() const { return fSet; }
   // intentionally no non-const getter for the array
   const TSnRecoSingleChanOffset* GetOffsetsOfChan(const UChar_t ch) const;
         TSnRecoSingleChanOffset* GetOffsetsOfChan(const UChar_t ch);
   const TSnRecoSingleChanOffset& GetOffsetsOfChanSafe(const UChar_t ch) const;
         TSnRecoSingleChanOffset& GetOffsetsOfChanSafe(const UChar_t ch);
   Double_t                       GetTimeOffset(const UChar_t ch) const;
   Double_t                       GetOffsetError(const UChar_t ch) const;
   Double_t                       GetFitQual(const UChar_t ch) const;
   
   
   void  SetOffsetsOfChan(const UChar_t ch,
                          const TSnRecoSingleChanOffset& co);
   // intentionally no setter for the entire array


   virtual void Print(Option_t* option = "") const;
   
   
   friend class TSnRecoSingleChanOffsetSetWhisperer;
   
   ClassDef(TSnRecoSingleChanOffsetSet, 1); // a collection of single channel offset reco results
};


class TSnRecoSingleChanOffsetSetWhisperer {
 private:
   static
   TObjArray* GetAllChanOffsets(TSnRecoSingleChanOffsetSet& s) { return s.fSet; }
   
 public:
   friend class TSnSaveRecoResultMod;
};

#endif // SNS_TSnRecoSingleChanOffsetSet
