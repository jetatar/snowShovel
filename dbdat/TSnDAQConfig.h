#ifndef SNS_TSnDAQConfig
#define SNS_TSnDAQConfig

#include <TNamed.h>
#include <TString.h>
#include <TTimeStamp.h>

#include "TSnDacSet.h"
#include "TSnPlaSet.h"
#include "TSnAmpOnSet.h"
#include "TSnCommWinSet.h"
#include "TSnTriggerSetup.h"

#include <vector>

class TSnDAQConfig : public TNamed {
 public:
   static const Int_t      kMaxLblLen;
   
   enum EDatPackBit {
      // must match the bits on the mbed!
      kSDcard = BIT(0),
      kIrid   = BIT(1),
      kAfar   = BIT(2),
      kUSB    = BIT(3)
   };
   
   enum EPowerModeBit {
      kAmpsDatTak = BIT(0),
      kCardDatTak = BIT(1),
      kIridDatTak = BIT(2),
      kAfarDatTak = BIT(3),
      kAmpsComWin = BIT(4),
      kCardComWin = BIT(5),
      kIridComWin = BIT(6),
      kAfarComWin = BIT(7)
   };

   enum ERunModeBit {
      kSingleSeqBit   = BIT(0), // if 0, infinite sequences
      kCountPowerBit  = BIT(1), // if 0, count events
   };

   struct TObsoletePars {
      // old parameters we don't usually want to stream, but we may need
      // for backwards compatibility
      UShort_t          fForced;
      Short_t           fBatVoltLowPwr;
      
      TObsoletePars() : fForced(0), fBatVoltLowPwr(0) {}
      virtual ~TObsoletePars() {}
      ClassDef(TSnDAQConfig::TObsoletePars, 1); // conf back compat layer
   };
   

 private:
   TString           fLabel;  // UNIQUE label for this configuration
   TString           fUsage;  // usage tag
   TString           fUser;   // user that built config
   TString           fDesc;   // description of this config
   TTimeStamp        fBuilt;  // date config built (default current time)
   TSnDacSet         fDacs;   // the DAC values [chan][dac num] (ADC)
   TSnPlaSet         fPlas;   // the trigger patterns [pla num] (binary)
   TSnTriggerSetup   fTrigSet;// the trigger setup
   TSnAmpOnSet       fAmpsOn; // which amps are on [chan]
   TSnCommWinSet     fComWin; // the communication window (default: period 600s, duration 420s, send no data). only the send data value from this window matters, currently.
   TSnCommWinSet     fLPComWin; // the low power communication window (default: period 28200s, duration 900s)
   UChar_t           fRunMode;// see ERunModeBit (default inf seqs and count evts)
   UShort_t          fHrtBt;  // heart beat period (s) (default off)
   Bool_t            fStreamHiLoPlas; // if true, add the separated hi/lo thresh PLA patterns to the i/o (default false)
   UChar_t           fWvLoseLSB; // num least significant bits to lose in compression (default 0)
   UChar_t           fWvLoseMSB; // num most  significant bits to lose in compression (default 4)
   UShort_t          fWvBaseline; // global baseline to use in compression (default 0)
   UChar_t           fDatPackType; // which comm systems to compress the data for (see EDatPackType) (default all)
   UChar_t           fPowMode; // power mode (default ALL OFF)
   UShort_t          fBatVoltLowPwr; // battery volt at which to go to low power (not used) (default 0)
   UShort_t          fBatVoltFromLowPwr; // battery volt at which to go from low back to high power (default 0)
   UShort_t          fVoltCheckPer; // how often to check battery voltage (default 300s)
   UInt_t            fWchDogPer; // watch dog period before reset (s) (default 1200s)
   
   TSnDAQConfig::TObsoletePars fObs;    // conf back compat layer
   Bool_t                      fHasObs; // if back compat vars set
   
   void ResetName();

   // intentionally private
   const TSnDAQConfig::TObsoletePars& GetObsPars() const;
   
 public:
   TSnDAQConfig() : 
      fComWin(600, 420, 0),
      fLPComWin(28200, 900, 0),
      fRunMode(0), fHrtBt(0),
      fStreamHiLoPlas(kFALSE),
      fWvLoseLSB(0), fWvLoseMSB(4), fWvBaseline(0), 
      fDatPackType(kSDcard|kIrid|kAfar|kUSB),
      fPowMode(0), // these need to be 0 to allow easy enabling
      fBatVoltLowPwr(0), fBatVoltFromLowPwr(0),
      fVoltCheckPer(300),
      fWchDogPer(1200),
      fHasObs(kFALSE) {
   }
   virtual ~TSnDAQConfig() { }
   
