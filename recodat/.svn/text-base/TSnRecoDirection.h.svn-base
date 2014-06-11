#ifndef SNS_TSnRecoDirection
#define SNS_TSnRecoDirection

#include <TVector3.h>

#include "TSnRecoResult.h"

class TSnRecoDirection : public TSnRecoResult {
 private:
   TVector3      fDir; // the reconstructed direction
   TVector3      fErr; // errors on the direction parameters
   Double_t      fLik; // the fit quality paramter (chi2, log likelihood, etc)

 public:
   TSnRecoDirection() : fLik(0) {}
   TSnRecoDirection(const Char_t* name, const Char_t* minimizerTag) :
      TSnRecoResult(name, minimizerTag),
      fLik(0) {
   }
   TSnRecoDirection(const Char_t* name, const Char_t* minimizerTag,
                    const Double_t theta, const Double_t terr,
                    const Double_t phi, const Double_t perr,
                    const Double_t qual) :
      TSnRecoResult(name, minimizerTag),
      fLik(qual) {
      fDir.SetMagThetaPhi(1.0, theta, phi);
      fErr.SetMagThetaPhi(1.0, terr, perr);
   }

   virtual void CopyFrom(const TSnRecoResult& x) {
      // this will throw a std::bad_cast if x is not a TSnRecoDirection
      *this = dynamic_cast<const TSnRecoDirection&>(x);
   }
   
   const TVector3& GetDir() const { return fDir; }
   const TVector3& GetErr() const { return fErr; }
   Double_t        GetFitQual() const { return fLik; }
   Double_t        GetTheta() const { return fDir.Theta(); }
   Double_t        GetPhi() const { return fDir.Phi(); }
   Double_t        GetThetaErr() const { return fErr.Theta(); }
   Double_t        GetPhiErr() const { return fErr.Phi(); }

   void            SetFitQual(const Double_t q) { fLik = q; }
   void            SetDir(const TVector3& d) { fDir = d; }
   void            SetDir(const Double_t theta, const Double_t phi)
      { fDir.SetMagThetaPhi(1.0, theta, phi); }
   void            SetDir(const Double_t x, const Double_t y, const Double_t z)
      { fDir.SetXYZ(x,y,z); }
   void            SetErr(const TVector3& d) { fErr = d; }
   void            SetErr(const Double_t terr, const Double_t perr)
      { fErr.SetMagThetaPhi(1.0, terr, perr); }
   void            SetErr(const Double_t x, const Double_t y, const Double_t z)
      { fErr.SetXYZ(x,y,z); } 
   
   virtual void Print(Option_t* option = "") const;
   
   ClassDef(TSnRecoDirection, 1); // reconstructed direction
};

#endif // SNS_TSnRecoDirection
