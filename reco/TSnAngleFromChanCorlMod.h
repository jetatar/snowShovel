#ifndef SNS_TSnAngleFromChanCorlMod
#define SNS_TSnAngleFromChanCorlMod

#include <TString.h>
#include <TVector3.h>
class TObjArray;

#include "NSnConstants.h"
#include "TSnFittingModule.h"
class TSnRecoChanOffsets;
class TSnGeoStnSite;

class TSnAngleFromChanCorlMod : public TSnFittingModule {
 public:
   enum EPars_t {
      kTheta,
      kPhi,
      kNpars
   };

 private:
   TString       fChDtsNm;  // name of published channel offsets object (default: ChanDtOffets)
   TString       fGeoNm;    // name of published geoemtry object (default: AriannaGeo)
   TString       fCorlNm;   // name of the published channel correlation objects (default: CrossChanCorls.)
   TString       fRecAngNm; // name of output reco direction object (default: AngleFromChanCorl.)
   Double_t      fNgTopFern;// group index of refraction at the top of the fern where the LPDAs are (default: 1.33)
   Float_t       fCsig;     // sigma of gaussian that determines how strict the time constraints are applied (default: 1/(2*sampRate))
   
   // branch/obj data
   const TSnRecoChanOffsets* fDts;   //! global channel offsets
   const TSnGeoStnSite*      fGeo;   //! top geometry volume
   const TObjArray*          fCorls; //! channel correlations for this event
   TVector3 fPosCh[NSnConstants::kNchans]; //! convenient storage of lpda positions. assumed not to change event to event.
   
 protected:
   virtual void SlaveBegin();
   virtual void Process();
   
 public:
   TSnAngleFromChanCorlMod() : fNgTopFern(1.33),
                               fCsig(1.0 / (2.0 * NSnConstants::kSampRate)),
                               fDts(0), fGeo(0), fCorls(0) {}
   TSnAngleFromChanCorlMod(const Char_t* name,
                           const Char_t* miniName="Minuit2",
                           const Char_t* algoName="Migrad");
   virtual ~TSnAngleFromChanCorlMod() {}
   
   TString  GetRecoAngleName() const { return fRecAngNm; }
   TString  GetChanOffsetsName() const { return fChDtsNm; }
   TString  GetGeometryName() const { return fGeoNm; }
   TString  GetChanCorlsName() const { return fCorlNm; }
   Double_t GetNgTopFern() const { return fNgTopFern; }
   Float_t  GetCorlConstraintSigma() const { return fCsig; }
   
   void     SetRecoAngleName(const Char_t* n) { fRecAngNm = n; }
   void     SetChanOffsetsName(const Char_t* n) { fChDtsNm = n; }
   void     SetGeometryName(const Char_t* n) { fGeoNm = n; }
   void     SetChanCorlsName(const Char_t* n) { fCorlNm = n; }
   void     SetNgTopFern(const Double_t n) { fNgTopFern = n; }
   void     SetCorlConstraintSigma(const Float_t c) { fCsig = c; }
   
   Double_t GetAngleLL(const Double_t* pars);
   
   static
   const Char_t* GetParName(const TSnAngleFromChanCorlMod::EPars_t x) {
      switch (x) {
         case kTheta:
            return "Theta";
         case kPhi:
            return "Phi";
         default:
            return "UNKNOWN";
      };
   }
   
   ClassDef(TSnAngleFromChanCorlMod, 2); // reconstruct theta/phi of plane wave from channel correlations
};

#endif // SNS_TSnAngleFromChanCorlMod