   Bool_t            IsValidForDb() const;
   virtual void	     Print(Option_t* option = "") const;
   virtual Bool_t    IsEqual(const TObject* obj) const;
   virtual ULong_t   Hash() const { return fLabel.Hash(); }
   
   UInt_t            GetLabelLen() const { return fLabel.Length(); }
   const Char_t*     GetLabel() const { return fLabel.Data(); }
   TString           GetLabelString() const { return fLabel; }
   const Char_t*     GetUsage() const { return fUsage.Data(); }
   const Char_t*     GetUser()  const { return fUser.Data(); }
   const Char_t*     GetDescription() const { return fDesc.Data(); }
   const TTimeStamp& GetDateBuilt() const { return fBuilt; }
   // dacs
   const TSnDacSet&  GetDacSet() const { return fDacs; }
   const TSnDacSet::DacSet_t&
                     GetDacs()   const { return fDacs.GetDacs(); }
   const TSnDacSet::ChDacs_t&
                     GetDacsChan(const TSnDacSet::DacSet_t::size_type ch) const
      { return fDacs.GetDacsChan(ch); }
   UShort_t          GetDac(const TSnDacSet::DacSet_t::size_type ch,
                            const TSnDacSet::ChDacs_t::size_type dn) const
      { return fDacs.GetDac(ch, dn); }
   // plas
   const TSnPlaSet&  GetPlaSet() const { return fPlas; }
   const TSnPlaSet::PlaSet_t&
                     GetPlas() const { return fPlas.GetPlas(); }
   UShort_t          GetPla(const TSnPlaSet::PlaSet_t::size_type pn) const
      { return fPlas.GetPla(pn); }
   // trigger
   const TSnTriggerSetup& GetTrigSetup() const { return fTrigSet; }
   const TMap*       GetTrigLogic() const { return fTrigSet.GetTrigLogic(); }
   UShort_t          GetTrigLogic(const Char_t* type) const
      { return fTrigSet.GetTrigLogic(type); }
   UChar_t           GetNumCardsMajLog() const
      { return fTrigSet.GetNumCardsMajLog(); }
   UShort_t          GetThrottlePeriodms() const
      { return fTrigSet.GetThrottlePeriodms(); }
   Float_t           GetForcedPeriod() const 
      { return fTrigSet.GetForcedPeriod(); }
   Bool_t            IsThermalTriggering() const 
      { return fTrigSet.IsThermalTriggering(); }
   // comm win
   const TSnCommWinSet& GetCommWin()       const { return fComWin; }
   const TSnCommWinSet& GetLowPwrCommWin() const { return fLPComWin; }
   UInt_t            GetCommWinDuration() const 
      { return fComWin.GetDuration(); }
   UInt_t            GetCommWinPeriod() const { return fComWin.GetPeriod(); }
   UChar_t           GetCommWinConnectTOmin() const
      { return fComWin.GetConnectTOmin(); }
   UChar_t           GetCommWinListenTOmin() const
      { return fComWin.GetListenTOmin(); }
   
