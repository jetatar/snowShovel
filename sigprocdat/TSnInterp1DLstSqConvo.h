#ifndef SNS_TSnInterp1DLstSqConvo
#define SNS_TSnInterp1DLstSqConvo

#include <TMatrixT.h>
class TSpline3;
class TGraph;

#include "TSnInterp1DWvData.h"
class TSnCalWvData;

class TSnInterp1DLstSqConvo : public TSnInterp1DWvData {
 private:
   TSpline3*           fFit; // a spline fit to the smoothed lst sq convolution
   UInt_t              fN;   // polynomial order
   UInt_t              fM;   // 2m+1 points per window
   UInt_t              fS;   // the s'th derivative
   TMatrixT<Double_t>  fH;   // coefficient table


   void BuildFromData(Double_t* const x,
                      Double_t* const y,
                      const Int_t npts);
   void AssertSpline(const Char_t* loc) const;
   
 public:
   TSnInterp1DLstSqConvo() : fFit(0), fN(0), fM(0), fS(0) {}
   TSnInterp1DLstSqConvo(const Char_t* name, const Char_t* title,
                         const UInt_t order, const UInt_t mWinWid,
                         const UInt_t derivative,
                         Double_t* const x, Double_t* const y,
                         const Int_t npts);
   TSnInterp1DLstSqConvo(const Char_t* name, const Char_t* title,
                         const UInt_t order, 
                         const UInt_t mWinWid,
                         const UInt_t derivative,
                         const TGraph* g);
   TSnInterp1DLstSqConvo(const Char_t* name, const Char_t* title,
                         const UInt_t order, 
                         const UInt_t mWinWid,
                         const UInt_t derivative,
                         const TSnCalWvData& wvd, const UChar_t ch,
                         const Bool_t vsTime=kTRUE);
   TSnInterp1DLstSqConvo(const TSnInterp1DLstSqConvo& s);
   virtual ~TSnInterp1DLstSqConvo();
   
   static
   void                   Swap(TSnInterp1DLstSqConvo& a,
                               TSnInterp1DLstSqConvo& b);
   
   TSnInterp1DLstSqConvo&  operator=(TSnInterp1DLstSqConvo o) {
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
      *this = dynamic_cast<const TSnInterp1DLstSqConvo&>(x);
   }
   
   const TSpline3*        GetSpline() const { return fFit; }
         TSpline3*        GetSpline()       { return fFit; }   
   UInt_t                 GetGramPolyOrder() const { return fN; }
   UInt_t                 GetPointsPerWindow() const { return (fM<<1)+1; }
   UInt_t                 GetMwinWid() const { return fM; }
   UInt_t                 GetDerivativeOrder() const { return fS; }
   const TMatrixT<Double_t> GetCoefficientTable() const { return fH; }
   
   // intentionally no setters -- just rebuild the whole thing
   
   ClassDef(TSnInterp1DLstSqConvo, 1); // smooth "interpolation" of the data using least square convolution with access to derivatives (like Savitzky-Golay but using Gram polynomials and including end-points)
};

#endif // SNS_TSnInterp1DLstSqConvo
