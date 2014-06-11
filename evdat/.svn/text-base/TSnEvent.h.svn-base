#ifndef SNS_TSnEvent
#define SNS_TSnEvent

#include <TObject.h>

#include "TSnEventHeader.h"
#include "TSnRawWaveform.h"

class TSnEventWhisperer;
class TSnIOEventFrame;


class TSnEvent : public TObject {
 private:
   TSnEventHeader fHeader;          // the event header
   TSnRawWaveform fRawWvfm;         // the raw data
   
 public:
   TSnEvent() {}
   virtual ~TSnEvent() {}
   
   virtual void      Print(Option_t* option="") const;

         TSnEventHeader& GetHeader()       { return fHeader; }
   const TSnEventHeader& GetHeader() const { return fHeader; }
   const TTimeStamp& GetTime() const { return fHeader.GetTime(); }
   UInt_t            GetEvtNum() const { return fHeader.GetEvtNum(); }
   UInt_t            GetTrgNum() const { return fHeader.GetTrgNum(); }
   Int_t             GetDTms() const { return fHeader.GetDTms(); }
   const TSnTrgInfo& GetTrgInfo() const { return fHeader.GetTrgInfo(); }
         TSnRawWaveform& GetRawWaveform()       { return fRawWvfm; }
   const TSnRawWaveform& GetRawWaveform() const { return fRawWvfm; }
   UInt_t            GetStationCRC() const { return fRawWvfm.GetStationCRC(); }
   UInt_t            GetCRC() const { return fRawWvfm.GetCRC(); }
   const UShort_t*   GetWvData() const { return fRawWvfm.GetWvData(); }
   const UShort_t*   GetWvData(const UChar_t ch) const
      { return fRawWvfm.GetWvData(ch); }
   UShort_t          GetWvData(const UChar_t ch, const UChar_t smp) const
      { return fRawWvfm.GetWvData(ch, smp); }

   Bool_t            IsThermal() const { return fHeader.IsThermal(); }
   Bool_t            IsForced()  const { return fHeader.IsForced(); }

   virtual const char* GetName() const;

   void     SetHeader(const TSnEventHeader& h) { fHeader = h; }
   void     SetTime(const TTimeStamp& b) { fHeader.SetTime(b); }
   void     SetTime(const UInt_t tloc,
                    const Bool_t isUTC=kTRUE,
                    const Int_t secOffset=0,
                    const Bool_t dosDate=kFALSE)
      { fHeader.SetTime(tloc, isUTC, secOffset, dosDate); }
   void     SetEvtNum(const UInt_t e) { fHeader.SetEvtNum(e); }
   void     SetTrgNum(const UInt_t t) { fHeader.SetTrgNum(t); }
   void     SetDTms(const Int_t dt) { fHeader.SetDTms(dt); }
   void     SetTrgInfo(const TSnTrgInfo& t) { fHeader.SetTrgInfo(t); }
   void     SetTrgInfo(const UShort_t t) { fHeader.SetTrgInfo(t); }
   void     SetRawWaveform(const TSnRawWaveform& w) { fRawWvfm = w; }
   void     SetStationCRC(const UInt_t c) { fRawWvfm.SetStationCRC(c); }
   void     SetWvData(const UShort_t* d) { fRawWvfm.SetWvData(d); }
   void     SetWvData(const UChar_t ch, const UShort_t* d)
      { fRawWvfm.SetWvData(ch, d); }
   void     SetWvData(const UChar_t ch, const UChar_t smp, const UShort_t d)
      { fRawWvfm.SetWvData(ch, smp, d); }
   
   void     CalcCRC() const { fRawWvfm.CalcCRC(); }
   
   
   friend class TSnEventWhisperer;
   
   ClassDef(TSnEvent, 2); // an event - this container class is used only for convenience by the station I/O software
};


class TSnEventWhisperer {
   // allow non-const access to the data array only for select classes

   static
   UShort_t* GetWvData(TSnEvent& evt) { 
      return TSnRawWaveformWhisperer::GetWvData(evt.fRawWvfm);
   }
   
   static
   const UShort_t* GetWvData(const TSnEvent& evt) { return evt.GetWvData(); }
   
   // the IO class reading from raw data gets direct access
   friend class TSnIOEventFrame;
};

#endif // SNS_TSnEvent
