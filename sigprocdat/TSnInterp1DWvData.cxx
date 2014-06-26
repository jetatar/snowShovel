#include "TSnInterp1DWvData.h"

#include <Math/Functor.h>
#include <Math/BrentRootFinder.h>

#include <stdexcept>

ClassImp(TSnInterp1DWvData);

Double_t TSnInterp1DWvData::FindXAt(const Double_t y, 
                                    const Double_t xmin, const Double_t xmax,
                                    const Int_t nsteps,
                                    const Double_t epsilon,
                                    const Int_t maxiter, 
                                    const Bool_t logx) const {
   // analagous to TF1::GetX
   //
   // use ROOT::Math::BrentRootFinder to find the x value where this
   // interpolation crosses y in the window (xmax, xmin)
   //
   // step size is (xmax-xmin)/nsteps
   //
   // see TF1::GetX for explanation of parameters
   //
   // unlike TF1::GetX, logx is not set automatically if the current
   // pad has logx set.
   
   if (xmax>xmin) {
      SetY0(y);
      ROOT::Math::Functor1D wf(this,
                               &TSnInterp1DWvData::operator());
      ROOT::Math::BrentRootFinder brf;
      brf.SetFunction(wf, xmin, xmax);
      brf.SetNpx(nsteps);
      brf.SetLogScan(logx);
      brf.Solve(maxiter, epsilon, epsilon);
      return brf.Root();
      
   } else {
      throw std::invalid_argument(Form("<TSnInterp1DWvData::FindXAt>: "
                                       "xmax (%g) must be greater than "
                                       "xmin (%g)", xmax, xmin));
   }
}
