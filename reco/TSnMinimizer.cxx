#include "TSnMinimizer.h"

#include <Math/Minimizer.h>
#include <Math/Functor.h>
#include <Math/Factory.h>
#include <TString.h>

#include <stdexcept>
#include <algorithm>

ClassImp(TSnMinimizer);

TSnMinimizer::TSnMinimizer(const Char_t* miniName, const Char_t* algoName) :
   TNamed(miniName, algoName),
   fMini(ROOT::Math::Factory::CreateMinimizer(miniName, algoName)),
   fFcn(0) {
   // normal ctor
}

TSnMinimizer::TSnMinimizer(const TSnMinimizer& m) :
   TNamed(m), 
   fMini(ROOT::Math::Factory::CreateMinimizer(m.GetMinimizerName(),
                                              m.GetAlgorithmName())),
   fFcn(0) {
   // copy ctor. options are function are copied, but NOT PARAMETERS!
   
   const ROOT::Math::Functor* f = m.GetFunctor();
   if (f!=0) {
      SetFunctor(*f);
   }
   fMini->SetOptions(m.fMini->Options());
}

TSnMinimizer::~TSnMinimizer() {
   // dtor
   delete fMini;
   delete fFcn;
}

void TSnMinimizer::Swap(TSnMinimizer& a, TSnMinimizer& b) {
   std::swap(a.fMini, b.fMini);
   std::swap(a.fFcn,  b.fFcn);
}


void TSnMinimizer::AssertMini(const Char_t* loc) const {
   if (fMini==0) {
      throw std::runtime_error(Form("<TSnMinimizer::%s>: No minimizer created.",
                                    loc));
   }
}

void TSnMinimizer::SetFunctor(const ROOT::Math::Functor& f) {
   // clear the minimizer parameters and (re-)set the functor
   // to (a copy of) f
   AssertMini("SetFunctor");
   fMini->Clear();
   if (&f != fFcn) {
      delete fFcn;
      fFcn = new ROOT::Math::Functor(f);
   }
   fMini->SetFunction(*fFcn);
}

void TSnMinimizer::ResetMinimizer(const Char_t* miniNm, const Char_t* algoNm) {
   // reset the minimizer to use a different minimizer code and algorithm.
   // attempt to copy over most options (tolerance, iterations, etc)
   // BUT: these options may not make sense for the new minimizer
   // and parameter variables are NOT copied -- hence the function name
   // "Reset" instead of "Set"
   ROOT::Math::MinimizerOptions* o(0);
   if (fMini!=0) {
      o = new ROOT::Math::MinimizerOptions(fMini->Options());
      delete fMini;
   }
   fMini = ROOT::Math::Factory::CreateMinimizer(miniNm, algoNm);
   if (fFcn!=0) {
      fMini->SetFunction(*fFcn);
   }
   if (o!=0) {
      fMini->SetOptions(*o);
      delete o;
   }
}
