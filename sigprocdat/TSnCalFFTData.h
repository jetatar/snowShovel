#ifndef SNS_TSnCalFFTData
#define SNS_TSnCalFFTData

#include <TNamed.h>
class TGraph;
class TArrayF;

#include "NSnConstants.h"
class TSnCalWvData;


class TSnCalFFTData : public TNamed {
 public:
   static const UChar_t kFftPts = 1 + (NSnConstants::kNsamps >> 1); // Nsamps/2
   
 private:
   Float_t         fFFT[NSnConstants::kNchans][TSnCalFFTData::kFftPts]; // the FFT. use a C array so it can be processed by TTree::Draw
   Float_t         fFreq[TSnCalFFTData::kFftPts];                         // the frequencies on one channel. use a C array so it can be processed by TTree::Draw
   
   TGraph*         fCache[NSnConstants::kNchans]; //! cache the graphs (not streamed ==> not stored in a TTree)
   

   void       AssertPoint(const UChar_t point) const;
   void       AssertChan(const UChar_t ch) const;
   void       AssertRefPower(const Float_t p0) const;

   void       BuildFFTGraph(const UChar_t ch, TArrayF& fft);
   static
   Float_t    FrommVToPower(const Float_t v);
   static
   Float_t    FromPowerToDb(const Float_t v, const Float_t p0);
   TGraph*    NewFFTGraphPowOrDb(const UChar_t ch,
                                 const Float_t p0) const;
   
 public:
   TSnCalFFTData() {
      memset(fCache, 0, NSnConstants::kNchans * sizeof(TGraph*));
      ClearAll();
   }
   TSnCalFFTData(const Char_t* name, const Char_t* title) :
      TNamed(name, title) {
      memset(fCache, 0, NSnConstants::kNchans * sizeof(TGraph*));
      ClearAll();
   }
   TSnCalFFTData(const Char_t* name, const Char_t* title,
                 const TSnCalWvData& d) :
      TNamed(name, title) {
      memset(fCache, 0, NSnConstants::kNchans * sizeof(TGraph*));
      SetFFTFor(d);
   }
   TSnCalFFTData(const TSnCalFFTData& o) :
      TNamed(o) {
      memset(fCache, 0, NSnConstants::kNchans * sizeof(TGraph*));
      CopyDataFrom(o);
   }
   virtual ~TSnCalFFTData();
   
   void ClearCache();
   void CopyDataFrom(const TSnCalFFTData& o);
   
   void ClearAll() {
      memset(fFFT, 0, NSnConstants::kNchans * kFftPts * sizeof(Float_t));
      memset(fFreq, 0, kFftPts * sizeof(Float_t));
      ClearCache();
   }
   
   static
   void Swap(TSnCalFFTData& a, TSnCalFFTData& b);
   
   TSnCalFFTData& operator=(TSnCalFFTData o) {
      // assign using efficient copy from pass by value
      Swap(*this, o);
      return *this;
   }
   
   // "raw" getters -- probably not very useful
   const Float_t* GetFFTData(const UChar_t ch) const;
         Float_t* GetFFTData(const UChar_t ch);
   const Float_t* GetFreqData() const { return fFreq; }
         Float_t* GetFreqData()       { return fFreq; }
   
   // get values one by one
   static
   UShort_t       GetNumPoints() { return kFftPts; }
   Float_t        GetFFTData(const UChar_t ch, const UChar_t point) const;
   Float_t        GetFFTInPower(const UChar_t ch, const UChar_t point) const;
   Float_t        GetFFTInDb(const UChar_t ch, const UChar_t point,
                             const Float_t p0) const;
   Float_t        GetFreqData(const UChar_t point) const;
   
   // higher level getters -- more overhead, but probably more useful
   const TGraph&  GetFFTGraph(const UChar_t ch) const;
   TGraph*        NewFFTGraph(const UChar_t ch) const;
   TGraph*        NewFFTGraphInPower(const UChar_t ch) const;
   TGraph*        NewFFTGraphInDb(const UChar_t ch, const Float_t p0) const;
   
   // only way to change the FFT is to pass in new data
   void           SetFFTFor(const TSnCalWvData& dat);
   
   ClassDef(TSnCalFFTData, 1); // store FFT data (from calib data) for all channels
};

#endif // SNS_TSnCalFFTData
