#ifndef SNS_TSnEventHeader
#define SNS_TSnEventHeader

#include <TObject.h>
#include <TTimeStamp.h>

#include "TSnTrgInfo.h"

class TSnEventHeader : public TObject {
 private:
   TTimeStamp     fTime;            // event time
   UInt_t         fNum;             // event number
   UInt_t         fTrgNum;          // trigger number
   Int_t          fDTms;            // delta(T) since last event in ms
   TSnTrgInfo     fTrgInfo;         // trigger bit word

 public:
   TSnEventHeader() : fNum(0), fTrgNum(0), fDTms(0) {}
   virtual ~TSnEventHeader() {}
   
   virtual void      Print(Option_t* option="") const;

   time_t            GetUnixTime() const { return fTime.GetSec(); }
   const TTimeStamp& GetTime() const { return fTime; }
         TTimeStamp& GetTime()       { return fTime; }
   UInt_t            GetEvtNum() const { return fNum; }
   UInt_t            GetTrgNum() const { return fTrgNum; }
   Int_t             GetDTms() const { return fDTms; }
   const TSnTrgInfo& GetTrgInfo() const { return fTrgInfo; }
   
   Bool_t            IsThermal() const { return fTrgInfo.IsThermal(); }
   Bool_t            IsForced()  const { return fTrgInfo.IsForced(); }
   Bool_t            IsAdcToMBFlagged() const
      { return fTrgInfo.IsAdcToMBFlagged(); }
   
   void     SetTime(const TTimeStamp& b) { fTime = b; }
   void     SetTime(const UInt_t tloc,
                    const Bool_t isUTC=kTRUE,
                    const Int_t secOffset=0,
                    const Bool_t dosDate=kFALSE)
      { fTime.Set(tloc, isUTC, secOffset, dosDate); }
   void     SetEvtNum(const UInt_t e) { fNum = e; }
   void     SetTrgNum(const UInt_t t) { fTrgNum = t; }
   void     SetDTms(const Int_t dt) { fDTms = dt; }
   void     SetTrgInfo(const TSnTrgInfo& t) { fTrgInfo = t; }
   void     SetTrgInfo(const UShort_t t) { fTrgInfo.SetTrgBits(t); }
   
   
   ClassDef(TSnEventHeader, 1); // store event number, time, etc
};

#endif // SNS_TSnEventHeader
