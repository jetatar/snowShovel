#ifndef SNS_TSnSpectral
#define SNS_TSnSpectral

#include <TString.h>
#include <TMath.h>
#include <TBranch.h>
#include <TLeaf.h>

#include <stdexcept>

#include "TSnMath.h"

class TGraph;

//////////////////////////////////////////////////////////////////////////
//
// $Id$
//
// TSnSpectral
//
// Useful functions for doing spectral analysis. Generally adapted from
// Numerical Recipes in C.
// 
//////////////////////////////////////////////////////////////////////////

struct TSnSpectral {
   
   static const Double_t kTwoPi;
   static const Double_t kEpsilon;
   static const UInt_t   kMACC;

   enum EWinType_t {
      kSquare,
      kWelch,
      kBartlett
   };

   template<typename Num_t>
   static
   void DiscreteCpxFFT(Num_t* const data, const UInt_t nn,
                       const Bool_t inverse=kFALSE);
   
   template<typename Num_t>
   static
   TGraph* NewGraphFromRealFFT(const Num_t* const rfft,
                               const UInt_t n,
                               const Num_t sampDt);

   template<typename Num_t>
   static
   TGraph* NewRealFFTGraph(const Num_t* const data,
                           const UInt_t n,
                           const Num_t sampDt);
   
   static
   void GetRealFFTFreqBins(const UInt_t n, const Double_t sampDt,
                           Int_t& nFreqBins,
                           Double_t& freqMin, Double_t& freqMax);
   
   template<typename Num_t>
   static
   void RealFFT(Num_t* const data, const UInt_t n, 
                const Bool_t inverse=kFALSE);

   template<typename Num_t>
   static
   void RealFFTSimul(const Num_t* data1, const Num_t* data2,
                     Num_t* fft1, Num_t* fft2,
                     const UInt_t n);

   template<typename Num_t>
   static
   Num_t PearsonSmpCorrCoef(const Num_t* const data1,
                            const Num_t* const data2,
                            const UInt_t n);

   template<typename Num_t>
   static
   TGraph* NewScanPrsCorrCoefGraphUneq(const Num_t* const data1,
                                       const UInt_t n1,
                                       const Num_t* const data2,
                                       const UInt_t n2);

   template<typename Num_t>
   static
   TGraph* NewScanPrsCorrCoefGraph(const Num_t* const data1,
                                   const Num_t* const data2,
                                   const UInt_t n,
                                   const UInt_t winmin=0,
                                   const UInt_t winmax=0,
                                   const Bool_t shiftInWin=kTRUE);

    template<typename Num_t>
    static
    void PearsonSmpCorrCoefCirc(    std::vector<Num_t>& data1,
                                    std::vector<Num_t>& data2,
                                    std::vector<Num_t>& ans );


   template<typename Num_t>
   static 
   void Correlate(const Num_t* data1, const Num_t* data2,
                  const UInt_t n,
                  Num_t* ans);
   
   template<typename Num_t>
   static
   TGraph* NewCorrelateGraph(const Num_t* data1, const Num_t* data2,
                             const UInt_t n,
                             const Bool_t normalize);

   template<class DSource_t, typename Num_t>
   static
   void Spectrum(DSource_t* br, Num_t* const p, const Int_t m,
                 const Int_t k, const Bool_t overlap,
                 const Int_t firstEntry,
                 const TSnSpectral::EWinType_t window);

   template<typename Num_t>
   static
   void SlowLomb(const Num_t* const x, const Num_t* const y,
                 const UInt_t n,
                 const Float_t ofac,  // oversampling factor (e.g. >=4)
                 const Float_t hifac, // max frequency = hifac * <f_c>
                 Num_t* const px, Num_t* const py, // output
                 const UInt_t np, // output
                 UInt_t&  nout,
                 UInt_t&  jmax,
                 Num_t& prob);

   template<typename Num_t>
   static
   void FastLomb(const Num_t* const x, const Num_t* const y,
                 const UInt_t n,
                 const Float_t ofac, const Float_t hifac,
                 Num_t* const wk1, Num_t* const wk2,
                 const UInt_t nwk,
                 UInt_t& nout, UInt_t& jmax, Num_t& prob);

   template<typename Num_t>
   static
   void HilbertCpxAnalyticSignal(Num_t* const x, const UInt_t n);

   template<typename Num_t>
   static
   void EnvelopeReal(Num_t* const data, const UInt_t n);
   

   template<typename Num_t>
   static
   void Spread(const Num_t y, Num_t* const yy, const UInt_t n,
               const Num_t x, const UInt_t m);
   

   template<typename Num_t>
   static 
   Num_t SquareWin(const Int_t i, const Num_t a, const Num_t b) {
      return 1.0;
   }
   
   template<typename Num_t>
   static 
   Num_t WelchWin(const Int_t i, const Num_t a, const Num_t b) {
      const Num_t d = (i-a)*b;
      return (1.0 - (d*d));
   }
   
   template<typename Num_t>
   static 
   Num_t BartlettWin(const Int_t i, const Num_t a, const Num_t b) {
      const Num_t d = (i-a)*b;
      return (1.0 - TMath::Abs(d));
   }
   
   // helper functions so we don't have to #include <TGraph.h> in
   // this header file
   static
   TGraph* NewGraph(const Int_t i);
   static
   void    SetPoint(TGraph* gr, const Int_t i,
                    const Double_t x, const Double_t y);

   TSnSpectral() {}
   virtual ~TSnSpectral() {}
  
   ClassDef(TSnSpectral, 0);


 private:
   template<typename Num_t>
   static
   Bool_t CheckDataType(TBranch* br, Num_t& brdata);

   template<class Hist_t, typename Num_t>
   static
   Bool_t CheckDataType(Hist_t* h, Num_t& brdata);

   template<typename Num_t>
   static
   void CopyDataTo(TBranch* br, const Int_t entry,
                   Num_t& source, Num_t& dest);

   template<class Hist_t, typename Num_t>
   static
   void CopyDataTo(Hist_t* h, const Int_t bin, Num_t& source, Num_t& dest);

};

