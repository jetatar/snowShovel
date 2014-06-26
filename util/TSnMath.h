#ifndef SNS_TSnMath
#define SNS_TSnMath

#include <TString.h>
#include <TMath.h>
class TH1;

#include <stdexcept>
#include <limits>


//////////////////////////////////////////////////////////////////////////
//
// $Id$
//
// TSnMath
//
// Useful math algorithms not already provided by Root.
// 
//////////////////////////////////////////////////////////////////////////

struct TSnMath {
   static const Double_t kSecPerDay;
   
   template<typename Num_t>
   static
   void GetAveVarFast(const Num_t* const data, const UInt_t n,
                      Num_t& ave, Num_t& var);
   template<typename Num_t>
   static
   void GetAveRMSFast(const Num_t* const data, const UInt_t n,
                      Num_t& ave, Num_t& rms);
   
   template<typename Num_t>
   static
   void GetAveVarTwoPass(const Num_t* const data, const UInt_t n,
                         Num_t& ave, Num_t& var);
   template<typename Num_t>
   static
   void GetAveRMSTwoPass(const Num_t* const data, const UInt_t n,
                         Num_t& ave, Num_t& rms);

   template<typename Num_t>
   static
   void GetWeightedAveVarFast(const Num_t* const data,
                              const Num_t* const weights,
                              const UInt_t n,
                              Num_t& ave, Num_t& var,
                              const Double_t epsilon=1e-20);
   
   template<typename Num_t>
   static
   void GetWeightedAveRMSFast(const Num_t* const data,
                              const Num_t* const weights,
                              const UInt_t n,
                              Num_t& ave, Num_t& rms,
                              const Double_t epsilon=1e-20);

   template<typename Num_t>
   static
   void GetWeightedAveVarTwoPass(const Num_t* const data,
                                 const Num_t* const weights,
                                 const UInt_t n,
                                 Num_t& ave, Num_t& var,
                                 const Double_t epsilon=1e-20);

   template<typename Num_t>
   static
   void GetWeightedAveRMSTwoPass(const Num_t* const data,
                                 const Num_t* const weights,
                                 const UInt_t n,
                                 Num_t& ave, Num_t& rms,
                                 const Double_t epsilon=1e-20);
   
   template<typename Num_t>
   static
   Num_t GetRange(const Num_t* const x, const UInt_t n,
                  Num_t& xmin, Num_t& xmax);

   template<typename Num_t>
   static
   Num_t GetAngleDiff(const Num_t a1, const Num_t a2) {
      Num_t diff = a2 - a1;
      static const Double_t kPiOver2 = TMath::Pi() / 2.0; // Root doesn't cache it
      while (diff < -kPiOver2) {
         diff += TMath::Pi();
      }
      while (diff > TMath::Pi()) {
         diff -= TMath::Pi();
      }
      return diff;
   }
   
   template<typename Num_t>
   static
   Num_t GetAngleDiffDeg(const Num_t a1, const Num_t a2) {
      return ( TMath::RadToDeg() * GetAngleDiff(a1*TMath::DegToRad(),
                                                a2*TMath::DegToRad()) );
   }

   static
   void  StepFromPeak(const TH1& h, const Float_t peakFrc,
                      Int_t& lo, Int_t& hi);

   static
   void  FindMeanRmsSteppingFromPeak(const TH1& h, const Float_t peakFrc,
                                     Double_t& mean, Double_t& rms,
                                     Double_t& n);

   static
   void  FindGausMeanRmsSteppingFromPeak(TH1& h, const Float_t peakFrc,
                                         Double_t& mean, Double_t& rms,
                                         Double_t& n);

   static
   void  GetNiceDateAxisFor(const UInt_t startTime, const UInt_t finishTime,
                            Int_t& nbins, Double_t& axmin, Double_t& axmax);

   template<typename Num_t>
   static
   Bool_t SearchOneToOneMapping(const Long64_t n,
                                const Num_t* const from,
                                const Num_t* const to,
                                const Num_t fromVal,
                                Num_t& toVal);

   static inline
   Double_t GenFactorial(const Int_t a, const Int_t b);

   template<typename Num_t>
   static
   Double_t GramPolynomial(const Num_t x, const UInt_t m,
                           const Int_t k, const UInt_t s);

