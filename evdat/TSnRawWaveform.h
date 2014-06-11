#ifndef SNS_TSnRawWaveform
#define SNS_TSnRawWaveform

#include <TObject.h>

#include "NSnConstants.h"

class TSnRawWaveformWhisperer;

class TSnRawWaveform : public TObject {
 private:
   UShort_t       fData[NSnConstants::kTotSamps]; // the waveform data. use C array so it can be processed by TTree::Draw
   UInt_t         fStationCRC;      // 32-bit CRC of the waveform data calc by the station
   mutable UInt_t fCRC;             // 32-bit CRC of the waveform data

   UShort_t*   GetWvData(const UChar_t ch) {
      // intentionally private
      return const_cast<UShort_t*>(
         const_cast<const TSnRawWaveform*>(this)->GetWvData(ch));
   }
   
 public:
   TSnRawWaveform() : fStationCRC(0), fCRC(0) {
      memset(fData, 0, NSnConstants::kTotSamps*sizeof(UShort_t));
   }
   virtual ~TSnRawWaveform() {}

   virtual void      Print(Option_t* option="") const;
   
   static
   UShort_t          GetUncheckedIndex(const UChar_t ch, const UChar_t smp) {
      return (ch*NSnConstants::kNsamps)+smp;
   }
   static
   UShort_t          GetIndexOf(const UChar_t ch, const UChar_t smp);
   
   UInt_t            GetStationCRC() const { return fStationCRC; }
   UInt_t            GetCRC() const { return fCRC; }
   const UShort_t*   GetWvData() const { return fData; }
   const UShort_t*   GetWvData(const UChar_t ch) const {
      return fData+(GetIndexOf(ch,0));
   }
   UShort_t          GetWvData(const UChar_t ch, const UChar_t smp) const {
      return fData[GetIndexOf(ch,smp)];
   }
   
   void     SetStationCRC(const UInt_t c) { fStationCRC = c; }
   void     SetWvData(const UShort_t* d) {
      memmove(fData, d, NSnConstants::kTotSamps*sizeof(UShort_t));
   }
   void     SetWvData(const UChar_t ch, const UShort_t* d) {
      memmove(GetWvData(ch), d, NSnConstants::kNsamps*sizeof(UShort_t));
   }
   void     SetWvData(const UChar_t ch, const UChar_t smp, const UShort_t d) {
      fData[GetIndexOf(ch,smp)] = d;
   }
   
   void     CalcCRC() const;
   
   
   friend class TSnRawWaveformWhisperer;
   
   ClassDef(TSnRawWaveform,1); // raw waveform sample data
};


class TSnRawWaveformWhisperer {
   // allow non-const access to the data array only for select classes
   
   static
   UShort_t* GetWvData(TSnRawWaveform& w) { return w.fData; }
   
   // allow TSnEventWhisperer access
   friend class TSnEventWhisperer;
};

#endif // SNS_TSnRawWaveform