template<typename Num_t>
void TSnSpectral::DiscreteCpxFFT(Num_t* const data, const UInt_t nn,
                                 const Bool_t inverse) {
   // replace the data in the 'data' array by its discrete, complex-valued FFT
   // 
   // if inverse is true, will do the inverse FFT
   // (unlike the function provided in NumRecC, the inverse FFT will
   // be provided already be mulitplied by 1/n)
   // 
   // data must be [0..2*nn-1]
   // nn must be a power of two
   // the data is organized like: [ real_0, img_0, real_1, img_1, ... ]
   //
   // NUM REC C - 12.2, pg 507-508
   
   if ( ((nn-1)&nn)!=0 ) {
      throw std::invalid_argument(Form("<TSnSpectral::DiscreteCpxFFT>: "
                                       "nn (=%u) must be a power of 2.",nn));
   } else {
   
      const UInt_t n = nn<<1;  // nn * 2
      Num_t tempr,tempi;
      UInt_t m;
      register UInt_t i,j=1;
      Num_t* di=data, * di1=data+1, * dj, * dj1;
      for (i=1; i<n; i+=2, di+=2, di1+=2) {
         if (j > i) {
            dj    = data + j - 1;
            dj1   = dj + 1;
            tempr = *dj;  *dj  = *di;  *di  = tempr;
            tempr = *dj1; *dj1 = *di1; *di1 = tempr;
         }
         m = nn;
         while (m>1 && j>m) {
            j -= m;
            m >>= 1;
         }
         j += m;
      }
   
      Double_t wtemp, wr, wpr, wpi, wi, theta;
      UInt_t mmax=2, istep;
      const Int_t isign = inverse ? -1 : 1;
      while (n > mmax) {
         istep = mmax<<1;
         theta = isign * (kTwoPi / mmax);
         wtemp = TMath::Sin(0.5*theta);
         wpr   = -2.0 * wtemp * wtemp;
         wpi   = TMath::Sin(theta);
         wr    = 1.0;
         wi    = 0.0;
         for (m=1; m<mmax; m+=2) {
            for (i=m; i<=n; i+=istep) {
               j     = i + mmax;
               dj    = data + j - 1;
               dj1   = dj+1;
               di    = data + i - 1;
               di1   = di+1;
               tempr = wr * (*dj)  - wi*(*dj1);
               tempi = wr * (*dj1) + wi*(*dj);
               *dj   = *di  - tempr;
               *dj1  = *di1 - tempi;
               *di  += tempr;
               *di1 += tempi;
            }
            wtemp = wr;
            wr = wtemp*wpr - wi*wpi    + wr;
            wi = wi*wpr    + wtemp*wpi + wi;
         }
         mmax = istep;
      }
      if (inverse) {
         // scale amplitude back
         di = data;
         const Num_t tn = 2.0/static_cast<Num_t>(n);
         for (i=0; i<n; ++i, ++di) {
            *di *= tn;
         }
      }
   }
}

template<typename Num_t>
TGraph* TSnSpectral::NewRealFFTGraph(const Num_t* const data,
                                     const UInt_t n,
                                     const Num_t sampDt) {
   // computes the real fft from the data and returns a new
   // TGraph that shows the real fft vs the frequency
   //
   // sampDt is the width of each sample in the time domain
   //
   // see RealFFT and NewGraphFromRealFFT
   
   Num_t* rfft = new Num_t[n];
   memcpy(rfft, data, n*sizeof(Num_t));
   RealFFT(rfft, n);
   TGraph* gr = NewGraphFromRealFFT(rfft, n, sampDt);
   delete[] rfft;
   return gr;
}

template<typename Num_t>
TGraph* TSnSpectral::NewGraphFromRealFFT(const Num_t* const rfft,
                                         const UInt_t n,
                                         const Num_t sampDt) {
   // rfft is the list of fft coefficients given by RealFFT (length n) 
   // sampDt is the width of each sample in the time domain
   //
   // returns a TGraph of sqrt(power) vs frequency
   // this function calls new, so the caller is responsible for
   // deleting the TGraph
   
   TGraph* gr(0);
   if (rfft!=0) {
      const UInt_t hn = n>>1; // n/2
      const Num_t  fi = 1.0 / static_cast<Num_t>(sampDt*n);
      
      UInt_t j=0;
      gr = NewGraph(hn+1);
      SetPoint(gr, j++, 0, TMath::Abs(rfft[0]));
      const Num_t* fe=rfft+2;
      const Num_t* fo=fe+1;
      Num_t  fr=fi;
      for (UInt_t i=2; i<n; i+=2, fe+=2, fo+=2, fr+=fi) {
         SetPoint(gr, j++, fr, TMath::Sqrt(((*fe)*(*fe))+((*fo)*(*fo))));
      }
      SetPoint(gr, j++, fr, TMath::Abs(rfft[1]));
      
   } else {
      throw std::invalid_argument(Form("<TSnSpectral::RealFFTFreqs>: "
                                       "rfft is a null array"));
   }
   return gr;
}

