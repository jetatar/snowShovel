#ifndef SNS_TSnShiftCalWvDataMod
#define SNS_TSnShiftCalWvDataMod

#include <TString.h>

#include "TAModule.h"

class TSnCalWvData;

class TSnShiftCalWvDataMod : public TAModule {
 private:
   TString       fDatBrNm;  // name of the input branch containing the data (fpn sub or amp out)
   TString       fStopNm;   // name of the input stop position object in the event (default: StopPositions)
   TString       fShiftNm;  // name of the output shifted TSnCalWvData (no default!)
   
   // event data
   TSnCalWvData* fDat; //! the data

 protected:
   virtual void SlaveBegin();
   virtual void Process();

 public:
   TSnShiftCalWvDataMod() : fDat(0) {}
   TSnShiftCalWvDataMod(const Char_t* name,
                        const Char_t* dataBrNm,
                        const Char_t* shiftNm,
                        const Char_t* stopNm="StopPositions") :
      TAModule(name, "shift calib waveform using stop positions"),
      fDatBrNm(dataBrNm), fStopNm(stopNm), fShiftNm(shiftNm),
      fDat(0) {
      // normal ctor
   }
   virtual ~TSnShiftCalWvDataMod() {}
   
   
   TString  GetDataBranchName() const { return fDatBrNm; }
   TString  GetStopPosnName() const { return fStopNm; }
   TString  GetShiftedDataName() const { return fShiftNm; }

   void     SetDataBranchName(const Char_t* n) { fDatBrNm = n; }
   void     SetStopPosnName(const Char_t* n) { fStopNm = n; }
   void     SetShiftedDataName(const Char_t* n) { fShiftNm = n; }

   ClassDef(TSnShiftCalWvDataMod, 1); // shift a calib wvfm using stop positions found in the event
};

#endif // SNS_TSnShiftCalWvDataMod
