#include "NSnChanCorl.h"

#include <TString.h>
#include <TError.h>
#include <TMath.h>
#include <TObjArray.h>
#include <TVector3.h>

#include "NSnConstants.h"
#include "TSnInterp1DWvData.h"
#include "TSnRecoChanOffsets.h"
#include "TSnMath.h"

#include <algorithm>
#include <stdexcept>

const Double_t NSnChanCorl::kC_m_ns  = TMath::C() / 1.0e9;
const Double_t NSnChanCorl::kMaxDt   = NSnConstants::kNsamps /
   (2.0 * NSnConstants::kSampRate);
const Double_t NSnChanCorl::kEpsilon = 1e-300;


Double_t NSnChanCorl::GetProbForDTFromCorl(Double_t dt,
                                           const TSnInterp1DWvData& corl) {
   // get the probability that an offset of 'dt' would be observed
   // given the specified correlation function between the channels
   //
   // both dt and corl should have units of time (not samples)
   //
   // applies a severe penalty if dt is bigger than half the readout window
   
   Double_t od = 1;
   Bool_t oob = kFALSE; // out of bounds?
   if (dt<-kMaxDt) {
      od  = dt + kMaxDt;
      dt  = -kMaxDt;
      oob = kTRUE;
   } else if (dt>kMaxDt) {
      od  = dt - kMaxDt;
      dt  = kMaxDt;
      oob = kTRUE;
   }
   Double_t c = GetProbFromNormCorl(corl.Eval(dt));
   if (oob) {
      c /= od*od;
   }
   return c;
}


Double_t NSnChanCorl::GetLLfromParallelChDTsForPlaneWv(const TArrayD& dts,
                                                       const TObjArray& corl,
                                                       const Float_t csig) {
   // this function ONLY uses two delta-t numbers: the parallel chans
   // dt(2,0) and dt(3,1). therefore this only works for 4 channel detectors
   //
   // calculate the negative log likelihood that the time offests
   // specified in dts came from a plane wave given the correlation
   // probablity functions specified in corl.
   //
   // expect arrays to be indexed like in TSnRecoChanOffsets
   // corl should contain TSnInterp1DWvData objects
   // csig is the gaussian sigma used to constrain the plane wave
   //  (same units as dts)
   //
   // the 'dts' array should be the time differences due to a plane
   // wave (i.e. the result of GetPlaneWaveOffsets)
   //
   // expect dts and corl to have units of time (NOT samples!)
   
   Double_t c, dt, tc(1.0);
   if (NSnConstants::kNchans==4) {
   
      const TSnInterp1DWvData* cr;
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         for (UChar_t xc=0; xc<ch; ++xc) {
            if ( (ch==2 && xc==0) || (ch==3 && xc==1) ) {
               const UInt_t di = TSnRecoChanOffsets::IndexFor(ch,xc);
               dt = dts[di];
               cr  = dynamic_cast<const TSnInterp1DWvData*>(corl.At(di));
               if (cr!=0) {
                  c = GetProbForDTFromCorl(dt, *cr);
                  if (isnan(c)) {
                     ::Error("GetLLfromParallelChDTsForPlaneWv",
                             "NAN at ch=%hhu, xc=%hhu, dt=%g, corl=%g",
                             ch, xc, dt, cr->Eval(dt));
                  }
                  tc *= c;
               } else {
                  throw std::runtime_error(
                     Form("<NSnChanCorl::GetLLfromParallelChDTsForPlaneWv>: "
                          "Could not get TSnInterp1DWvData object for "
                          "ch=%hhu, xc=%hhu.",
                          ch, xc));
               }
            }
         }
      }
   } else {
      throw std::runtime_error(Form(
         "<NSnChanCorl::GetLLfromParallelChDTsForPlaneWv>: "
         "Only works for a 4 channel detector, not %hhu!",
         NSnConstants::kNchans));
   }

   if (tc>kEpsilon) {
      return ( -TMath::Log(tc) );
   } else {
      // prevent NaN's -- increase the log lik linearly for probabilities
      // smaller than epsilon
      return ( -TMath::Log(kEpsilon) / TMath::Abs(kEpsilon-tc) );
   }
}