template<typename Num_t>
void TSnSpectral::RealFFT(Num_t* const data, const UInt_t n, 
                          const Bool_t inverse) {
   // replace the real-valued data with the positive frequency half of its
   // complex fourier transform
   //
   // the real valued first & last components of the complex transform
   // are stored in data[0] and data[1], respectively
   //
   // n must be a power of 2
   //
   // data goes from [0..n-1]
   // 
   // if inverse is true, will perform the inverse FFT
   // (unlike the function provided in NumRecC, the inverse FFT will
   // be provided already be mulitplied by 2/n)
   //
   // NUM REC C - 12.3, pg 513-514

   if ( ((n-1)&n)!=0 ) {
      throw std::invalid_argument(Form("<TSnSpectral::RealFFT>: "
                                       "n (=%u) must be a power of 2.",n));
   } else {
   
      Double_t theta = TMath::Pi() / static_cast<Double_t>(n>>1);
      Float_t c1=0.5, c2;
      if (inverse) {
         c2 = 0.5;
         theta = -theta;     // set up for inverse tranform
      } else {
         c2 = -0.5;
         DiscreteCpxFFT(data,n>>1,kFALSE); // forward FFT
      }
   
      Double_t wtemp     = TMath::Sin(0.5*theta);
      const Double_t wpr = -2.0*wtemp*wtemp;
      const Double_t wpi = TMath::Sin(theta);
      Double_t wr        = 1.0+wpr;
      Double_t wi        = wpi;
      Float_t h1r, h1i, h2r, h2i;
      const UInt_t n4  = n>>2; // n/4
      register UInt_t i;
      Num_t* d1 = data+2,   * d2 = d1+1,
         * d3 = data+n-2, * d4 = d3+1;
      for (i=1; i<n4; ++i, d1+=2, d2+=2, d3-=2, d4-=2) {
         h1r   =  c1 * ((*d1)+(*d3));
         h1i   =  c1 * ((*d2)-(*d4));
         h2r   = -c2 * ((*d2)+(*d4));
         h2i   =  c2 * ((*d1)-(*d3));
         *d1   =  h1r + wr*h2r - wi*h2i;
         *d2   =  h1i + wr*h2i + wi*h2r;
         *d3   =  h1r - wr*h2r + wi*h2i;
         *d4   = -h1i + wr*h2i + wi*h2r;
         wtemp = wr;
         wr    = wtemp*wpr - wi*wpi    + wr;
         wi    = wi*wpr    + wtemp*wpi + wi;
      }
   
      // now fill in the beginning and end of the array
      if (inverse) {
         h1r   = *data;
         d1    = data+1;
         *data = c1*(h1r + (*d1));
         *d1   = c1*(h1r - (*d1));
         DiscreteCpxFFT(data,n>>1,inverse);       // inverse tranform
      } else {
         h1r   = *data;
         d1    = data+1;
         *data = h1r + (*d1);
         *d1   = h1r - (*d1);
      }
   }
}

template<typename Num_t>
void TSnSpectral::RealFFTSimul(const Num_t* data1, const Num_t* data2,
                               Num_t* fft1, Num_t* fft2,
                               const UInt_t n) {
   // calculate the FFT of two real-valued data sets at once
   //
   // data1 and data2 are two real input arrays
   // call DiscreteCpxFFT and return two complex fft output arrays
   //
   // data must be [0..n-1]
   // fft  must be [0..2*n-1]
   // n must be a power of 2
   //
   // NUM REC C - 12.3, pg 511
   
   if ( ((n-1)&n)!=0 ) {
      throw std::invalid_argument(Form("<TSnSpectral::RealFFTSimul>: "
                                       "n (=%u) must be a power of 2.",n));
   } else {
   
      const Num_t* d1j=data1, * d2j=data2;
      Num_t * f1e=fft1, * f1o=fft1+1;
      register UInt_t j;
      for (j=0; j<n; ++j, ++d1j, ++d2j, f1e+=2, f1o+=2) {
         *f1e = *d1j;
         *f1o = *d2j;
      }

      DiscreteCpxFFT(fft1,n,kFALSE); // transform

      Float_t rep, rem, aip, aim;
      fft2[0] = fft1[1];
      fft1[1] = fft2[1] = 0.0;
      f1e = fft1+2;
      f1o = fft1+3;
      Num_t* f2e = fft2+2, * f2o = fft2+3;
      Num_t* vb1 = fft1+n+n-1;
      Num_t* bb1 = vb1-1;
      Num_t* vb2 = fft2+n+n-1;
      Num_t* bb2 = vb2-1;
      for (j=2; j<=n; j+=2, f1e+=2, f1o+=2, f2e+=2, f2o+=2,
              vb1-=2, vb2-=2, bb1-=2, bb2-=2) {
         rep = 0.5*((*f1e)+(*bb1));
         rem = 0.5*((*f1e)-(*bb1));
         aip = 0.5*((*f1o)+(*vb1));
         aim = 0.5*((*f1o)-(*vb1));
         *f1e =  rep;
         *f1o =  aim;
         *bb1 =  rep;
         *vb1 = -aim;
         *f2e =  aip;
         *f2o = -rem;
         *bb2 =  aip;
         *vb2 =  rem;
      }
   }
}

template<typename Num_t>
void TSnSpectral::Correlate(const Num_t* data1, const Num_t* data2,
                            const UInt_t n,
                            Num_t* ans) {
   // compute correlation of data sets
   //
   // data must be [0..n-1]  (length n)
   // ans must be [0..2*n-1] (length 2n)
   //
   // result is in first half of ans:
   // 0 shift    = ans[0]
   // +1 shift   = ans[1]
   // +2 shift   = ans[2]
   // +N/2 shift = ans[(n/2)]
   // -N/2 shift = ans[n-(n/2)] = ans[n/2]
   // -2 shift   = ans[n-2]
   // -1 shift   = ans[n-1]
   //
   // NOTE: to get the right answer, your data may need to be zero-padded
   // appropriately. See NUM REC C 13.1, pgs 540-542 for discussion.
   // The data is assumed to be periodic, with period n.
   //
   // NUM REC C - 13.2, pg 546
   
   if (ans!=0)  {
      Num_t* fft = new Num_t[n<<1];
      RealFFTSimul(data1, data2, fft, ans, n); // get both transforms
      // unlike in NumRecC, we don't divide the terms by
      // N/2, because our inverse FFT gives the proper normalization
      Num_t* ae = ans, * ao = ans+1;
      Num_t* fe = fft, * fo = fft+1;
      Num_t dum;
      for (register UInt_t i=1; i<n+2; i+=2, ao+=2, ae+=2, fe+=2, fo+=2) {
         dum = *ae;
         *ae = (((*fe)*(dum)) + ((*fo)*(*ao)));
         *ao = (((*fo)*(dum)) - ((*fe)*(*ao)));
      }
      ans[1] = ans[n];
      RealFFT(ans, n, kTRUE);
      delete[] fft;
   } else {
      throw std::invalid_argument(Form("<TSnSpectral::Correlate>: "
                                       "ans array null"));
   }
}

