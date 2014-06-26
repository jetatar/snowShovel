#include "TSnInterp1DLstSqConvo.h"

#include <TSpline.h>
#include <TString.h>
#include <TGraph.h>

#include "TSnCalWvData.h"
#include "TSnMath.h"
#include "TSnInterp1DSpline3.h"

#include <stdexcept>
#include <utility>

ClassImp(TSnInterp1DLstSqConvo);

TSnInterp1DLstSqConvo::TSnInterp1DLstSqConvo(const Char_t* name,
                                             const Char_t* title,
                                             const UInt_t order, 
                                             const UInt_t mWinWid,
                                             const UInt_t derivative,
                                             Double_t* const x, 
                                             Double_t* const y,
                                             const Int_t npts) :
   TSnInterp1DWvData(name, title),
   fFit(0),
   fN(order), fM(mWinWid), fS(derivative),
   fH( (mWinWid<<1)+1, (mWinWid<<1)+1 ) {
   // normal ctor
   //
   // order = order of the Gram polynomials
   // mWinWid = use 2*mWinWid + 1 data points in each successive window
   //           for the Gram polynomial fits
   // derivative = 0 to smooth the data, 1 to smooth the first derivative
   //              of the data, 2 to smooth the second derivative, etc.
   //
   // The data must be evenly sampled, but this is not verified!
   //
   // A higher mWinWid value will result in more smoothing.
   //
   // Note that the order and mWinWid are not totally independent (some
   // combinations are invalid).
   //
   // The Gram polynomials are calculated recursively, so higher orders will
   // be slower.
   //
   // A typical value may be order=3, mWinWid=2, which will fit 3rd degree
   // Gram polynomials in windows of 5 data points and usually provides
   // a reasonable amount of smoothing.
   //
   // (the arrays are not const because TSpline3 is not const correct)
   BuildFromData(x,y,npts);
}

TSnInterp1DLstSqConvo::TSnInterp1DLstSqConvo(const Char_t* name,
                                             const Char_t* title,
                                             const UInt_t order, 
                                             const UInt_t mWinWid,
                                             const UInt_t derivative,
                                             const TGraph* g) :
   TSnInterp1DWvData(name, title),
   fFit(0),
   fN(order), fM(mWinWid), fS(derivative),
   fH( (mWinWid<<1)+1, (mWinWid<<1)+1 ) {
   // normal ctor
   //
   // order = order of the Gram polynomials
   // mWinWid = use 2*mWinWid + 1 data points in each successive window
   //           for the Gram polynomial fits
   // derivative = 0 to smooth the data, 1 to smooth the first derivative
   //              of the data, 2 to smooth the second derivative, etc.
   //
   // The data must be evenly sampled, but this is not verified!
   //
   // A higher mWinWid value will result in more smoothing.
   //
   // Note that the order and mWinWid are not totally independent (some
   // combinations are invalid).
   //
   // The Gram polynomials are calculated recursively, so higher orders will
   // be slower.
   //
   // A typical value may be order=3, mWinWid=2, which will fit 3rd degree
   // Gram polynomials in windows of 5 data points and usually provides
   // a reasonable amount of smoothing.
   BuildFromData(g->GetX(), g->GetY(), g->GetN());
}
   

TSnInterp1DLstSqConvo::TSnInterp1DLstSqConvo(const Char_t* name,
                                             const Char_t* title,
                                             const UInt_t order, 
                                             const UInt_t mWinWid,
                                             const UInt_t derivative,
                                             const TSnCalWvData& wvd,
                                             const UChar_t ch,
                                             const Bool_t vsTime) :
   TSnInterp1DWvData(name, title),
   fFit(0),
   fN(order), fM(mWinWid), fS(derivative),
   fH( (mWinWid<<1)+1, (mWinWid<<1)+1 ) {
   // normal ctor
   //
   // order = order of the Gram polynomials
   // mWinWid = use 2*mWinWid + 1 data points in each successive window
   //           for the Gram polynomial fits
   // derivative = 0 to smooth the data, 1 to smooth the first derivative
   //              of the data, 2 to smooth the second derivative, etc.
   //
   // The data must be evenly sampled, but this is not verified!
   //
   // A higher mWinWid value will result in more smoothing.
   //
   // Note that the order and mWinWid are not totally independent (some
   // combinations are invalid).
   //
   // The Gram polynomials are calculated recursively, so higher orders will
   // be slower.
   //
   // A typical value may be order=3, mWinWid=2, which will fit 3rd degree
   // Gram polynomials in windows of 5 data points and usually provides
   // a reasonable amount of smoothing.
   
   // need to copy to arrays of doubles
   Double_t xs[NSnConstants::kNsamps];
   Double_t ys[NSnConstants::kNsamps];
   Double_t* x = xs;
   Double_t* y = ys;
   const Float_t* d = wvd.GetData(ch);
   for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm, ++x, ++y, ++d) {
      *x = sm;
      if (vsTime) {
         *x /= NSnConstants::kSampRate;
      }
      *y = static_cast<Double_t>(*d);
   }
   BuildFromData(xs,ys,NSnConstants::kNsamps);   
}

