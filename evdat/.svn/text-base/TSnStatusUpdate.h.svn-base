#ifndef SNS_TSnStatusUpdate
#define SNS_TSnStatusUpdate

#include <TObject.h>
#include <TString.h>
#include <TTimeStamp.h>

class TSnStatusUpdate : public TObject {
 public:
   struct TObsoletePars {
      // old parameters we don't usually want to stream, but we may need
      // for backwards compatibility
      UInt_t            fNfiles;
      Float_t            fTotKB;
      
      TObsoletePars() : fNfiles(0), fTotKB(0) {}
      virtual ~TObsoletePars() {}
      ClassDef(TSnStatusUpdate::TObsoletePars, 1); // status back compat layer
   };
   
 private:
   ULong64_t         fMacAdr;
   TString           fConfLabel;
   UInt_t            fRun;
   TTimeStamp        fTime;
   UChar_t           fWvLoseLSB;
   UChar_t           fWvLoseMSB;
   UShort_t          fWvBaseline;
   UShort_t          fSeq;
   Float_t           fThmRate;
   Float_t           fEvtRate;
   Float_t           fFreeMB;
   UChar_t           fWDreset;
   
   TSnStatusUpdate::TObsoletePars fObs;    // status back compat layer
   Bool_t                         fHasObs; // if back compat vars set

   // intentionally private
   const TSnStatusUpdate::TObsoletePars& GetObsPars() const;
      
 public:
   TSnStatusUpdate() :
      fMacAdr(0), fRun(0), fWvLoseLSB(0), fWvLoseMSB(0), fWvBaseline(0),
      fSeq(0), fThmRate(0), fEvtRate(0), fFreeMB(0), fWDreset(0),
      fHasObs(kFALSE) {}
   virtual ~TSnStatusUpdate() { }
   

   virtual void      Print(Option_t* option="") const;

   ULong64_t         GetMacAddress() const { return fMacAdr; }
   const Char_t*     GetMacAdrAsStr() const;
   UInt_t            GetConfLabelLen() const { return fConfLabel.Length(); }
   const Char_t*     GetConfLabel() const { return fConfLabel.Data(); }
   const TString&    GetConfLabelStr() const { return fConfLabel; }
   UInt_t            GetRunNum() const { return fRun; }
   const TTimeStamp& GetStatusTime() const { return fTime; }
   UChar_t           GetWvLoseLSB() const { return fWvLoseLSB; }
   UChar_t           GetWvLoseMSB() const { return fWvLoseMSB; }
   UShort_t          GetWvBaseline() const { return fWvBaseline; }
   UShort_t          GetSeqNum() const { return fSeq; }
   Float_t           GetThmRate() const { return fThmRate; }
   Float_t           GetEvtRate() const { return fEvtRate; }
   Float_t           GetFreeMB() const { return fFreeMB; }
   UChar_t           GetWDReset() const { return fWDreset; }
   Bool_t            DidWatchdogReset() const { return fWDreset!=0; } 
   
   void     SetMacAddress(const ULong64_t m) { fMacAdr = m; }
   void     SetConfLabel(const Char_t* l) { fConfLabel = l; }
   void     SetRunNum(const UInt_t r) { fRun = r; }
   void     SetStatusTime(const TTimeStamp& t) { fTime = t; }
   void     SetStatusTime(const UInt_t tloc,
                          const Bool_t isUTC=kTRUE,
                          const Int_t secOffset=0,
                          const Bool_t dosDate=kFALSE)
      { fTime.Set(tloc, isUTC, secOffset, dosDate); }
   void     SetWvLoseLSB(const UChar_t l) { fWvLoseLSB = l; }
   void     SetWvLoseMSB(const UChar_t m) { fWvLoseMSB = m; }
   void     SetWvBaseline(const UShort_t b) { fWvBaseline = b; }
   void     SetSeqNum(const UShort_t s) { fSeq = s; }
   void     SetThmRate(const Float_t r) { fThmRate = r; }
   void     SetEvtRate(const Float_t r) { fEvtRate = r; }
   void     SetFreeMB(const Float_t m) { fFreeMB = m; }
   void     SetWDReset(const UChar_t r) { fWDreset = r; }

   virtual
   void Copy(TObject& object) const;
   
   Bool_t   HasObsParams() const { return fHasObs; }
   UInt_t   GetObsNfiles() const { return GetObsPars().fNfiles; }
   Float_t  GetObsTotKB() const  { return GetObsPars().fTotKB; }
   void     SetObsNfiles(const UInt_t n);
   void     SetObsTotKB(const Float_t t);
   
   ClassDef(TSnStatusUpdate, 8); // a status update
};

#endif // SNS_TSnStatusUpdate
