#ifndef SNS_NSnChanCorl
#define SNS_NSnChanCorl

#include <Rtypes.h>
#include <TArrayD.h>
class TObjArray;
class TVector3;

class TSnInterp1DWvData;

namespace NSnChanCorl {
   
   extern const Double_t kC_m_ns;  // C in m/ns units
   extern const Double_t kMaxDt;   // max time offset = half the sample window in time (~32ns)
   extern const Double_t kEpsilon; // a small number (e.g. below which the -log will not be calculated)
   
   inline Double_t GetC_m_ns() { return kC_m_ns; }
   inline Double_t GetMaxDt() { return kMaxDt; }
   inline Double_t GetEpsilon() { return kEpsilon; }
   
   
   inline
   Double_t GetProbFromNormCorl(const Double_t coef) {
      const Double_t c = (coef + 1.0) / 2.0;
      return c*c;
   }
   
   extern
   Double_t GetProbForDTFromCorl(Double_t dt,
                                 const TSnInterp1DWvData& corl);
   
   extern
   Double_t GetLLfromDTsForPlaneWv(const TArrayD& dts,
                                   const TObjArray& corl,
                                   const Float_t csig);
   
   extern
   TArrayD  GetPlaneWaveOffsets(const Double_t theta,
                                const Double_t phi,
                                const TArrayD& offsets,
                                const TVector3 posCh[],
                                const Double_t ngTopFern);
   
   extern
   Double_t GetLLfromEnvelopeCoGs(const Double_t* const env1,
                                  const Double_t* const env2,
                                  const Float_t         minVal);
   
   extern
   Double_t GetLLfromDistandOffset(const Float_t* const env1,
                                   const Float_t* const env2,
                                   const Float_t minVal,
                                   const Float_t maxfrac,
                                   const Float_t dt);
   
};

#endif // SNS_NSnChanCorl