   // amps on
   const TSnAmpOnSet&
                     GetAmpOnSet() const { return fAmpsOn; }
   const TSnAmpOnSet::AmpOnSet_t&
                     GetAmpsOn() const { return fAmpsOn.GetAmpsOn(); }
   Bool_t            IsAmpOn(const UInt_t ch)
      { return fAmpsOn.IsAmpOn(ch); }
   UChar_t           GetRunMode() const { return fRunMode; }
   Bool_t            IsCountingPowerReadings() const 
      { return ((fRunMode & kCountPowerBit)!=0); }
   Bool_t            IsSingleSeqRunMode() const 
      { return ((fRunMode & kSingleSeqBit)!=0); }
   UShort_t          GetHeartBeatPeriod() const { return fHrtBt; }
   Bool_t            IsStreamingHiLoPlas() const { return fStreamHiLoPlas; }
   UChar_t           GetWvLoseLSB() const { return fWvLoseLSB; }
   UChar_t           GetWvLoseMSB() const { return fWvLoseMSB; }
   UShort_t          GetWvBaseline() const { return fWvBaseline; }
   UChar_t           GetDatPackType() const { return fDatPackType; }
   Bool_t            IsDatPackedFor(const EDatPackBit t)
      { return (fDatPackType & t)!=0; }
   UChar_t           GetPowerMode() const { return fPowMode; }
   UShort_t          GetBatVoltToLowPwr() const { return fBatVoltLowPwr; }
   UShort_t          GetBatVoltFromLowPwr() const { return fBatVoltFromLowPwr; }
   UShort_t          GetVoltCheckPeriod() const { return fVoltCheckPer; }
   Short_t           GetCommWinSendData() const
      { return fComWin.GetSendData(); }
   Bool_t            IsSendingAllFiles() const
      { return fComWin.IsSendingAllFiles(); }
   Bool_t            IsObeyingTimeout() const
      { return fComWin.IsObeyingTimeout(); }
   Bool_t            IsDeletingFiles() const
      { return fComWin.IsDeletingFiles(); }
   Bool_t            IsForcingSBDdata() const
      { return fComWin.IsForcingSBDdata(); }
   UInt_t            GetCommWinPerLowPower() const
      { return fLPComWin.GetPeriod(); }
   UInt_t            GetCommWinDurLowPower() const
      { return fLPComWin.GetDuration(); }
   UInt_t            GetWatchdogPeriod() const { return fWchDogPer; }
   
   Bool_t            IsPoweredFor(const EPowerModeBit p) const { 
      return ((fPowMode & p)!=0) ? 1 : 0;
   }
   
   void     SetLabel(const Char_t* l) { fLabel = l; ResetName(); }
   void     SetUsage(const Char_t* u) { fUsage = u; }
   void     SetUser(const Char_t* u) { fUser = u; }
   void     SetDescription(const Char_t* d) { fDesc = d; }
   void     SetDateBuilt(const TTimeStamp& b) { fBuilt = b; }
   void     SetDateBuilt(const UInt_t tloc,
                         const Bool_t isUTC=kTRUE,
                         const Int_t secOffset=0,
                         const Bool_t dosDate=kFALSE);
   void     SetDac(const TSnDacSet::DacSet_t::size_type ch,
                   const TSnDacSet::ChDacs_t::size_type dn,
                   const UShort_t dac)
      { fDacs.SetDac(ch, dn, dac); }
   void     SetDac(const TSnDacSet::DacSet_t::size_type ch,
                   const TSnDacSet::ChDacs_t::size_type dn,
                   const UShort_t dac,
                   const Char_t*  type)
      { fDacs.SetDac(ch, dn, dac, type); }
   void     SetDacSet(const TSnDacSet& ds) { fDacs = ds; }
   void     SetPla(const TSnPlaSet::PlaSet_t::size_type pn,
                   const UShort_t pla)
      { fPlas.SetPla(pn, pla); }
   void     SetPlaSet(const TSnPlaSet& ps) { fPlas = ps; }
   void     SetPla(const TSnPlaSet::PlaSet_t::size_type pn,
                   const Char_t* pla,
                   const Bool_t r2l=kFALSE)
      { fPlas.SetPla(pn, pla, r2l); }
   void     AddPla(const UShort_t pla) { fPlas.AddPla(pla); }
   void     AddPla(const Char_t* pla, const Bool_t r2l=kFALSE)
      { fPlas.AddPla(pla, r2l); }
   void     SetTrigSetup(const TSnTriggerSetup& t) { fTrigSet = t; }
   void     SetTrigLogic(const Char_t* type, const UShort_t val)
      { fTrigSet.SetTrigLogic(type, val); }
   void     SetNumCardsMajLog(const UShort_t n)
      { fTrigSet.SetNumCardsMajLog(n); }
   void     SetThrottlePeriodms(const UShort_t p)
      { fTrigSet.SetThrottlePeriodms(p); }
   void     SetForcedPeriod(const Float_t p)
      { fTrigSet.SetForcedPeriod(p); }
   void     SetThermalTriggering(const Bool_t t=kTRUE)
      { fTrigSet.SetThermalTriggering(t); }
   void     SetCommWinSet(const TSnCommWinSet& c)   { fComWin = c; }
   void     SetLowPwrCommWinSet(const TSnCommWinSet& c) { fLPComWin = c; }
   void     SetCommWinDuration(const UInt_t d) { fComWin.SetDuration(d); }
   void     SetCommWinPeriod(const UInt_t p) { fComWin.SetPeriod(p); }
   void     SetCommWinConnectTOmin(const UChar_t t) {
      // set both connect timeouts
      fComWin.SetConnectTOmin(t);
      fLPComWin.SetConnectTOmin(t);
   }
   void     SetCommWinListenTOmin(const UChar_t t) {
      // set both listen timeouts
      fComWin.SetListenTOmin(t);
      fLPComWin.SetListenTOmin(t);
   }
   void     SetAmpOn(const UInt_t ch,
                     const Bool_t on=kTRUE)
      { fAmpsOn.SetAmpOn(ch, on); }
   void     SetAmpOnSet(const TSnAmpOnSet& ao) { fAmpsOn = ao; }
   void     SetRunMode(const UChar_t m) { fRunMode = m; }
   void     SetHeartbeatPeriod(const UShort_t h) { fHrtBt = h; }
   void     SetStreamHiLoPlas(const Bool_t p) { fStreamHiLoPlas = p; }
   void     SetWvLoseLSB(const UChar_t l) { fWvLoseLSB = l; }
   void     SetWvLoseMSB(const UChar_t m) { fWvLoseMSB = m; }
   void     SetWvBaseline(const UShort_t b) { fWvBaseline = b; }
   void     SetDatPackType(const UChar_t d) { fDatPackType = d; }
   void     EnablePackDatFor(const EDatPackBit t) { fDatPackType |= t; }
   void     DisablePackDatFor(const EDatPackBit t) { fDatPackType &= ~t; }
   void     TogglePackDatFor(const EDatPackBit t) { fDatPackType ^= t; }
   void     SetPowerMode(const UChar_t m) { fPowMode = m; }
   void     SetBatVoltToLowPwr(const UShort_t v) { fBatVoltLowPwr = v; }
   void     SetBatVoltFromLowPwr(const UShort_t v) { fBatVoltFromLowPwr = v; }
   void     SetVoltCheckPeriod(const UShort_t v) { fVoltCheckPer = v; }
   void     SetCommWinPerLowPower(const UInt_t p) { fLPComWin.SetPeriod(p); }
   void     SetCommWinDurLowPower(const UInt_t d) { fLPComWin.SetDuration(d); }
   void     SetWatchdogPeriod(const UInt_t w) { fWchDogPer = w; }