template<typename Num_t>
TGraph* TSnSpectral::NewCorrelateGraph(const Num_t* data1, const Num_t* data2,
                                       const UInt_t n,
                                       const Bool_t normalize) {
   // correlate the two data sets and return a TGraph that
   // plots the correlation strength vs the sample shift amount
   //
   // data1 and data2 must both contain n numbers
   //
   // normalize: if true, the normalized correlation is returned
   //
   // note that 'new' is called on the TGraph, so it is the caller's
   // responsibility to delete the returned graph
   
   Num_t* ans = new Num_t[n<<1];
   if (normalize) {
      // make normalized copies of the data
      // we could divide one by n, but the SimulFFT used in
      // Correlate wants functions that have about the same magnitude
      // to avoid roundoff errors. so we will divide by n after correlation
      Num_t* ndat1 = new Num_t[n];
      Num_t* ndat2 = new Num_t[n];
      Num_t* nd1 = ndat1, * nd2 = ndat2;
      const Num_t*  d1 = data1, *  d2 = data2;
      Num_t m1, r1, m2, r2;
      TSnMath::GetAveRMSFast(data1, n, m1, r1);
      TSnMath::GetAveRMSFast(data2, n, m2, r2);
      for (UInt_t i=0; i<n; ++i, ++nd1, ++nd2, ++d1, ++d2) {
         *nd1  = (*d1 - m1)/r1;
         *nd2  = (*d2 - m2)/r2;
      }
      // do the correlation
      Correlate(ndat1, ndat2, n, ans);
      // cleanup the copies
      delete[] ndat1;
      delete[] ndat2;
      // normalize the answer
      Num_t* a = ans;
      const Double_t nf = static_cast<Double_t>(n);
      for (UInt_t i=0; i<n; ++i, ++a) {
         *a /= (nf-1.0);
      }
   } else {
      Correlate(data1, data2, n, ans);
   }
   const UInt_t hn = n>>1;
   TGraph* g = NewGraph(n);
   UInt_t k=0;
   for (Int_t i=-hn, j=hn; k<n; ++i, ++j, ++k) {
      SetPoint(g, k, i, (i<0) ? ans[j] : ans[i]);
   }
   delete[] ans;
   return g;
}

template<typename Num_t>
Num_t TSnSpectral::PearsonSmpCorrCoef(const Num_t* const data1,
                                      const Num_t* const data2,
                                      const UInt_t n) {
   // calculate the Pearson sample correlation coefficient
   // between the two data sets, each of size n
   //
   // uses a single pass algorithm
   //
   // returns the correlation coefficient, bounded between [-1,+1]
   //  -1 => perfect anti-correlation
   //  +1 => perfect correlation
   //   0 => no correlation
   // error on this value is (1-cor^2)/sqrt(n-1)
   //
   // Note: if calling from Python, use array.array not numpy.array

   const Num_t* d1=data1;
   const Num_t* d2=data2;
   Num_t a1(0), a2(0), r1(0), r2(0), cor(0), ncr(n);
   for (UInt_t i=0; i<n; ++i, ++d1, ++d2) {
      a1  += *d1;
      a2  += *d2;
      r1  += (*d1)*(*d1);
      r2  += (*d2)*(*d2);
      cor += (*d1)*(*d2);
   }
   if (ncr>1.0) {
      // rms
      r1  -= (a1*a1)/ncr;
      r2  -= (a2*a2)/ncr;
      r1  /= ncr-1.0;
      r2  /= ncr-1.0;
      r1   = TMath::Sqrt(r1);
      r2   = TMath::Sqrt(r2);
      // ave
      a1  /= ncr;
      a2  /= ncr;
      // cor
      cor -= ncr*a1*a2;
      if ((r1>0.) && (r2>0.)) {
         cor /= (ncr-1.0)*r1*r2;
      } else {
         cor = 0;
      }
   }
   return cor;
}

template<typename Num_t>
TGraph* TSnSpectral::NewScanPrsCorrCoefGraphUneq(const Num_t* const data1,
                                                 const UInt_t n1,
                                                 const Num_t* const data2,
                                                 const UInt_t n2) {
   // shifts the data1 waveform between:
   //   -n1/2 .. 0 .. n2-n1/2
   //
   // similar to NewScanPrsCorrCoefGraph, but never shifts into the window
   // and can scan unequal windows of the two waveforms
   
   TGraph* gr(0);
   const Num_t* end1  = data1+n1;
   const Num_t* end2  = data2+n2;
   const Int_t  n1h   = n1>>1;    // n1/2
   const Int_t  maxsh = n2-n1h;
   gr = NewGraph(n2+1);
   Int_t i=0;
   const Num_t* d1, * d2;
   for (Int_t sh=-n1h; sh<maxsh; ++sh, ++i) {
      d1 = data1;
      d2 = data2-sh;
      for (; (d1<end1) && (d2<end2); ++d1, ++d2) {
         if (d2>=data2) {
            SetPoint(gr, i, sh,
                     PearsonSmpCorrCoef(d1,d2,
                                        TMath::Min(data1+n1-d1,data2+n2-d2)));
            break;
         }
      }
   }
   return gr;
   
}
                                             