   static inline
   Double_t LSqConvCoefficient(const Int_t i, const Int_t t,
                               const UInt_t m, const UInt_t n,
                               const UInt_t s);

   
   template<typename T>
   static
   TString GetBitsAsString(const T x) {
      TString o;
      for (Int_t i=(sizeof(T)*std::numeric_limits<unsigned char>::digits)-1;
           i>-1; i--) {
         if ( (x&(1<<i))==0 ) {
	    o += "0";
         } else {
            o += "1";
         }
      }
      return o;
   }
   
   template<typename T>
   static
   void PrintBits(const T x, const Bool_t endLine) {
      TString b = GetBitsAsString(x);
      printf("%s",b.Data());
      if (endLine) {
         printf("\n");
      }
   }

   
   
   TSnMath() {}
   virtual ~TSnMath() {}

   ClassDef(TSnMath, 0);
   
};

template<typename Num_t>
void TSnMath::GetAveVarFast(const Num_t* const data, const UInt_t n,
                            Num_t& ave, Num_t& var) {
   // get the average and unbiased variance (s^2) of the data in
   // a single loop through the data.
   // if roundoff error is an issue, use GetAveVarTwoPass instead.

   ave = var = 0;
   if (n>1) {
      const Num_t* d = data;
      const Num_t fn = static_cast<Num_t>(n);
      for (UInt_t i=0; i<n; ++i, ++d) {
         ave += *d;
         var += (*d)*(*d);
      }
      var -= (ave*ave)/fn;
      var /= fn-1.0;
      ave /= fn;
   } else {
      throw std::domain_error(Form("<TSnMath::GetAveVarFast>: "
                                   "Need more than %u points to find "
                                   "variance.",n));
   }
}

template<typename Num_t>
void TSnMath::GetAveRMSFast(const Num_t* const data, const UInt_t n,
                            Num_t& ave, Num_t& rms) {
   // get the average and RMS of the data in a single loop through the data.
   // if roundoff error is an issue, use GetAveRMSTwoPass instead.

   GetAveVarFast(data, n, ave, rms);
   rms = TMath::Sqrt(rms);
}

template<typename Num_t>
void TSnMath::GetAveVarTwoPass(const Num_t* const data, const UInt_t n,
                               Num_t& ave, Num_t& var) {
   // use corrected two-pass algorithm to minimize roundoff error
   //
   // sets ave = sample mean of the data (or the first n points of data)
   //      var = unbiased sample variance of data; s^2
   //
   // if rms is desired, use GetAveRMSTwoPass
   //
   // NUM REC C - 14.1, eqn 14.1.8
   
   ave = var = 0;
   if (n>1) {
      const Num_t* d  = data;
      const Num_t  fn = static_cast<Num_t>(n);
      for (UInt_t i=0; i<n; ++i, ++d) {
         ave += *d;
      }
      ave /= fn;
      
      d = data;
      Num_t df, tdf=0, tdf2=0;
      for (UInt_t i=0; i<n; ++i, ++d) {
         df    = *d - ave;
         tdf  += df;
         tdf2 += df*df;
      }
      var  = tdf2 - ((tdf*tdf)/fn);
      var /= fn-1.0;
   } else {
      throw std::domain_error(Form("<TSnMath::GetAveVarTwoPass>: "
                                   "Need more than %u points to find "
                                   "variance.",n));
   }
}

template<typename Num_t>
void TSnMath::GetAveRMSTwoPass(const Num_t* const data, const UInt_t n,
                               Num_t& ave, Num_t& rms) {
   // use corrected two-pass algorithm to minimize roundoff error
   //
   // sets ave = sample mean of the data (or the first n points of data)
   //      rms = square root of the unbiased variance
   //
   // see GetAveVarTwoPass

   GetAveVarTwoPass(data, n, ave, rms);
   rms = TMath::Sqrt(rms);
}

template<typename Num_t>
void TSnMath::GetWeightedAveVarFast(const Num_t* const data,
                                    const Num_t* const weights,
                                    const UInt_t n,
                                    Num_t& ave, Num_t& var,
                                    const Double_t epsilon) {
   // get the weighted average and unbiased variance (s^2) of the data in
   // a single loop through the data.
   // if roundoff error is an issue, use GetWeightedAveVarTwoPass instead.

   ave = var = 0;
   if (n>1) {
      const Num_t* d = data;
      const Num_t* w = weights;
      Num_t       tw = 0;       // sum of weights
      Num_t       ws = 0;       // sum of weights^2
      for (UInt_t i=0; i<n; ++i, ++d, ++w) {
         tw  += *w;
         ws  += (*w) * (*w);
         ave += (*d) * (*w);
         var += (*d) * (*d) * (*w);
      }
      const Num_t den = (tw*tw) - ws;
      if ( (TMath::Abs(tw)>epsilon) && (TMath::Abs(den)>epsilon) ) {
         var -= (ave*ave)/tw;
         var *= tw / den;
         ave /= tw;
      } else {
         throw std::runtime_error(Form("<TSnMath::GetWeightedAveVarFast>: "
                                       "Cannot find unbiased variance. "
                                       "Weights sum to [%g] and variance "
                                       "of weights is [%g].",
                                       static_cast<Double_t>(tw),
                                       static_cast<Double_t>(den)));
      }
   } else {
      throw std::domain_error(Form("<TSnMath::GetWeightedAveVarFast>: "
                                   "Need more than %u points to find "
                                   "variance.",n));
   }
}

