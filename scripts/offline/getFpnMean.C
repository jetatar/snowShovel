#if !defined(__CINT__) || defined(__MAKECINT__)
#include "TMath.h"
#include "TGraphErrors.h"
#include "TString.h"
#include "TFile.h"

#include <TSnMath.h>
#include <NSnConstants.h>
#endif

TFile* inf;
TGraphErrors* gPeds[NSnConstants::kNsamps];

void getFpnMean(const Char_t* infn) {
  inf = TFile::Open(infn);
  
  for (Int_t ch=0; ch<NSnConstants::kNsamps; ++ch) {
    gPeds[ch] = 
      dynamic_cast<TGraphErrors*>(inf->Get(
					   Form("gPedAndRmsVsSamp_ch%02d",
						ch)));
    if (gPeds[ch]!=0) {
      Double_t mean(0),rms(0);
      TSnMath::GetAveRMSFast(gPeds[ch]->GetY(), NSnConstants::kNsamps,
			     mean, rms);
      Printf("chan %d: FPN ave %3.3e, rms %3.3e",
	     ch, mean, rms);
    }
  }
}