template<typename Num_t>
TGraph* TSnSpectral::NewScanPrsCorrCoefGraph(const Num_t* const data1,
                                             const Num_t* const data2,
                                             const UInt_t n,
                                             const UInt_t winmin/*=0*/,
                                             const UInt_t winmax/*=0*/,
                                             const Bool_t shiftInWin/*=kTRUE*/
                                             ) {
   // shifts data2 w.r.t. data1 by amounts: -n/2 .. 0 .. n/2
   // the shifts do NOT wrap!
   // stores the correlation coefficients and shift amounts in the
   // resulting graph
   // see PearsonSmpCorrCoef
   //
   // winmin,winmax,shiftInWin are only used to scan a sub-window of the data.
   // for example:
   //     128 data samples total, stored arrays in wv1 and wv2.
   //     (e.g. Float_t* wv1 = ...)
   //   (a) we want to test shifts across the entire waveforms:
   //      NewScanPrsCorrCoefGraph(wv1, wv2, 128)
   //   (b) the interesting part of wv1 is in bins 20-59 (40 bins long).
   //       we want to tests shifts of wv2 that may bring its data into this
   //       window; (e.g. a shift of +1 would compare wv2[19] and wv1[20]):
   //      NewScanPrsCorrCoefGraph(wv1, wv2, 128, 20, 59, kTRUE)
   //   (c) same setup as (a), but this time we ONLY want to use data from
   //       the interesting bins in BOTH waveforms (i.e. no shifting into the
   //       window)
   //      NewScanPrsCorrCoefGraph(wv1, wv2, 128, 20, 59, kFALSE)
   //       or equivalently:
   //      NewScanPrsCorrCoefGraph(wv1+20, wv2+20, 40)
   //
   // the graph is newly allocated and it is up to the caller to delete it!
   
   TGraph* gr(0);
   if ( (winmax<n) && (winmin<=winmax) ) {
      const UInt_t winsz = TMath::Min(winmax - winmin + 1, n);
      const Int_t nh = (winsz>1) ? (winsz>>1) : (n>>1);
      gr = NewGraph((nh<<1)+1);
      Int_t i=0;
      const Num_t* d1, * d2;
      for (Int_t sh=-nh; sh<=nh; ++sh, ++i) {
         d1 = data1+winmin;
         d2 = data2+winmin-sh;
         for (UInt_t drop=0; drop<n; ++drop, ++d1, ++d2) {
            if (shiftInWin) {
               if (d2>=data2) {
                  if ( (d1<=(data1+n)) && (d2<=(data2+n)) ) {
                     SetPoint(gr, i, sh, PearsonSmpCorrCoef(d1,d2,
                                         TMath::Min(data1+n-d1,data2+n-d2)));
                  }
                  break;
               }
            } else {
               if (d2>=(data2+winmin)) {
                  if ( (d1<=(data1+winmax)) && (d2<=(data2+winmax)) ) {
                     SetPoint(gr, i, sh, PearsonSmpCorrCoef(d1,d2,
                                         TMath::Min(data1+winmax-d1,
                                                    data2+winmax-d2)));
                  }
                  break;
               }
            }
         }
      }
   } else {
      throw std::invalid_argument(
         Form("<TSnSpectral::NewScanPrsCorrCoefGraph>: "
              "Improper window (%u, %u) with %u elements.",
              winmin, winmax, n));
   }
   return gr;
}


template<typename Num_t> void TSnSpectral::PearsonSmpCorrCoefCirc(
                                                std::vector<Num_t>& data1,
                                                std::vector<Num_t>& data2,
                                                std::vector<Num_t>& ans )
{
    const UInt_t sz = data1.size( );
    ans.resize(sz);
                  
    std::vector<Num_t> dt2( sz );
                                   
    for( UInt_t i = 0; i < sz; i++ )
    {
        const UInt_t nn = sz + i;
        for( UInt_t j = i; j < nn; j++ )
        {
            if( j < sz )
                dt2[j] = data2[j-i];
            else
         	dt2[j-sz] = data2[j-i];
        }
/*      Float_t* dat1 = new Float_t[sz];
        Float_t* dat2 = new Float_t[sz];

        for( UInt_t j = 0; j < sz; j++ )
        {
            dat1[j] = data1[j];
            dat2[j] = dt2[j];

//          std::cout << dat1[j] << ' ' << dat2[j] << std::endl;
        }
*/
  	ans[i] = PearsonSmpCorrCoef(    &(*(data1.begin())),
                                        &(*(dt2.begin())), sz );

//      delete dat1[], dat2[];
    }
}



template<class DSource_t, typename Num_t>
void TSnSpectral::Spectrum(DSource_t* br, Num_t* const p, const Int_t m,
                           const Int_t k, const Bool_t overlap,
                           const Int_t firstEntry,
                           const TSnSpectral::EWinType_t window) {
   // from NUM REC C - 13.4, pg 557
   //
   // 'br' can be either a TBranch or a TH1[F,D,..]. for a histogram,
   // 'firstEntry' should be 1, since 0 is the underflow bin. note that
   // a pointer to the base class TH1 will NOT work (as it does not contain
   // functions for accessing the data arrays)
   //
   // the value of data in branch/histogram should be the
   // amplitude of the (evenly sampled) waveform in each bin
   //
   // sets p to the data's power at frequency i/(2*m) for i=0..m-1
   // power = mean square amplitude (eqn 13.4.5)
   //
   // overlap = true  ==> use (2*k + 1)*m data points
   // overlap = false ==> use 4*k*m data points
   //
   // window indicates how data points should be weighted
   
   
   if ( ((m-1)&m)!=0 ) {
      throw std::invalid_argument(Form("<TSnSpectral::Spectrum>: "
                                       "m (=%u) must be a power of 2.",m));
      return;
   }
   
   if (br==0) {
      throw std::invalid_argument("<TSnSpectral::Spectrum>: "
                                  "Null hist/branch.");
      return;
   }
   
   Num_t brdata;
   const Bool_t dtok = CheckDataType(br, brdata);
   if (dtok==kFALSE) {
      throw std::invalid_argument("<TSnSpectral::Spectrum>: "
                                  "Data type mismatch.");
      return;
   }
   
   Int_t entry = firstEntry;
   if (firstEntry==0 && br->IsA()->InheritsFrom("TH1")) {
      throw std::invalid_argument("<TSnSpectral::Spectrum>: "
              "firstEntry==0 requested for histogram (underflow bin)");
      return;
   }
   
   // choose the window function
   typedef Num_t (*Windowing_t)(const Int_t, const Num_t, const Num_t);
   Windowing_t winfcn = 0;
   switch (window) {
      case kSquare:
         winfcn = &(TSnSpectral::SquareWin);
         break;
      case kWelch:
         winfcn = &(TSnSpectral::WelchWin);
         break;
      case kBartlett:
         winfcn = &(TSnSpectral::BartlettWin);
         break;
      default:
         throw std::invalid_argument(Form("<TSnSpectral::Spectrum>: "
               "Unknown window type [%d]",static_cast<Int_t>(window)));
         return;
   };
   
   const Int_t mm  = m+m,
               m4  = mm+mm,
               m43 = m4+3;
   
   Num_t* const w1  = new Num_t[m4],
        * const w2  = new Num_t[m],
        *       dw1,
        *       dw2,
        *       dw3,
        *       dw4,
        *       dp;
   
   const Float_t facm = static_cast<Float_t>(m),
                 facp = 1.0 / facm;
   
   // sum weights sqd
   Num_t sumw=0, t, den=0;
   register Int_t j, kk, joff, joffn, j2;
   for (j=0; j<mm; ++j) {
      t     = winfcn(j, facm, facp);
      sumw += t*t;
   }
   
   // initialize spectrum to 0
   memset(p, 0, m*sizeof(Num_t));
   
   if (overlap) {
      // initalize the "save" half-buffer
      dw2 = w2;
      for (j=0; j<m; ++j, ++dw2) {
	 CopyDataTo(br, entry++, brdata, *dw2);
      }
   }
   
   for (kk=0; kk<k; ++kk) {
      for (joff=0; joff<2; ++joff) {
         if (overlap) {
            
            dw2 = w2;
            for (j=0; j<m; ++j, ++dw2) {
               *(w1 + joff+j+j) = *dw2;
            }
            dw2 = w2;
            for (j=0; j<m; ++j, ++dw2) {
	       CopyDataTo(br, entry++, brdata, *dw2);
            }
            joffn = joff+mm;
            dw2 = w2;
            for (j=0; j<m; ++j, ++dw2) {
               *(w1 + joffn+j+j) = *dw2;
            }
            
         } else {
            
            dw1 = w1+joff;
            for (j=joff; j<m4; j+=2, dw1+=2) {
	       CopyDataTo(br, entry++, brdata, *dw1);
            }
            
         }
      }
      // apply the window
      for (j=0; j<mm; ++j) {
         j2        = j+j;
         t         = winfcn(j, facm, facp);
         dw1       = w1 + j2;
         *(dw1+1) *= t;
         *dw1     *= t;
      }
      
      DiscreteCpxFFT(w1,mm,kFALSE); // FFT windowed data
      
      dw1 = w1+1;
      *p += ((*w1)*(*w1)) + ((*dw1)*(*dw1));
      
      dp = p+1;
      for (j=1; j<m; ++j, ++dp) {
         j2   = j+j;
         
         dw1  = w1+j2;
         dw2  = dw1+1;
         dw3  = w1+m43-j2;
         dw4  = dw3-1;
         
         *dp +=   ((*dw1)*(*dw1)) + ((*dw2)*(*dw2))
                + ((*dw3)*(*dw3)) + ((*dw4)*(*dw4));
      }
      den += sumw;
   }
   den *= m4;
   
   dp = p;
   for (j=0; j<m; ++j, ++dp) {
      *dp /= den;
   }
   
   delete[] w1;
   delete[] w2;
   
}