template<typename Num_t>
void TSnMath::GetWeightedAveRMSFast(const Num_t* const data,
                                    const Num_t* const weights,
                                    const UInt_t n,
                                    Num_t& ave, Num_t& rms,
                                    const Double_t epsilon) {
   // get the average and unbiased RMS of the weighted data in a single loop
   // through the data.
   //
   // if roundoff error is an issue, use GetWeightedAveRMSTwoPass instead.

   GetWeightedAveVarFast(data, weights, n, ave, rms, epsilon);
   rms = TMath::Sqrt(rms);
}

template<typename Num_t>
void TSnMath::GetWeightedAveVarTwoPass(const Num_t* const data,
                                       const Num_t* const weights,
                                       const UInt_t n,
                                       Num_t& ave, Num_t& var,
                                       const Double_t epsilon) {
   // use corrected two-pass algorithm to minimize roundoff error
   //
   // sets ave = weighted sample mean of the data
   //      var = unbiased weighted sample variance of data; s^2
   //
   // if rms is desired, use GetWeightedAveRMSTwoPass
   //
   // NUM REC C - 14.1, eqn 14.1.8
   
   ave = var = 0;
   if (n>1) {
      const Num_t* d = data;
      const Num_t* w = weights;
      Num_t       tw = 0;
      Num_t       ws = 0;
      for (UInt_t i=0; i<n; ++i, ++d, ++w) {
         ave += (*d) * (*w);
         tw  += *w;
         ws  += (*w) * (*w);
      }
      const Num_t den = (tw*tw) - ws;
      if ( (TMath::Abs(tw)>epsilon) && (TMath::Abs(den)>epsilon) ) {
         ave /= tw;
         
         d = data;
         w = weights;
         Num_t df, tdf=0, tdf2=0;
         for (UInt_t i=0; i<n; ++i, ++d, ++w) {
            df    = (*d - ave);
            tdf  += df * (*w);
            tdf2 += df*df * (*w);
         }
         var  = tdf2 - ((tdf*tdf)/tw);
         var *= tw / den;
      } else {
         throw std::runtime_error(Form("<TSnMath::GetWeightedAveVarTwoPass>: "
                                       "Cannot find unbiased variance. "
                                       "Weights sum to [%g] and variance "
                                       "of weights is [%g].",
                                       static_cast<Double_t>(tw),
                                       static_cast<Double_t>(den)));
      }
   } else {
      throw std::domain_error(Form("<TSnMath::GetAveVarTwoPass>: "
                                   "Need more than %u points to find "
                                   "variance.",n));
   }
}

template<typename Num_t>
void TSnMath::GetWeightedAveRMSTwoPass(const Num_t* const data,
                                       const Num_t* const weights,
                                       const UInt_t n,
                                       Num_t& ave, Num_t& rms,
                                       const Double_t epsilon) {
   // use corrected two-pass algorithm to minimize roundoff error
   //
   // sets ave = weighted sample mean of the data
   //      rms = square root of the weighted unbiased sample variance
   //
   // see GetWeightedAveVarTwoPass

   GetWeightedAveVarTwoPass(data, weights, n, ave, rms, epsilon);
   rms = TMath::Sqrt(rms);
}

template<typename Num_t>
Num_t TSnMath::GetRange(const Num_t* const x, const UInt_t n,
                        Num_t& xmin, Num_t& xmax) {
   // find range of x
   //
   // sets xmin to the min of the data x
   // sets xmax to the max of the data x
   // returns xmax-xmin
   
   xmin = xmax = *x;
   const Num_t* dx = x;
   for (register UInt_t j=1; j<n; ++j, ++dx) {
      if (*dx > xmax) xmax = *dx;
      if (*dx < xmin) xmin = *dx;
   }
   return (xmax - xmin);
}

