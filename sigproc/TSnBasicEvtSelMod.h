#ifndef SNS_TSnBasicEvtSelMod
#define SNS_TSnBasicEvtSelMod

#include <TString.h>

#include "TAModule.h"
#include "TSnTrgInfo.h"
class TSnEventHeader;
class TSnRawWaveform;

class TSnBasicEvtSelMod : public TAModule {
 private:
   TString      fHdrBrNm; // name of header branch (defaults to TSnRawTreeMaker::kEHdBrNm)
   TString      fEWvBrNm; // name of event waveform branch (defaults to TSnRawTreeMaker::kEWvBrNm)
   TSnTrgInfo   fTbits; //  desired trigger bits. keep events have *one or more* of these bits set. (default: NO trig bits => NO events, so the user must explicitly set the desired triggers)
   TSnTrgInfo   fRbits; //  undesired trigger bits. reject events that have *any* of these bits set. (default: reject kAdcToMBflag)
   Bool_t       fDoCRC; //  whether or not to check the CRC's (default: true)

   // current event variables
   TSnEventHeader* fHdr; //! event header
   TSnRawWaveform* fRaw; //! raw waveform

 protected:
   virtual void SlaveBegin();
   virtual void Process();
   
 public:
   TSnBasicEvtSelMod() : fDoCRC(kTRUE), fHdr(0), fRaw(0) {}
   TSnBasicEvtSelMod(const Char_t* name,
                     const UShort_t bits=0,
                     const UShort_t rejBits=TSnTrgInfo::kAdcToMBflag);
   virtual ~TSnBasicEvtSelMod() {}
   
   const TSnTrgInfo& GetTrgBits() const { return fTbits; }
         TSnTrgInfo& GetTrgBits()       { return fTbits; }
   const TSnTrgInfo& GetRejectTrgBits() const { return fRbits; }
         TSnTrgInfo& GetRejectTrgBits()       { return fRbits; }
   Bool_t            IsCheckingCRC() const { return fDoCRC; }
   TString           GetHeaderBranchName() const { return fHdrBrNm; }
   TString           GetEvtWvfmBranchName() const { return fEWvBrNm; }
   
   void         SetTrgBits(const UShort_t b)    { fTbits.SetTrgBits(b); }
   void         SetTrgBits(const TSnTrgInfo& t) { fTbits = t; }
   void         SetRejectTrgBits(const UShort_t b)    { fRbits.SetTrgBits(b); }
   void         SetRejectTrgBits(const TSnTrgInfo& t) { fRbits = t; }
   void         SetCheckCRC(const Bool_t c)     { fDoCRC = c; }
   void         SetHeaderBranchName(const Char_t* n) { fHdrBrNm = n; }
   void         SetEvtWvfmBranchName(const Char_t* n) { fEWvBrNm = n; }
   
   ClassDef(TSnBasicEvtSelMod, 4); // select events by checking CRC and trigger bits
};

#endif // SNS_TSnBasicEvtSelMod