template<class Hist_t, typename Num_t>
Bool_t TSnSpectral::CheckDataType(Hist_t* h, Num_t& brdata) {
   // check the bin type of the hist
   
   if (sizeof(Num_t)!=sizeof(h->At(0))) {
      throw std::invalid_argument(Form("<TSnSpectral::CheckDataType>: "
            "Size of data type in hist [%s] is [%d], while "
            "size of array type is [%d]",
            h->GetName(), static_cast<Int_t>(sizeof(h->At(0))), 
               static_cast<Int_t>(sizeof(Num_t))));
      return kFALSE;
   } else {
      return kTRUE;
   }
}

template<typename Num_t>
Bool_t TSnSpectral::CheckDataType(TBranch* br, Num_t& brdata) {
   // check branch is ok and call SetAddress
   
   const TObjArray* leafs = br->GetListOfLeaves();
   if (leafs->GetEntries() > 1) {
      throw std::invalid_argument("<TSnSpectral::CheckDataType>: "
                                  "Branch holds more than one leaf.");
      return kFALSE;
   }
   const TLeaf* lf = dynamic_cast<TLeaf*>(leafs->At(0));
   if (lf->GetLenType() != sizeof(Num_t)) {
      throw std::invalid_argument(Form("<TSnSpectral::CheckDataType>: "
            "Size of data type in branch [%s] is [%d], while "
            "size of array type is [%d]",
            br->GetName(), lf->GetLenType(), static_cast<Int_t>(
               sizeof(Num_t))));
      return kFALSE;
   }
   
   br->SetAddress(&brdata);
   return kTRUE;
}

template<typename Num_t>
void TSnSpectral::CopyDataTo(TBranch* br, const Int_t entry,
                             Num_t& source, Num_t& dest) {
   // GetEntry from br, into 'source'. copy to 'dest'.
   // DOES NOT increment entry
   br->GetEntry(entry);
   dest = source;
}

template<class Hist_t, typename Num_t>
void TSnSpectral::CopyDataTo(Hist_t* h, const Int_t bin,
                             Num_t& source, Num_t& dest) {
  // GetBinContent from h at 'bin' and copy to 'dest'.
  // WARNING: all boundry checks are circumvented for speed purposes!
  //
  // 'source' is not used at all; merely present to overload the
  // TBranch version.

  dest = h->GetArray()[bin];
}

