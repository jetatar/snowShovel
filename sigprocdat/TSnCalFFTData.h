#ifndef SNS_TSnCalFFTData
#define SNS_TSnCalFFTData

#include <TNamed.h>
class TGraph;
class TArrayF;

#include "NSnConstants.h"
class TSnCalWvData;

#include <utility>

class TSnCalFFTData : public TNamed {
 public:
   static const UChar_t kFftPts = 1 + (NSnConstants::kNsamps >> 1); // Nsamps/2

   typedef std::pair<Float_t,Float_t> EvenOddPair_t;
   
 private:
   Float_t         fFFT[NSnConstants::kNchans][NSnConstants::kNsamps]; // the FFT. use a C array so it can be processed by TTree::Draw
   Float_t         fFreq[TSnCalFFTData::kFftPts];                         // the frequencies on one channel. use a C array so it can be processed by TTree::Draw
   Float_t         fToPower; // used to convert the units of the original data to sqrt(power) units. (default: mV^2 to V^2 = 1e-6)
   
   mutable TGraph* fCache[NSnConstants::kNchans]; //! cache the graphs (not streamed ==> not stored in a TTree)
   

   void       AssertPoint(const UChar_t point) const;
   void       AssertChan(const UChar_t ch) const;
   void       AssertSamp(const UChar_t sm) const;
   void       AssertRefPower(const Float_t p0) const;

   void       BuildFFTGraph(const UChar_t ch) const;

   Float_t    FrommVToPower(const Float_t v) const;
   static
   Float_t    FromPowerToDb(const Float_t v, const Float_t p0);
   TGraph*    NewFFTGraphPowOrDb(const UChar_t ch,
                                 const Float_t p0) const;
   
 public:
   TSnCalFFTData() : fToPower(1e-6) {
      memset(fCache, 0, NSnConstants::kNchans * sizeof(TGraph*));
      ClearAll();
   }
   TSnCalFFTData(const Char_t* name, const Char_t* title) :
      TNamed(name, title),
      fToPower(1e-6) {
      memset(fCache, 0, NSnConstants::kNchans * sizeof(TGraph*));
      ClearAll();
   }
   TSnCalFFTData(const Char_t* name, const Char_t* title,
                 const TSnCalWvData& d) :
      TNamed(name, title),
      fToPower(1e-6) {
      memset(fCache, 0, NSnConstants::kNchans * sizeof(TGraph*));
      SetFFTFor(d);
   }
   TSnCalFFTData(const TSnCalFFTData& o) :
      TNamed(o),
      fToPower(1e-6) {
      memset(fCache, 0, NSnConstants::kNchans * sizeof(TGraph*));
      CopyDataFrom(o);
   }
   virtual ~TSnCalFFTData();
   
   void ClearCache();
   void CopyDataFrom(const TSnCalFFTData& o);
   void CopyDataWithoutCacheFrom(const TSnCalFFTData& o);
   
   void ClearAll() {
      memset(fFFT, 0,
             NSnConstants::kNchans * NSnConstants::kNsamps * sizeof(Float_t));
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

   Float_t        GetConvToPowerFactor() const { return fToPower; }
   
   // "raw" getters -- probably not very useful
   const Float_t* GetFFTRawCoefs(const UChar_t ch) const;
         Float_t* GetFFTRawCoefs(const UChar_t ch);
   const Float_t* GetFreqData() const { return fFreq; }
         Float_t* GetFreqData()       { return fFreq; }
   
   // get values one by one
   static
   UShort_t       GetNumFrequencies() { return kFftPts; }
   Float_t        GetFFTMag(const UChar_t ch, const UChar_t frqBin) const;
   Float_t        GetFFTMagInPower(const UChar_t ch, const UChar_t frqBin) const;
   Float_t        GetFFTMagInDb(const UChar_t ch, const UChar_t frqBin,
                                const Float_t refPwr=1.0) const;
   Float_t        GetFrequency(const UChar_t frqBin) const;
   Float_t        GetPhase(const UChar_t ch, const UChar_t frqBin) const;
   Float_t        GetFFTCoeff(const UChar_t ch, const UChar_t sampleBin) const;
   void           GetFFTCoeffPair(const UChar_t ch, const UChar_t frqBin,
                                  Float_t& evenCoeff, Float_t& oddCoeff) const;
   EvenOddPair_t  GetFFTCoeffPair(const UChar_t ch, const UChar_t frqBin) const;
   
   // higher level getters -- more overhead, but probably more useful
   const TGraph&  GetFFTGraph(const UChar_t ch) const;
   TGraph*        NewFFTGraph(const UChar_t ch) const;
   TGraph*        NewFFTGraphInPower(const UChar_t ch) const;
   TGraph*        NewFFTGraphInDb(const UChar_t ch, const Float_t p0) const;
   TSnCalWvData*  NewDataFromInverseFFT() const;
   
   void           SetFFTFor(const TSnCalWvData& dat);
   void           SetFFTCoeff(const UChar_t ch, const UChar_t sampleBin,
                              const Float_t coeff);
   void           SetFFTCoeffPair(const UChar_t ch, const UChar_t frqBin,
                                  const Float_t evenCoeff, 
                                  const Float_t oddCoeff);
   void           SetFFTCoeffPair(const UChar_t ch, const UChar_t frqBin,
                                  const TSnCalFFTData::EvenOddPair_t& coeffs);

   void           SetConvToPowerFactor(const Float_t f) { fToPower = f; }
   
   ClassDef(TSnCalFFTData, 2); // store FFT data (from calib data) for all channels
};

#endif // SNS_TSnCalFFTData