TSnInterp1DLstSqConvo::TSnInterp1DLstSqConvo(const TSnInterp1DLstSqConvo& s) :
   fFit(0),
   fN(s.fN), fM(s.fM), fS(s.fS), fH(s.fH)
{
   // copy ctor
   if (s.fFit!=0) {
      fFit = new TSpline3(*(s.fFit));
   }
}

TSnInterp1DLstSqConvo::~TSnInterp1DLstSqConvo() {
   delete fFit;
}

void TSnInterp1DLstSqConvo::Swap(TSnInterp1DLstSqConvo& a,
                                 TSnInterp1DLstSqConvo& b) {
   std::swap(a.fFit, b.fFit);
   std::swap(a.fN, b.fN);
   std::swap(a.fM, b.fM);
   std::swap(a.fS, b.fS);
   std::swap(a.fH, b.fH);
}

void TSnInterp1DLstSqConvo::BuildFromData(Double_t* const x,
                                          Double_t* const y,
                                          const Int_t npts) {

   // see Analytical Chemistry, vol 62, no 6, 1990, pg570
   //
   // evenly sampled data points are assumed, but this is not
   // verified!
   //
   // (the arrays are not const because TSpline3 is not const correct)
   
   if (npts>1) {
      const Double_t dx = TMath::Abs(x[1] - x[0]);
      if (dx>0) {
      
         // build coefficient table
         if (fM>=static_cast<UInt_t>(kMaxInt)) {
            throw std::runtime_error(
               Form("<TSnInterp1DLstSqConvo::BuildFromData>: "
                    "fM [%u] too large to fit into integer!",fM));
         }
         const Int_t m = static_cast<Int_t>(fM);
         for (Int_t t=-m; t<=m; ++t) {
            for (Int_t i=-m; i<=m; ++i) {
               Double_t& h = fH(t+fM,i+fM);
               h = TSnMath::LSqConvCoefficient(i, t, fM, fN, fS);
            }
         }
   
         // build smoothed points
         Double_t* const smooth = new Double_t[npts];
         Double_t*          sum = smooth;
         const Int_t        max = npts-fM;
         for (Int_t i=0; i<npts; ++i, ++sum) {
            Int_t t(0);
            if (i<=m) {
               // first point(s)
               t = -fM+i;
            } else if (i>=max) {
               // last point(s)
               t = i-(max-1);
            }  // middle points t=0
            
            // get the smoothed point
            *sum = 0;
            
            for (Int_t j=-m; j<=m; j++) {
               *sum += fH(t+fM,j+fM) * (y[i+j-t]);
            }
            if (fS>0) {
               *sum /= dx;
            }
         }
         
         delete fFit;
         fFit = new TSpline3(GetTitle(), x, smooth, npts);
         
         delete[] smooth;
         
      } else {
         throw std::invalid_argument(
            "<TSnInterp1DLstSqConvo::BuildFromData>: "
            "Multiple points at same x value.");
      }
   } else {
      throw std::invalid_argument(
         Form("<TSnInterp1DLstSqConvo::BuildFromData>: "
              "Cannot build interpolation from only %d points.",
              npts));
   }
}

void TSnInterp1DLstSqConvo::AssertSpline(const Char_t* loc) const {
   if (fFit==0) {
      throw std::runtime_error(Form("<TSnInterp1DLstSqConvo::%s>: "
                                    "No spline available.", loc));
   }
}

TObject* TSnInterp1DLstSqConvo::GetInterpolator() {
   return GetSpline();
}

const TObject* TSnInterp1DLstSqConvo::GetInterpolator() const {
   return GetSpline();
}

Double_t TSnInterp1DLstSqConvo::Eval(const Double_t x) const {
   AssertSpline("Eval");
   return fFit->Eval(x);
}

Double_t TSnInterp1DLstSqConvo::FindXAtExtremum(const Bool_t min,
                                             Double_t* const ymax) const {
   AssertSpline("FindXAtExtremum");
   return TSnInterp1DSpline3::FindXAtExtremum(*fFit, min, ymax);
}