Double_t NSnChanCorl::GetLLfromDTsForPlaneWv(const TArrayD& dts,
                                             const TObjArray& corl,
                                             const Float_t csig) {
   // calculate the negative log likelihood that the time offests
   // specified in dts came from a plane wave given the correlation
   // probablity functions specified in corl.
   //
   // expect arrays to be indexed like in TSnRecoChanOffsets
   // corl should contain TSnInterp1DWvData objects
   // csig is the gaussian sigma used to constrain the plane wave
   //  (same units as dts)
   //
   // the 'dts' array should be the time differences due to a plane
   // wave (i.e. the result of GetPlaneWaveOffsets)
   //
   // expect dts and corl to have units of time (NOT samples!)
   
   Double_t c, dt, tc(1.0);
   const TSnInterp1DWvData* cr;
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      for (UChar_t xc=0; xc<ch; ++xc) {
         const UInt_t di = TSnRecoChanOffsets::IndexFor(ch,xc);
         dt = dts[di];
         cr  = dynamic_cast<const TSnInterp1DWvData*>(corl.At(di));
         if (cr!=0) {
            c = GetProbForDTFromCorl(dt, *cr);
            if (isnan(c)) {
               ::Error("GetLLfromDTsForPlaneWv",
                       "NAN at ch=%hhu, xc=%hhu, dt=%g, corl=%g",
                       ch, xc, dt, cr->Eval(dt));
            }
            tc *= c;
         } else {
            throw std::runtime_error(
               Form("<NSnChanCorl::GetLLfromDTsForPlaneWv>: "
                    "Could not get TSnInterp1DWvData object for "
                    "ch=%hhu, xc=%hhu.",
                    ch, xc));
         }
      }
   }
   // apply the constraints
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      for (UChar_t xc=0; xc<ch; ++xc) {
         if ( (ch-xc)>1 ) {
            dt = 0;
            for (UChar_t xi=xc+1; xi<=ch; ++xi) {
               dt += dts[ TSnRecoChanOffsets::IndexFor(xi, xi-1) ];
            }
            tc *= TMath::Gaus( dts[ TSnRecoChanOffsets::IndexFor(ch,xc) ],
                               dt,
                               csig );
         }
      }
   }
   
   if (tc>kEpsilon) {
      return ( -TMath::Log(tc) );
   } else {
      // prevent NaN's -- increase the log lik linearly for probabilities
      // smaller than epsilon
      return ( -TMath::Log(kEpsilon) / TMath::Abs(kEpsilon-tc) );
   }
}

TArrayD NSnChanCorl::GetPlaneWaveOffsets(const Double_t theta,
                                         const Double_t phi,
                                         const TArrayD& offsets,
                                         const TVector3 posCh[],
                                         const Double_t ngTopFern) {
   // theta and phi are the arrival angles (in rad) of the plane wave
   //
   // offsets should be in ns. posCh should be in meters.
   //
   // find the offsets yielded by a plane wave from the specified
   // direction and add on the a-priori known offsets
   //
   // because the number of channel combinations is know to be small,
   // the function returns a TArrayD by value. if this changes, it should
   // probably accept a reference to the output array instead.
   
   TVector3 norm;
   norm.SetMagThetaPhi(1.0, theta, phi);
   TArrayD dts(offsets.GetSize());
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      const Double_t disCh = -(posCh[ch].Dot(norm));
      for (UChar_t xc=0; xc<ch; ++xc) {
         const Double_t disXc = -(posCh[xc].Dot(norm));
         const UInt_t       i = TSnRecoChanOffsets::IndexFor(ch,xc);
         dts[i] = offsets[i] + ( (disCh-disXc) * ngTopFern / kC_m_ns );
      }
   }
   return dts;
}

Double_t NSnChanCorl::GetLLfromEnvelopeCoGs(const Double_t* const env1,
                                            const Double_t* const env2,
                                            const Float_t         minVal) {
   // get the negative log likelihood that the two (positive) envelopes
   // overlap using the distance of the center of gravity (weighted
   // average) of the envelopes. only the region of the envelopes above
   // 'minVal' is used. this allows a threshold above noise to be set.
   //
   // assumes that the envelope is already time ordered in an absolute
   // sense -- that is, earlier entires in the array are earlier in time.
   // (no wrapping)
   //
   // assumes envelope is always positive, but does not check this
   
   // acceptable region is [first, last] sample to cross minfrac
   const Double_t* e1 = env1, * e2 = env2;
   UChar_t start1(0), end1(0), start2(0), end2(0);
   for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm, ++e1) {
      if (*e1 > minVal) {
         start1=sm;
         break;
      }
   }
   for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm, ++e2) {
      if (*e2 > minVal) {
         start2=sm;
         break;
      }
   }
   e1 = env1 + NSnConstants::kNsamps -1;
   for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm, --e1) {
      if (*e1 > minVal) {
         end1=sm;
         break;
      }
   }
   e2 = env2 + NSnConstants::kNsamps -2;
   for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm, --e2) {
      if (*e2 > minVal) {
         end2=sm;
         break;
      }
   }
   
   // find the weighted average of the selected envelope region
   Double_t smps[NSnConstants::kNsamps];
   Double_t* s = smps;
   for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm, ++s) {
      *s = sm;
   }
   Double_t ave1(0), rms1(0);
   TSnMath::GetWeightedAveRMSFast(smps, env1+start1, (end1-start1), ave1, rms1);
   Double_t ave2(0), rms2(0);
   TSnMath::GetWeightedAveRMSFast(smps, env2+start2, (end2-start2), ave2, rms2);
   
   // find the difference between the weighted averages
   const Double_t diff = (ave1 - ave2) / 
      static_cast<Double_t>(NSnConstants::kNsamps);
   const Double_t p = 1.0 - (diff*diff);
   
   if (p>kEpsilon) {
      return ( -TMath::Log(p) );
   } else {
      // prevent NaN's -- increase the log lik linearly for probabilities
      // smaller than epsilon
      return ( -TMath::Log(kEpsilon) / TMath::Abs(kEpsilon-p) );
   }
   
}

