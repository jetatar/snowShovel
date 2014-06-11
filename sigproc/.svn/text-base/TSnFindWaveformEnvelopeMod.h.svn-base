#ifndef SNS_TSnFindWaveformEnvelopeMod
#define SNS_TSnFindWaveformEnvelopeMod

#include <TString.h>

#include "TSnCalDatModule.h"

class TSnFindWaveformEnvelopeMod : public TSnCalDatModule {
 public:
   static const UInt_t kDebugEnv; // verbosity level at which envelope debugging is turned on (i.e. plots are drawn in each event)

 private:
   TString       fDatBrNm;  // name of the input branch (or evt object) containing the TSnCalWvData data
   TString       fEnvDatNm; // name of the output envelope data object added to the event
   
   // branch data
   TSnCalWvData* fDat;      //! the data to filter

 protected:
   virtual void SlaveBegin();
   virtual void Process();
   
 public:
   TSnFindWaveformEnvelopeMod() : fDat(0) {}
   TSnFindWaveformEnvelopeMod(const Char_t* name,
                              const Char_t* datBrNm,
                              const Char_t* envDtNm) :
      TSnCalDatModule(name, "adds waveform envelope to event"),
      fDatBrNm(datBrNm),
      fEnvDatNm(envDtNm),
      fDat(0) {
      // normal ctor
   }
   virtual ~TSnFindWaveformEnvelopeMod() {}
   
   TString       GetDataBrName() const { return fDatBrNm; }
   TString       GetEnvDatName() const { return fEnvDatNm; }
   
   void          SetDataBrName(const Char_t* n) { fDatBrNm = n; }
   void          SetEnvDatName(const Char_t* n) { fEnvDatNm = n; }
   
   ClassDef(TSnFindWaveformEnvelopeMod, 1); // find the envelope of a waveform and add it to the event
};

#endif // SNS_TSnFindWaveformEnvelopeMod
