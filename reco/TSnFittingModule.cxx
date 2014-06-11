#include "TSnFittingModule.h"

#include "TSnMinimizer.h"

#include <stdexcept>

ClassImp(TSnFittingModule);

TSnFittingModule::TSnFittingModule(const Char_t* name, const Char_t* title,
                                   const Char_t* miniName,
                                   const Char_t* algoName) :
      TAModule(name, title), fMiniNm(miniName), fAlgoNm(algoName),
      fMini(new TSnMinimizer(miniName, algoName)) {
}

TSnFittingModule::~TSnFittingModule() {
   delete fMini;
}

void TSnFittingModule::SetFunctor(const ROOT::Math::Functor& f) {
   AssertMini("SetFunctor");
   fMini->SetFunctor(f);
}

void TSnFittingModule::ResetMinimizer(const Char_t* miniNm, 
                                      const Char_t* algoNm) {
   // must set mini and algo names together because we have to create
   // a new minimizer object each time they change.
   //
   // this also clears any settings the minimizer had (since it's new)
   // like parameters, number of iterations, etc!
   AssertMini("ResetMinimizer");
   fMiniNm = miniNm;
   fAlgoNm = algoNm;
   fMini->ResetMinimizer(fMiniNm.Data(), fAlgoNm.Data());
}

const ROOT::Math::Minimizer& TSnFittingModule::GetMiniSafe() const {
   AssertMini("GetMiniSafe");
   return fMini->GetMiniSafe();
}

ROOT::Math::Minimizer& TSnFittingModule::GetMiniSafe() {
   AssertMini("GetMiniSafe");
   return fMini->GetMiniSafe();
}

const TSnMinimizer& TSnFittingModule::GetMinimizerSafe() const {
   AssertMini("GetMinimizerSafe");
   return *fMini;
}

TSnMinimizer& TSnFittingModule::GetMinimizerSafe() {
   AssertMini("GetMinimizerSafe");
   return *fMini;
}

const ROOT::Math::Functor& TSnFittingModule::GetFunctorSafe() const {
   AssertMini("GetFunctorSafe");
   const ROOT::Math::Functor* fcn = fMini->GetFunctor();
   if (fcn!=0) {
      return *fcn;
   } else {
      throw std::runtime_error("<TSnFittingModule::GetFunctorSafe>: "
                               "No functor yet set in minimizer, or "
                               "TSnMinimizer::SetFunctor not used.");
   }
}

void TSnFittingModule::AssertMini(const Char_t* loc) const {
   if (fMini==0) {
      throw std::runtime_error(Form("<TSnFittingModule::%s>: "
                                    "No minimizer object.", loc));
   }
}
