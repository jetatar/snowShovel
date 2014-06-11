#ifndef SNS_TSnFPNSubMod
#define SNS_TSnFPNSubMod

#include <TString.h>

#include "TAModule.h"

class TSnRawWaveform;
class TSnFPNCalibSet;

class TSnFPNSubMod : public TAModule {
 private:
   TString          fFpnSetName; // name of (input) FPN set in the published objs
   TString          fCalDatNm;   // name of (output) FPN sub wvfm data added to evt (default "FPNSubData")
   
   // temp stuff
   TSnRawWaveform*       fRaw;    //! raw waveform data
   const TSnFPNCalibSet* fFpnSet; //! set of FPN values
   
 protected:
   virtual void        SlaveBegin();
   virtual void        Process();
   
 public:
   TSnFPNSubMod() : fRaw(0), fFpnSet(0) {}
   TSnFPNSubMod(const Char_t* name,
                const Char_t* fpnsetnm,
                const Char_t* caldatnm="FPNSubData") :
      TAModule(name, "module to subtract FPN from raw wvfm data"),
      fFpnSetName(fpnsetnm), fCalDatNm(caldatnm),
      fRaw(0), fFpnSet(0) {}
   virtual ~TSnFPNSubMod() {}
   
   TString  GetFPNSetName() const { return fFpnSetName; }
   TString  GetCalDatName() const { return fCalDatNm; }
   
   void     SetFPNSetName(const Char_t* n) { fFpnSetName = n; }
   void     SetCalDatName(const Char_t* n) { fCalDatNm = n; }
   
   ClassDef(TSnFPNSubMod, 1); // subtract FPN from raw data
};

#endif // SNS_TSnFPNSubMod
