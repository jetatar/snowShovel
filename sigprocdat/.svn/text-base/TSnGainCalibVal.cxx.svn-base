#include "TSnGainCalibVal.h"

#include <TMath.h>
#include <TString.h>

ClassImp(TSnGainCalibVal);

Float_t TSnGainCalibVal::CalibError(const Float_t x, const Float_t ex) const {
   // this fcn assumes gaussian errors and zero covariance between
   // any combination of the fit parameters and independent
   // variable
   return TMath::Sqrt( (x*x*fEM*fEM)+(fEB*fEB)+(fM*fM*ex*ex) );
}

void TSnGainCalibVal::Print(Option_t* option) const {
   Printf(" ((%g +/- %g) * x) + (%g +/- %g)",fM,fEM,fB,fEB);
}

