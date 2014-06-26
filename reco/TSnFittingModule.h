#ifndef SNS_TSnFittingModule
#define SNS_TSnFittingModule

#include <TString.h>
namespace ROOT {
   namespace Math {
      class Minimizer;
      class Functor;
   };
};

#include "TAModule.h"

class TSnMinimizer;


class TSnFittingModule : public TAModule {
////////////////////////////////////////////////////////////////////////////////
//
// TSnFittingModule
//
// A module with easy access to some of the fitting code. Most important
// is the choice of minimizer and algorithm. Possibilities are:
//
//     minner              algo
//  Minuit /Minuit2        Migrad,Simplex,Combined,Scan  (default is Migrad)
//  Minuit2                Fumili2
//  Fumili
//  GSLMultiMin            ConjugateFR, ConjugatePR, BFGS, 
//                         BFGS2, SteepestDescent
//  GSLMultiFit
//  GSLSimAn
//  Genetic
//
////////////////////////////////////////////////////////////////////////////////

 private:
   TString       fMiniNm;   // name of minimizer to use (default: Minuit2)
   TString       fAlgoNm;   // name of minimization algorithm to use (default: Migrad)

   // fitting stuff
   TSnMinimizer* fMini;     // the minimizer to be used

 public:
   TSnFittingModule() : fMini(0) {}
   TSnFittingModule(const Char_t* name, const Char_t* title,
                    const Char_t* miniName/*="Minuit2"*/,
                    const Char_t* algoName/*="Migrad"*/);
   virtual ~TSnFittingModule();

   void AssertMini(const Char_t* loc) const;

   TString GetMinimizerName() const { return fMiniNm; }
   TString GetAlgorithmName() const { return fAlgoNm; }
   const TSnMinimizer* GetMinimizer() const { return fMini; }
         TSnMinimizer* GetMinimizer()       { return fMini; }
   const TSnMinimizer& GetMinimizerSafe() const;
         TSnMinimizer& GetMinimizerSafe();
   const ROOT::Math::Minimizer& GetMiniSafe() const;
         ROOT::Math::Minimizer& GetMiniSafe();
   const ROOT::Math::Functor&   GetFunctorSafe() const;
   
   const Char_t* GetDefMinTag() const {
      // uses Form, so should not be called from within another Form
      return Form("%s:%s", fMiniNm.Data(), fAlgoNm.Data());
   }

   // rather than copy every Functor ctor here, we only allow copying in
   void   SetFunctor(const ROOT::Math::Functor& f);

   // must set mini and algo names together because we have to create
   // a new minimizer object each time they change
   void    ResetMinimizer(const Char_t* miniNm, const Char_t* algoNm);

   
   ClassDef(TSnFittingModule, 1); // base class for modules that will be fitting with a TSnMinimizer
};

#endif // SNS_TSnFittingModule