   void     SetCommWinSendData(const Short_t s) { fComWin.SetSendData(s); }
   void     EnableCommWinSendDataOpt(const TSnCommWinSet::ESendDataBit b)
      { fComWin.EnableSendDataOpt(b); }
   void     DisableCommWinSendDataOpt(const TSnCommWinSet::ESendDataBit b)
      { fComWin.DisableSendDataOpt(b); }
   void     ToggleCommWinSendDataOpt(const TSnCommWinSet::ESendDataBit b)
      { fComWin.ToggleSendDataOpt(b); }
   
   void     EnablePowerModeFor(const EPowerModeBit b) { fPowMode |= b; }
   void     DisablePowerModeFor(const EPowerModeBit b) { fPowMode &= ~b; }
   void     TogglePowerModeFor(const EPowerModeBit b) { fPowMode ^= b; }
   
   void     EnableRunModeBit(const ERunModeBit b) { fRunMode |= b; }
   void     DisableRunModeBit(const ERunModeBit b) { fRunMode &= ~b; }
   void     ToggleRunModeBit(const ERunModeBit b) { fRunMode ^= b; }
   
   static
   TString  GetDatPackBitName(const EDatPackBit x);
   static
   TString  PyGetDatPackBitName(const Int_t x) {
      // helper function for pyroot
      return GetDatPackBitName(static_cast<EDatPackBit>(x));
   }

   
   Bool_t   HasObsParams() const { return fHasObs; }
   UShort_t GetObsForcedPeriodUShort() const 
            { return GetObsPars().fForced; }
   Short_t  GetObsBatVoltToLowPwrShort() const
            { return GetObsPars().fBatVoltLowPwr; }
   void     SetObsForcedPeriodUShort(const UShort_t f);
   void     SetObsBatVoltToLowPwrShort(const Short_t v);

   ClassDef(TSnDAQConfig, 8); // DAQ configuration parameters
};

#endif // SNS_TSnDAQConfig
