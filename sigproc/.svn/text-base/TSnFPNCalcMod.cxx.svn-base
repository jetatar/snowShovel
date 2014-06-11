#include "TSnFPNCalcMod.h"

#include <TH1.h>
#include <TH2.h>
#include <TString.h>

#include "TSnMath.h"
#include "TSnFPNCalibSet.h"
#include "TSnRawWaveform.h"
#include "TSnRawTreeMaker.h"

ClassImp(TSnFPNCalcMod);


TSnFPNCalcMod::TSnFPNCalcMod(const Char_t* name,
                             const Char_t* fpnsetname) :
   TAModule(name, "module to calculate FPN values"),
   fFpnSet(0), fPeakFrc(0.05), fFpnSetNm(fpnsetname),
   fGausFit(kFALSE),
   fSbins(NSnConstants::kTotSamps), 
   fSmin(-0.5), fSmax(NSnConstants::kTotSamps-0.5),
   fAbins(NSnConstants::kNadcs),
   fAmin(-0.5), fAmax(NSnConstants::kNadcs-0.5),
   fRaw(0) {
}

TSnFPNCalcMod::~TSnFPNCalcMod() {
   delete fFpnSet;
}

void TSnFPNCalcMod::SlaveBegin() {
   ReqBranch(TSnRawTreeMaker::kEWvBrNm, fRaw);
   
   TString hn(Form("hAdcs_%s",GetName()));
   fHadcs = new TH2F(hn.Data(),
                     "all adc values;sample;adc",
                     fSbins, fSmin, fSmax,
                     fAbins, fAmin, fAmax);
   AddOutput(fHadcs);
}

void TSnFPNCalcMod::Process() {

   // assume event has already passed event selection
   LoadBranch(TSnRawTreeMaker::kEWvBrNm);
   
   if (fRaw!=0) {
      const UShort_t* d = fRaw->GetWvData();
      for (UShort_t s=0; s<NSnConstants::kTotSamps; ++s, ++d) {
         fHadcs->Fill(s, *d);
      }
   } else {
      SendError(kAbortModule, "Process",
                "Could not get raw waveform data.");
   }
}

void TSnFPNCalcMod::Terminate() {
   
   Double_t ave(0), rms(0), n(0);
   delete fFpnSet;
   fFpnSet = new TSnFPNCalibSet(fFpnSetNm.Data());
   AddOutput(fFpnSet);
   
   // loop over samples
   for (UShort_t s=0; s<NSnConstants::kTotSamps; ++s) {
      // calc mean & rms for this sample
      // bin number counts from 1
      TH1D* h = fHadcs->ProjectionY("_py",s+1,s+1);
      /*
      ave = h->GetMean();
      rms = h->GetRMS();
      */
      // get mean of primary peak
      if (fGausFit) {
         TSnMath::FindGausMeanRmsSteppingFromPeak(*h, fPeakFrc,
                                                  ave, rms, n);
      } else {
         TSnMath::FindMeanRmsSteppingFromPeak(*h, fPeakFrc,
                                              ave, rms, n);
      }
      fFpnSet->AddCalibAt(s, ave, rms, n);
      
      delete h;
      
   }
}