template<typename Num_t>
void TSnSpectral::SlowLomb(const Num_t* const x, const Num_t* const y,
                           const UInt_t n,
                           const Float_t ofac,  // oversampling factor (e.g. >=4)
                           const Float_t hifac, // max frequency = hifac * <f_c>
                           Num_t* const px, Num_t* const py, // output
                           const UInt_t np, // output
                           UInt_t&  nout,
                           UInt_t&  jmax,
                           Num_t& prob) {
   // NUM REC C - 13.8, pg 579-581
   
   nout = static_cast<UInt_t>(0.5*ofac*hifac*n);
   if (nout > np) {
      throw std::invalid_argument("<TSnSpectral::SlowLomb>: "
                                  "Output arrays too small.");
      return;
   }
   
   Num_t ave, var;
   TSnMath::GetAveVarTwoPass(y, n, ave, var);
   if (var < kEpsilon) {
      throw std::invalid_argument("<TSnSpectral::SlowLomb>: "
                                  "No variance.");
      return;
   }
   
   // find range of x
   Num_t xmin=*x, xmax=*x;
   const Num_t xdif = TSnMath::GetRange(x, n, xmin, xmax);
   const Num_t xave = 0.5*(xmax+xmin);
   
   const Num_t pincr = 1.0/(xdif*ofac);
   Num_t pnow=pincr; // starting freq
   
   // trig recurrences
   Double_t  arg;
   Double_t* const wi   = new Double_t[n];
   Double_t* const wpi  = new Double_t[n];
   Double_t* const wpr  = new Double_t[n];
   Double_t* const wr   = new Double_t[n];
   Double_t* dwi        = wi;
   Double_t* dwpi       = wpi;
   Double_t* dwpr       = wpr;
   Double_t* dwr        = wr;
   const Num_t* dx=x, * dy=y;
   register UInt_t i,j;
   for (j=0; j<n; ++j, ++dx, ++dwi, ++dwpi, ++dwpr, ++dwr) {
      arg   = kTwoPi * ( (*dx - xave) * pnow );
      *dwpr = TMath::Sin(0.5*arg);
      *dwpr = -2.0*((*dwpr)*(*dwpr));
      *dwpi = TMath::Sin(arg);
      *dwr  = TMath::Cos(arg);
      *dwi  = *dwpi;
   }
   
   // main loop over frequencies
   Num_t pymax=0;
   Float_t sumsh, sumc, sums, sumsy, sumcy,
           wtau, swtau, cwtau, wtemp,
           ss, cc, yy;
   Num_t* dpx=px, * dpy=py;
   for (i=0; i<nout; ++i, ++dpx, ++dpy) {
      *dpx = pnow;
      sumsh = sumc = 0;
      dwr = wr; dwi = wi;
      for (j=0; j<n; ++j, ++dwr, ++dwi) {
         sumsh += (*dwi) * (*dwr);
         sumc  += (*dwr - *dwi) * (*dwr + *dwi);
      }
      wtau  = 0.5*TMath::ATan2(2.0*sumsh, sumc);
      swtau = TMath::Sin(wtau);
      cwtau = TMath::Cos(wtau);
      sums = sumc = sumsy = sumcy = 0;
      dwr = wr; dwi = wi; dwpr=wpr; dwpi=wpi; dy=y;
      for (j=0; j<n; ++j, ++dwr, ++dwi, ++dwpr, ++dwpi, ++dy) {
         ss     = (*dwi * cwtau) - (*dwr * swtau);
         cc     = (*dwr * cwtau) + (*dwi * swtau);
         sums  += ss*ss;
         sumc  += cc*cc;
         yy     = *dy - ave;
         sumsy += yy*ss;
         sumcy += yy*cc;
         wtemp  = *dwr;
         *dwr   = (wtemp*(*dwpr) - (*dwi)*(*dwpi)) + (*dwr);
         *dwi   = ((*dwi)*(*dwpr) + wtemp*(*dwpi)) + (*dwi);
      }
      *dpy = 0.5*(sumcy*(sumcy/sumc) + sumsy*(sumsy/sums))/var;
      if (*dpy >= pymax) {
          jmax = i;
         pymax = *dpy;
      }
      pnow += pincr;
   }
   // evaluate significance of maximum
   const Num_t expy = TMath::Exp(-pymax);
   const Num_t effm = 2.0*nout/ofac;
   prob = effm*expy;
   if (prob>0.01) {
      prob = 1.0 - TMath::Power(1.0-expy, effm);
   }
   
   // cleanup
   delete[] wi;
   delete[] wpi;
   delete[] wpr;
   delete[] wr;
}

template<typename Num_t>
void TSnSpectral::FastLomb(const Num_t* const x, const Num_t* const y,
                           const UInt_t n,
                           const Float_t ofac, const Float_t hifac,
                           Num_t* const wk1, Num_t* const wk2,
                           const UInt_t nwk,
                           UInt_t& nout, UInt_t& jmax, Num_t& prob) {
   // NUM REC C - 13.8, pg 582-584
   //
   // workspace arrays must be at least the power of 2 larger than
   // ofac * hifac * n * kMACC
   //
   // ofac = oversampling factor (usually >=4)
   // hifac = max freq / <f_c>
   // output put in wk1 and wk2, each should be already allocated arrays
   // of length nwk
   
   nout = static_cast<UInt_t>(0.5*ofac*hifac*n);
   const UInt_t nfreqt = static_cast<UInt_t>(ofac*hifac*n*kMACC);
   UInt_t nfreq = 64;
   while ( nfreq < nfreqt ) nfreq <<= 1;
   const UInt_t ndim = nfreq << 1;
   if (ndim > nwk) {
      throw std::invalid_argument(Form("<TSnSpectral::FastLomb>: "
                                       "Workspace arrays too small (%u < %u)",
                                       nwk, ndim));
      return;
   }
   Num_t ave, var;
   TSnMath::GetAveVarTwoPass(y, n, ave, var);
   if (var < kEpsilon) {
      throw std::invalid_argument("<TSnSpectral::FastLomb>: "
                                  "Variance too small");
      return;
   }
   const Num_t twovar = 2.0*var;
   
   // range
   Num_t xmin=*x, xmax=*x;
   const Num_t xdif = TSnMath::GetRange(x, n, xmin, xmax);
   
   // zero the workspaces
   memset(wk1, 0, ndim*sizeof(Num_t));
   memset(wk2, 0, ndim*sizeof(Num_t));
   
   const Float_t fndim = static_cast<Float_t>(ndim);
   const Float_t fac   = fndim / (xdif*ofac);
   
   // extirpolate the data to the workspaces
   Num_t ck, ckk;
   const Num_t* dx=x, * dy=y;
   register UInt_t j;
   for (j=0; j<n; ++j, ++dx, ++dy) {
      ck  = (*dx - xmin)*fac;
      while (ck  >= fndim) ck  -= fndim; // ck mod fndim
      ckk = 2.0*ck;
      while (ckk >= fndim) ckk -= fndim; // ckk mod fndim
      ++ck;
      ++ckk;

      Spread(*dy - ave, wk1, ndim, ck,  kMACC);
      Spread(static_cast<Num_t>(1.0),
                        wk2, ndim, ckk, kMACC);
   }

   // Take the FFT's
   RealFFT(wk1, ndim, kFALSE);
   RealFFT(wk2, ndim, kFALSE);
   
   const Float_t df = 1.0 / (xdif*ofac);
   Num_t pmax = -1.0f;
   
   Num_t* dwk1= wk1,   * dwk2= wk2,
        * kwk1= wk1+2, * kwk2= wk2+2,
        * pwk1=kwk1+1, * pwk2=kwk2+1;
   Float_t hypo, hc2wt, hs2wt, cwt, swt, cterm, sterm, den;
   for (j=0; j<nout; ++j, ++dwk1, ++dwk2, kwk1+=2, kwk2+=2, pwk1+=2, pwk2+=2) {
      hypo  = TMath::Sqrt( ((*kwk2)*(*kwk2)) + ((*pwk2)*(*pwk2)) );
      hc2wt = 0.5*(*kwk2)/hypo;
      hs2wt = 0.5*(*pwk2)/hypo;
      cwt   = TMath::Sqrt(0.5+hc2wt);
      swt   = TMath::Sqrt(0.5-hc2wt);
      swt   = (hs2wt<0) ? -TMath::Abs(swt) : TMath::Abs(swt);
      den   = 0.5*n + hc2wt*(*kwk2) + hs2wt*(*pwk2);
      cterm = cwt*(*kwk1) + swt*(*pwk1);
      cterm = cterm*cterm / den;
      sterm = cwt*(*pwk1) - swt*(*kwk1);
      sterm = sterm*sterm / (n-den);
      *dwk1 = (j+1)*df;
      *dwk2 = (cterm+sterm)/twovar;
      if (*dwk2 > pmax) {
         jmax = j;
         pmax = *dwk2;
      }
   }
   const Num_t expy = TMath::Exp(-pmax);
   const Num_t effm = 2.0*nout/ofac;
   prob = effm*expy;
   if (prob > 0.01) {
      prob = 1.0 - TMath::Power(1.0-expy, effm);
   }
}