template<typename Num_t>
Bool_t TSnMath::SearchOneToOneMapping(const Long64_t n,
                                      const Num_t* const from,
                                      const Num_t* const to,
                                      const Num_t fromVal,
                                      Num_t& toVal) {
   // with two array corresponding to a 1-to-1 mapping
   // "from" is the array of values in which to search for the value
   // "fromVal".
   // the value "toVal" is then set to be the value of the element in
   // "to" at the index in which "fromVal" occurs in "from"
   //
   // this function can be used, for example, to invert a TGraph, if it
   // is known that the graph represents a 1-to-1 mapping:
   // SearchOneToOneMapping(graph->GetN(), graph->GetY(), graph->GetX(), y, x)
   //
   // returns false on failure (i.e. if fromVal does not occur in from)
   
   toVal = 0;
   const Long64_t idx = TMath::BinarySearch(n, from, fromVal);
   if ( (idx>=0) && (idx<n) ) {
      if (fromVal==from[idx]) {
         toVal = to[idx];
         return kTRUE;
      }
   }
   return kFALSE;
}

Double_t TSnMath::GenFactorial(const Int_t a, const Int_t b) {
   // calculate the "generalized" factorial (a)*(a-1)*...*(a-(b-1))
   // i.e. GenFactorial(7,3) = 7*6*5 = 210
   Double_t f = 1.0;
   const Int_t end = a-b;
   if ( (end>-1) && (end<=a) ) {
      for (Int_t i=a; i>end; --i) {
         f *= i;
      }
   } else {
      throw std::invalid_argument(Form("<TSnMath::GenFactorial>: "
                                       "Invalid argument(s) a=%d, b=%d.",
                                       a,b));
   }
   return f;
}

template<typename Num_t>
Double_t TSnMath::GramPolynomial(const Num_t x, const UInt_t m, 
                                 const Int_t k, const UInt_t s) {
   // calculate the s'th derivative of the Gram (discrete Chebyshev)
   // polynomial of order k evaluated at x over 2m+1 points
   //
   // note that this is recursive, so "small" values of m and k are wise
   //
   // algorith from Analytical Chemistry, vol 62, no 6, 1990, pg570
   
   if (k>0) {
      const Int_t d = (m<<1) - k + 1;
      if (d!=0) {
         
         const Double_t e = k*d;
         const Double_t pk1s  = GramPolynomial(x,m,k-1,s);
         const Double_t pk1s1 = GramPolynomial(x,m,k-1,s-1);
         const Double_t pk2s  = GramPolynomial(x,m,k-2,s);
         
         const Double_t a = static_cast<Double_t>((k<<2)-2) / e;
         const Double_t b = a * ( (x*pk1s) + (s*pk1s1) );
         const Double_t c = (k-1)*((m<<1)+k) / e;
         
         return (b - (c * pk2s));
         
      } else {
         throw std::invalid_argument(Form("<TSnMath::GramPolynomial>: "
                                          "m=%u and k=%hhu "
                                          "lead to zero in denominator.",
                                          m, k));
      }
      
   } else { 
      if ( (k==0) && (s==0) ) {
         return 1.0;
      } else {
         return 0.0;
      }
   }
}

Double_t TSnMath::LSqConvCoefficient(const Int_t i, const Int_t t,
                                     const UInt_t m, const UInt_t n,
                                     const UInt_t s) {
   // calculate the coefficient for the i'th data point of the t'th
   // least-square fit point gram polynomial, of the s'th derivative,
   // over 2m+1 points, of order n
   //
   // this is used to fit gram polynomials in successive (small) windows
   // of the data. with s=0, the data itself is simply smoothed. with
   // s=1, a smooth derivative of the data can be obtained without the
   // noise associated with finite difference numerical derivatives.
   //
   // (the least square "fit" is solved analytically)
   //
   // algorith from Analytical Chemistry, vol 62, no 6, 1990, pg570
   
   Double_t sum(0);
   
   for (UInt_t k=0; k<=n; ++k) {
      const Double_t a = (k<<1)+1;
      const Double_t b = GenFactorial(m<<1,k) / GenFactorial((m<<1)+k+1,k+1);
      const Double_t c = GramPolynomial(i, m, k, 0);
      const Double_t d = GramPolynomial(t, m, k, s);
      sum += a*b*c*d;
   }
   
   return sum;
   
}

#endif // SNS_TSnMath
