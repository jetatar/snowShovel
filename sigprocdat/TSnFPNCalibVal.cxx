#include "TSnFPNCalibVal.h"

#include <TString.h>
#include <TMath.h>

#include <stdexcept>

ClassImp(TSnFPNCalibVal);

void TSnFPNCalibVal::Print(Option_t* option) const {
   Printf("  FPN=%g, RMS=%g, N=%g", fFpn, fRms, fN);
}

Float_t TSnFPNCalibVal::Calibrate(const Float_t x) const {
   return (x - fFpn);
}

Float_t TSnFPNCalibVal::CalibError(const Float_t, const Float_t ex) const {
   const Float_t fe = GetFPNError();
   return TMath::Sqrt( (ex*ex) + (fe*fe) );
}

Float_t TSnFPNCalibVal::GetFPNError() const {
   if (fN>1) {
      return fRms / TMath::Sqrt(fN);
   } else {
      throw std::runtime_error(Form("<TSnFPNCalibVal::GetFPNError>: "
                                    "Cannot calculate FPN error with N=%g; "
                                    "too few events. This can happen, for "
                                    "example, if only forced triggered events "
                                    "are being used to calculate FPN but the "
                                    "data does not contain any (due to very "
                                    "high thermal trigger rates).",
                                    fN));
   }
   return -1;
}

Float_t TSnFPNCalibVal::GetRMSError() const {
   if (fN>1) {
      return fRms / TMath::Sqrt(2.0*(fN-1.0));
   } else {
      throw std::runtime_error(Form("<TSnFPNCalibVal::GetRMSError>: "
                                    "Cannot calculate FPN error with N=%g; "
                                    "too few events. This can happen, for "
                                    "example, if only forced triggered events "
                                    "are being used to calculate FPN but the "
                                    "data does not contain any (due to very "
                                    "high thermal trigger rates).",
                                    fN));
   }
   return -1;
}

