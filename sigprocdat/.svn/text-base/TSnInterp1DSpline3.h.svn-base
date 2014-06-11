#ifndef SNS_TSnInterp1DSpline3
#define SNS_TSnInterp1DSpline3

class TGraph;
class TSpline3;

#include "TSnInterp1DWvData.h"
class TSnCalWvData;

class TSnInterp1DSpline3 : public TSnInterp1DWvData {
 private:
   TSpline3*      fSpline; // the spline doing the interpolating

   void                   AssertSpline(const Char_t* loc) const;

 public:
   TSnInterp1DSpline3() : fSpline(0) {}
   TSnInterp1DSpline3(const Char_t* name, const Char_t* title,
                      Double_t* x, Double_t* y,
                      const Int_t n);
   TSnInterp1DSpline3(const Char_t* name, const Char_t* title,
                      const TGraph* g);
   TSnInterp1DSpline3(const Char_t* name, const Char_t* title,
                      const TSnCalWvData& wvd, const UChar_t ch,
                      const Bool_t vsTime=kTRUE);
   TSnInterp1DSpline3(const TSnInterp1DSpline3& s);
   virtual ~TSnInterp1DSpline3();

   static
   void                   Swap(TSnInterp1DSpline3& a, TSnInterp1DSpline3& b);
   
   TSnInterp1DSpline3&    operator=(TSnInterp1DSpline3 o) {
      // assign using efficient copy from pass by value
      Swap(*this, o);
      return *this;
   }
   
   virtual Double_t       FindXAtExtremum(const Bool_t min,
                                          Double_t* const ymax=0) const;
   virtual Double_t       Eval(const Double_t x) const;
   virtual       TObject* GetInterpolator();
   virtual const TObject* GetInterpolator() const;
   virtual void           CopyFrom(const TSnInterp1DWvData& x) {
      *this = dynamic_cast<const TSnInterp1DSpline3&>(x);
   }
   
   const TSpline3*        GetSpline() const { return fSpline; }
         TSpline3*        GetSpline()       { return fSpline; }
   
   void                   SetSpline(const TSpline3& s);
   
   ClassDef(TSnInterp1DSpline3, 1); // 1D TSpline3 interpolator of event (waveform) data using
};

#endif // SNS_TSnInterp1DSpline3
