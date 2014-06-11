#include "TSnPlotPowerReadingsMod.h"

#include <TGraphErrors.h>
#include <TMath.h>

#include "TSnRawTreeMaker.h"
#include "TSnPowerReading.h"

ClassImp(TSnPlotPowerReadingsMod);


TSnPlotPowerReadingsMod::TSnPlotPowerReadingsMod(const Char_t* name) :
   TAModule(name, "make voltage/current graphs"),
   fVoltNm("gVolt"), fCurrNm("gCurr"), fVolt(0), fCurr(0), fPwr(0) {
}

TSnPlotPowerReadingsMod::~TSnPlotPowerReadingsMod() {
   delete fVolt;
   delete fCurr;
}

void TSnPlotPowerReadingsMod::SlaveBegin() {

   // request branch(es)
   ReqBranch(TSnRawTreeMaker::kPwrBrNm, fPwr);
   
   // make output
   fVolt = new TGraphErrors;
   fVolt->SetName(fVoltNm.Data());
   fVolt->SetTitle("Voltage vs Time; Date,time (UTC); V_in (V)");
   
   fCurr = new TGraphErrors;
   fCurr->SetName(fCurrNm.Data());
   fCurr->SetTitle("Current vs Time; Date,time (UTC); I_in (A)");
   
   AddOutput(fVolt);
   AddOutput(fCurr);
}

void TSnPlotPowerReadingsMod::Process() {
   LoadBranch(TSnRawTreeMaker::kPwrBrNm);
   
   // TODO: fix to use calibrations
   static const Float_t vgain=6.0649e-4;
   static const Float_t cped=1.112;
   static const Float_t cgain=-2.6828e-5;
   static const Float_t nave=500;
   const Float_t  v = fPwr->GetAveV1() * vgain;
   const Float_t ev = fPwr->GetRmsV1() * vgain / nave;
   const Float_t  c = cped + ( (fPwr->GetAveV2() - fPwr->GetAveV1())*cgain );
   const Float_t ec = TMath::Sqrt( (cgain*cgain)*
                                   ( (fPwr->GetRmsV1()*fPwr->GetRmsV1()) +
                                     (fPwr->GetRmsV2()*fPwr->GetRmsV2()) )
                                   / nave );
   const Int_t pv = fVolt->GetN();
   fVolt->SetPoint(pv, fPwr->GetTime(), v);
   fVolt->SetPointError(pv, 0, ev);
   const Int_t cv = fCurr->GetN();
   fCurr->SetPoint(cv, fPwr->GetTime(), c);
   fCurr->SetPointError(cv, 0, ec);
}


