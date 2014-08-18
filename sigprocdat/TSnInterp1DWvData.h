#ifndef SNS_TSnInterp1DWvData
#define SNS_TSnInterp1DWvData

#include <TNamed.h>

class TSnInterp1DWvData : public TNamed {
 private:
   mutable Double_t fY0; // used to find the x such that f(x)=fY0

   void           SetY0(const Double_t y) const { fY0 = y; }

 public:
   TSnInterp1DWvData() : fY0(0) {}
   TSnInterp1DWvData(const Char_t* name, const Char_t* title) :
      TNamed(name, title),
      fY0(0) {
      // normal ctor
   }
   virtual ~TSnInterp1DWvData() {}
   
   // the interface we want to have for any 1D interpolation
   virtual Double_t       FindXAtExtremum(const Bool_t min,
                                          Double_t* const ymax=0) const = 0;
   virtual Double_t       Eval(const Double_t x) const         = 0;
   virtual       TObject* GetInterpolator()                    = 0;
   virtual const TObject* GetInterpolator() const              = 0;
   virtual void           CopyFrom(const TSnInterp1DWvData& x) = 0;

   Double_t       FindXAtMaximum(Double_t* const ymax=0) const {
      // find the value of the independent variable (x) at which
      // the dependent variable (y) is a maximum
      //
      // if a non-zero pointer to 'ymax' is specified, the value of
      // ymax will be set to the maximum
      return FindXAtExtremum(kFALSE, ymax);
   }
   Double_t       FindXAtMinimum(Double_t* const ymin=0) const {
      // find the value of the independent variable (x) at which
      // the dependent variable (y) is a maximum
      //
      // if a non-zero pointer to 'ymin' is specified, the value of
      // ymin will be set to the maximum
      return FindXAtExtremum(kTRUE, ymin);
   }

   Double_t       FindMaximum() const {
      // return the maximum value of the dependent variable
      Double_t ymax(0);
      FindXAtMaximum(&ymax);
      return ymax;
   }
   Double_t       FindMinimum() const {
      // return the minimum value of the dependent variable
      Double_t ymin(0);
      FindXAtMinimum(&ymin);
      return ymin;
   }

   Double_t       operator()(Double_t x) const {
      // allows wrapping this interpolator in a ROOT::Math::Functor1D
      //
      // this operator is intentionally not virtual as it is passed to the
      // functor inside FindXAt. the "virtual-ness" is provided by Eval
      return Eval(x) - fY0;
   }
   
   virtual Double_t FindXAt(const Double_t y, 
                            const Double_t xmin, const Double_t xmax,
                            const Int_t nsteps = 100,
                            const Double_t epsilon = 1.e-10,
                            const Int_t maxiter = 100, 
                            const Bool_t logx = kFALSE) const;

   
   Double_t       GetY0() const { return fY0; }
   
   
   virtual void           Draw(Option_t* option = "") {
      TObject* o = GetInterpolator();
      if (o!=0) {
         o->Draw(option);
      }
   }
   
   ClassDef(TSnInterp1DWvData, 2); // ABC defining interface for 1D interpolation of event data
};

#endif // SNS_TSnInterp1DWvData
