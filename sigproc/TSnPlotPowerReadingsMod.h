#ifndef SNS_TSnPlotPowerReadingsMod
#define SNS_TSnPlotPowerReadingsMod

#include <TString.h>
class TGraphErrors;

#include "TAModule.h"

class TSnPowerReading;


class TSnPlotPowerReadingsMod : public TAModule {
 private:
   TString       fVoltNm;
   TString       fCurrNm;
   
   // resulting plots
   TGraphErrors* fVolt;
   TGraphErrors* fCurr;
   
   // branch(es)
   TSnPowerReading* fPwr; //!

 protected:
   virtual void        SlaveBegin();
   virtual void        Process();
   
 public:
   TSnPlotPowerReadingsMod() : fVolt(0), fCurr(0), fPwr(0) {}
   TSnPlotPowerReadingsMod(const Char_t* name);
   virtual ~TSnPlotPowerReadingsMod();
   
   const Char_t* GetVoltName() const { return fVoltNm.Data(); }
   const Char_t* GetCurrName() const { return fCurrNm.Data(); }
   
   void          SetVoltName(const Char_t* v) { fVoltNm = v; }
   void          SetCurrName(const Char_t* c) { fCurrNm = c; }
   
   ClassDef(TSnPlotPowerReadingsMod, 1); // make volt/curr plots
};

#endif // SNS_TSnPlotPowerReadingsMod
