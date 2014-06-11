#ifndef SNS_TSnMinimizer
#define SNS_TSnMinimizer

#include <TNamed.h>
namespace ROOT {
   namespace Math {
      class Minimizer;
      class Functor;
   };
};

class TSnMinimizer : public TNamed {
 private:
   ROOT::Math::Minimizer* fMini; //  the minimizer
   ROOT::Math::Functor*   fFcn;  //! wrapper for the fit function (owned)
   
   void  AssertMini(const Char_t* loc) const;
   
 public:
   TSnMinimizer() : fMini(0), fFcn(0) {}
   TSnMinimizer(const Char_t* miniName, const Char_t* algoName);
   TSnMinimizer(const TSnMinimizer& m);
   virtual ~TSnMinimizer();

   static
   void Swap(TSnMinimizer& a, TSnMinimizer& b);

   TSnMinimizer& operator=(TSnMinimizer o) { // pass by value for efficient copy
      Swap(*this, o);
      return *this;
   }
   
   const ROOT::Math::Minimizer* GetMinimizer() const { return fMini; }
         ROOT::Math::Minimizer* GetMinimizer()       { return fMini; }
   const ROOT::Math::Minimizer& GetMiniSafe() const
      { AssertMini("GetMiniSafe"); return *fMini; }
         ROOT::Math::Minimizer& GetMiniSafe()      
      { AssertMini("GetMiniSafe"); return *fMini; }
   
   const ROOT::Math::Functor*   GetFunctor() const { return fFcn; }
         ROOT::Math::Functor*   GetFunctor()       { return fFcn; }


   const Char_t* GetMinimizerName() const { return GetName(); }
   const Char_t* GetAlgorithmName() const { return GetTitle(); }
   
   // rather than copy every Functor ctor here, we only allow copying in
   void   SetFunctor(const ROOT::Math::Functor& f);
   

   // must set mini and algo names together because we have to create
   // a new minimizer object each time they change
   void   ResetMinimizer(const Char_t* miniNm, const Char_t* algoNm);
   
   ClassDef(TSnMinimizer, 1); // a class to provide an interface for Root minimizer routines
};

#endif // SNS_TSnMinimizer