template<typename Num_t>
void TSnSpectral::Spread(const Num_t y, Num_t* const yy, const UInt_t n,
                         const Num_t x, const UInt_t m) {
   // extirpolate (spread) a value y into m actual array elements
   // that best approximate the "fictional" array element x given the
   // array yy.
   //
   // NUM REC C - 13.8, pg 583-584
   
   static const Float_t nfac[14] = { // factorials
      0,            1,        1,         2,     6,
      24,         120,      720,      5040, 40320,
      362880, 3628800, 39916800, 479001600 // higher won't fit in 4 bytes
   };
   
   if (m>13) {
      throw std::out_of_range(Form("<TSnSpectral::Spread>: "
                                   "Factorial table too small for m=%u",m));
      return;
   }
   if (x<0) {
      throw std::out_of_range("<TSnSpectral::Spread>: "
                              "x negative");
      return;
   }
   
   UInt_t ix = static_cast<UInt_t>(x);
   if (x == static_cast<Float_t>(ix)) {
      yy[ix-1] += y;
   } else {
      // note: for some reason, if these Int_t's are instead of type UInt_t,
      // the algorithm doesn't work -- even though ilo and ihi are always
      // non-negative!
      const Int_t ilo=TMath::Min(TMath::Max(static_cast<Int_t>(x-0.5*m+1.0),1),
                                 static_cast<Int_t>(n-m+1));
      const Int_t ihi=ilo+m-1;
      Num_t fac=x-ilo, fnden=nfac[m];
      
      register Int_t j;
      for (j=ilo+1; j<=ihi; ++j) {
         fac *= (x-j);
      }
      yy[ihi-1] += y*fac/(fnden*(x-ihi));

      j=ihi-1;
      Num_t* dyy = yy+j-1;
      for (; j>=ilo; --j, --dyy) {
         fnden=(fnden/(j+1-ilo))*(j-ihi);
         *dyy += y*fac/(fnden*(x-j));
      }
   }
}      

template<typename Num_t>
void TSnSpectral::HilbertCpxAnalyticSignal(Num_t* const x, const UInt_t n) {
   // replace the data in the 'data' array by its discrete, complex-valued
   // analytic signal
   //
   // the real parts are simply the original data
   // the imaginary parts are the Hilbert transform of the data
   //
   // the analytic signal = InvFFT[ FFT(x) * 2U ]
   // where U is the Heaviside step fcn with U(0)=0.5
   //
   // the absolute value of the analytic signal will give the (upper)
   // envelope of the data
   // 
   // the array x must be [0..2*n-1]
   // n must be a power of two
   // the data is organized like: [ real_0, img_0, real_1, img_1, ... ]
   //
   
   // take FFT
   DiscreteCpxFFT(x, n, kFALSE);
   
   // multiply by step fcn
   // freq 0 (elems 0,1) & f_c (elems n/2,n/2+1) unchanged
   // pos freqs doubled
   Num_t* u = x+2;
   for (UInt_t i=2; i<n; ++i, ++u) {
      *u *= 2;
   }
   // neg freq zeroed
   u = x+n+2;
   for (UInt_t i=2; i<n; ++i, ++u) {
      *u = 0;
   }
   
   // inverse FFT
   DiscreteCpxFFT(x, n, kTRUE);
   
}

template<typename Num_t>
void TSnSpectral::EnvelopeReal(Num_t* const data, const UInt_t n) {
   // replace the contents of the real-valued data array
   // with its positive envelope
   //
   // data goes from [0..n-1]
   //
   // see HilbertCpxAnalyticSignal
   //
   // note that this only makes sense for data with a range
   // containing both positive and negative values (i.e. pedestal
   // subtracted data)

   // make temporary array for Hilbert transform
   const UInt_t n2 = n<<1;
   Num_t* const temp = new Num_t[n2];
   Num_t* t = temp;
   Num_t* d = data;
   for (UInt_t i=0; i<n; ++i, ++d, t+=2) {
      *t     = *d;
      *(t+1) =  0;
   }

   // get the analytic signal
   TSnSpectral::HilbertCpxAnalyticSignal(temp, n);
   // envelope is the abs value
   t = temp;
   d = data;
   for (UInt_t i=0; i<n; ++i, t+=2, ++d) {
      *d = TMath::Sqrt(  ((*t)*(*t)) + ( (*(t+1))*(*(t+1)) )  );
   }
   delete[] temp;
}                 


#endif // SNS_TSnSpectral
