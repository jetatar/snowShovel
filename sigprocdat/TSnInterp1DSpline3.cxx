#include "TSnInterp1DSpline3.h"

#include <TString.h>
#include <TSpline.h>

#include "TSnCalWvData.h"
#include "NSnConstants.h"

#include <algorithm>
#include <stdexcept>

ClassImp(TSnInterp1DSpline3);

TSnInterp1DSpline3::TSnInterp1DSpline3(const Char_t* name,
                                       const Char_t* title,
                                       Double_t* x, Double_t* y,
                                       const Int_t n) :
   TSnInterp1DWvData(name, title),
   fSpline(new TSpline3(title, x, y, n)) {
   // normal ctor
   // (the arrays are not const because TSpline3 is not const correct)
}

TSnInterp1DSpline3::TSnInterp1DSpline3(const Char_t* name, const Char_t* title,
                                       const TGraph* g) :
   TSnInterp1DWvData(name, title),
   fSpline(new TSpline3(title, g)) {
   // ctor from a TGraph
}

TSnInterp1DSpline3::TSnInterp1DSpline3(const Char_t* name, const Char_t* title,
                                       const TSnCalWvData& wvd,
                                       const UChar_t ch,
                                       const Bool_t vsTime) :
   TSnInterp1DWvData(name, title),
   fSpline(0) {
   // ctor from calib wave data on a channel
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
   fSpline = new TSpline3(title, xs, ys, NSnConstants::kNsamps);
}

TSnInterp1DSpline3::TSnInterp1DSpline3(const TSnInterp1DSpline3& s) :
   TSnInterp1DWvData(s),
   fSpline(new TSpline3(*(s.fSpline))) {
   // copy ctor
}

TSnInterp1DSpline3::~TSnInterp1DSpline3() {
   delete fSpline;
}

void TSnInterp1DSpline3::Swap(TSnInterp1DSpline3& a, TSnInterp1DSpline3& b) {
   std::swap(a.fSpline, b.fSpline);
}

void TSnInterp1DSpline3::SetSpline(const TSpline3& s) {
   delete fSpline;
   fSpline = new TSpline3(s);
}

void TSnInterp1DSpline3::AssertSpline(const Char_t* loc) const {
   if (fSpline==0) {
      throw std::runtime_error(Form("<TSnInterp1DSpline3::%s>: "
                                    "No spline available.", loc));
   }
}

TObject* TSnInterp1DSpline3::GetInterpolator() {
   return GetSpline();
}

const TObject* TSnInterp1DSpline3::GetInterpolator() const {
   return GetSpline();
}

Double_t TSnInterp1DSpline3::Eval(const Double_t x) const {
   AssertSpline("Eval");
   return fSpline->Eval(x);
}

Double_t TSnInterp1DSpline3::FindXAtExtremum(const Bool_t min,
                                             Double_t* const ymax) const {
   AssertSpline("FindExtremum");
   const Int_t n = fSpline->GetNp();
   // no direct access to the knot points :(
   Double_t x,y, xm(0), ym(0);
   for (Int_t i=0; i<n; ++i) {
      fSpline->GetKnot(i, x, y);
      if (i==0) {
         xm = x; ym = y;
      } else {
         // hopefully the compiler can optimize this
         if ( ( min && (y<ym)) ||
              (!min && (y>ym)) ) {
            ym = y;
            xm = x;
         }
      }
   }
   if (ymax!=0) {
      *ymax = ym;
   }
   return xm;
}