Double_t NSnChanCorl::GetLLfromDistandOffset(const Float_t* const env1,
                                             const Float_t* const env2,
                                             const Float_t minVal,
                                             const Float_t maxfrac,
                                             const Float_t dt) {
   // find the distance between the rise (attack) of the two
   // waveforms.  the rise is defined as the time dt = t2-t1 over
   // which the (envelope of the) waveform first goes above minVal
   // until maxfrac. minVal should be chosen carefully so as to
   // be above the noise.
   //
   // the distance between the waveforms is then
   // d = sqrt[ (t1-s1-dt)^2 + (t2-s2-dt)^2 ]
   // where t and s are the rise of the two waveforms or envelopes.
   // 
   // the returned value has units of time
   //
   // NOTE: this assumes that entries earlier in the arrays (i.e. lower
   // sample number) are ealier in absolute time -- this function does
   // not wrap!
   //
   // since only positive rises are considered, it makes sense to pass
   // this function the (positive) envelope rather than the real waveform
   //
   // arrays are assumed to be NSnConstants::kNsamps long
   /*                  _
      maxfrac---------/ \
                     /   |     _
      minVal--------/    |    / \     _
             .../\./     \    |  \   / \...  ../\..
                          \  /    \_/      \/
                    | |    \/      
                    | |               
                   t1 t2
   */
  

   // find the abs max of each envelope
   // this assumes the envelope is always positive (which it is if it
   // comes from TSnSpectral::Envelope)
   const Float_t max1 = *(std::max_element(env1, env1+NSnConstants::kNsamps));
   const Float_t max2 = *(std::max_element(env2, env2+NSnConstants::kNsamps));

   const Float_t cmax1 = maxfrac * max1;
   const Float_t cmax2 = maxfrac * max2;

   const Float_t* e1 = env1;
   UChar_t start1(0), end1(0);
   Bool_t  chkStart1(kTRUE);
   for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm, ++e1) {
      if (chkStart1 && (*e1 > minVal)) {
         start1 = sm;
         chkStart1 = kFALSE;
      }
      if ((chkStart1==kFALSE) && (*e1>cmax1)) {
         end1 = sm;
         break;
      }
   }

   const Float_t* e2 = env2;
   UChar_t start2(0), end2(0);
   Bool_t  chkStart2(kTRUE);
   for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm, ++e2) {
      if (chkStart2 && (*e2 > minVal)) {
         start2 = sm;
         chkStart2 = kFALSE;
      }
      if ((chkStart2==kFALSE) && (*e2>cmax2)) {
         end2 = sm;
         break;
      }
   }
   
   // find distance between the rises
   const Int_t d1 = ((start1 - start2) / NSnConstants::kSampRate) - dt;
   const Int_t d2 = ((end1 - end2) / NSnConstants::kSampRate) - dt;

   static const Double_t full = 
      (NSnConstants::kNsamps / NSnConstants::kSampRate) * TMath::Sqrt(2);
   
   const Double_t d = TMath::Sqrt( (d1*d1) + (d2*d2) ) / full;
   
   const Double_t p = 1.0 - d;
      
   if (p>kEpsilon) {
      return ( -TMath::Log(p) );
   } else {
      // prevent NaN's -- increase the log lik linearly for probabilities
      // smaller than epsilon
      return ( -TMath::Log(kEpsilon) / TMath::Abs(kEpsilon-p) );
   }

}
