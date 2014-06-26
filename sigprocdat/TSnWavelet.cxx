#include "TSnWavelet.h"

#include <TString.h>

ClassImp(TSnWavelet);

void TSnWavelet::Transform(const Float_t* const data,
                           const UInt_t n,
                           Float_t* const result) {
   if ((n>3) && !(n & (n-1))) { // check if power of 2
      memcpy(result, data, sizeof(Float_t)*n);
      for (UInt_t i=n; i!=2u; i>>=1) { // divide by 2 each time
         TransformStep(result, i);
      }
   }
}

void TSnWavelet::Inverse(const Float_t* const data,
                         const UInt_t n,
                         Float_t* const result) {
   if ((n>3) && !(n & (n-1))) { // check if power of 2
      memcpy(result, data, sizeof(Float_t)*n);
      for (UInt_t i=4u; i<=n; i<<=1) { // multiply by 2 each time
         InverseStep(result, i);
      }
   }
}

void TSnWavelet::TransformStep(Float_t* const data,
                               const UInt_t n) {
   // n must be a power of 2, but this is not checked
   // result must already be allocated to size n
   if (n>3) {
      TArrayF wksp(n);
      const UInt_t ncof = GetNcof();
      const UInt_t nmod = ncof * n;
      const UInt_t n1 = n-1;
      const UInt_t nh = n >> 1; // n/2
      UInt_t ni(0), nj(0), jf(0), jr(0);
      for (UInt_t ii=0, i=0; i<n; i+=2, ++ii) {
         ni = i + nmod + GetIoff();
         nj = i + nmod + GetJoff();
         for (UInt_t k=0; k<ncof; ++k) {
            jf = n1 & (ni+k); // wrap
            jr = n1 & (nj+k);
            wksp[ii]    += GetCC(k) * data[jf];
            wksp[ii+nh] += GetCR(k) * data[jr];
         }
      }
      memcpy(data, wksp.GetArray(), n*sizeof(Float_t));
   }
}


void TSnWavelet::InverseStep(Float_t* const data,
                             const UInt_t n) {
   // n must be a power of 2, but this is not checked
   // result must already be allocated to size n
   if (n>3) {
      TArrayF wksp(n);
      const UInt_t ncof = GetNcof();
      const UInt_t nmod = ncof * n;
      const UInt_t n1 = n-1;
      const UInt_t nh = n >> 1; // n/2
      UInt_t ni(0), nj(0), jf(0), jr(0);
      Float_t ai(0), ai1(0);
      for (UInt_t ii=0, i=0; i<n; i+=2, ++ii) {
         ai  = data[ii];
         ai1 = data[ii+nh];
         ni  = i + nmod + GetIoff();
         nj  = i + nmod + GetJoff();
         for (UInt_t k=0; k<ncof; ++k) {
            jf = n1 & (ni+k);
            jr = n1 & (nj+k);
            //Printf("n=%u, ii=%u, k=%u, n1=%u, ni=%u, jf=%u, nj=%u, jr=%u",
            //       n, ii, k, n1, ni, jf, nj, jr);
            wksp[jf] += GetCC(k) * ai;
            wksp[jr] += GetCR(k) * ai1;
         }
      }
      memcpy(data, wksp.GetArray(), n*sizeof(Float_t));
   }
}

void TSnWavelet::BuildCRandOffs() {
   Float_t sig = -1.0;
   const UInt_t n = GetNcof();
   for (UInt_t k=0; k<n; ++k) {
      SetCR( n-k-1, sig * GetCC(k) );
      sig = -sig;
   }
   const Int_t nx = -(n >> 1);
   SetIoff(nx);
   SetJoff(nx);
}

