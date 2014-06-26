#ifndef SNS_TSnAngleFromChanCorlMod
#define SNS_TSnAngleFromChanCorlMod

#include <TString.h>
#include <TVector3.h>
class TObjArray;
class TArrayD;

#include "NSnConstants.h"
#include "TSnFittingModule.h"
class TSnRecoChanOffsets;
class TSnGeoStnSite;
class TSnEventHeader;
class TSnEventMetadata;

class TSnAngleFromChanCorlMod : public TSnFittingModule {
 public:
   enum EPars_t {
      kTheta,
      kPhi,
      kNpars
   };

   static const UInt_t kDrawParSpace; // verb level at which to draw fit space

 private:
   TString       fChDtsNm;  // name of published channel offsets object (default: ChanDtOffets)
   TString       fGeoNm;    // name of published geoemtry object (default: AriannaGeo)
   TString       fCorlNm;   // name of the published channel correlation objects (default: CrossChanCorls.)
   TString       fRecAngNm; // name of output reco direction object (default: AngleFromChanCorl.)
   Double_t      fNgTopFern;// group index of refraction at the top of the fern where the LPDAs are (default: 1.33)
   Float_t       fCsig;     // sigma of gaussian that determines how strict the time constraints are applied (default: 1/(2*sampRate))
   Double_t      fThetaStr; // starting theta value (radians) (default: 3.0)
   Double_t      fPhiStr;   // starting phi value (radians) (default: 5.5)
   UInt_t        fNrndStr;  // number of random starting values to try (default: 0). starting value seed = fThetaStr + 10*fPhiStr
   UInt_t        fThetaScn; // number of coarse scan points along theta (default: 0)
   UInt_t        fPhiScn;   // number of coarse scan points along phi (default: 0)
   
   // branch/obj data
   const TSnRecoChanOffsets* fDts;   //! global channel offsets
   const TSnGeoStnSite*      fGeo;   //! top geometry volume
   const TObjArray*          fCorls; //! channel correlations for this event
   TVector3 fPosCh[NSnConstants::kNchans]; //! convenient storage of lpda positions. assumed not to change event to event.
   TSnEventHeader*           fHdr;   //!
   TSnEventMetadata*         fMeta;  //!



   void DoFit(const Double_t thetaStart,
              const Double_t thetaMin,
              const Double_t thetaMax,
              const Double_t phiStart,
              const Double_t phiMin,
              const Double_t phiMax,
              TArrayD& pars,
              TArrayD& errs,
              Bool_t& isValidErr,
              Double_t& minValue);

   
 protected:
   virtual void SlaveBegin();
   virtual void Process();
   
 public:
   TSnAngleFromChanCorlMod() : fNgTopFern(1.33),
                               fCsig(1.0 / (2.0 * NSnConstants::kSampRate)),
                               fThetaStr(3.0), fPhiStr(5.5), fNrndStr(0),
                               fThetaScn(0), fPhiScn(0),
                               fDts(0), fGeo(0), fCorls(0),
                               fHdr(0), fMeta(0) {}
   TSnAngleFromChanCorlMod(const Char_t* name,
                           const Char_t* miniName="Minuit2",
                           const Char_t* algoName="Migrad");
   virtual ~TSnAngleFromChanCorlMod() {}
   
   TString  GetRecoAngleName() const { return fRecAngNm; }
   TString  GetRecoDTsName() const {
      TString x("DTsOf");
      x += GetRecoAngleName();
      return x;
   }
   TString  GetChanOffsetsName() const { return fChDtsNm; }
   TString  GetGeometryName() const { return fGeoNm; }
   TString  GetChanCorlsName() const { return fCorlNm; }
   Double_t GetNgTopFern() const { return fNgTopFern; }
   Float_t  GetCorlConstraintSigma() const { return fCsig; }
   Double_t GetStartingTheta() const { return fThetaStr; }
   Double_t GetStartingPhi() const { return fPhiStr; }
   UInt_t   GetNumRandomStartValueTries() const { return fNrndStr; }
   UInt_t   GetNumThetaScanPoints() const { return fThetaScn; }
   UInt_t   GetNumPhiScanPoints() const { return fPhiScn; }

   void     SetRecoAngleName(const Char_t* n) { fRecAngNm = n; }
   void     SetChanOffsetsName(const Char_t* n) { fChDtsNm = n; }
   void     SetGeometryName(const Char_t* n) { fGeoNm = n; }
   void     SetChanCorlsName(const Char_t* n) { fCorlNm = n; }
   void     SetNgTopFern(const Double_t n) { fNgTopFern = n; }
   void     SetCorlConstraintSigma(const Float_t c) { fCsig = c; }
   void     SetStartingTheta(const Double_t t) { fThetaStr = t; }
   void     SetStartingPhi(const Double_t p) { fPhiStr = p; }
   void     SetNumRandomStartValueTries(const UInt_t n) { fNrndStr = n; }
   void     SetNumThetaScanPoints(const UInt_t n) { fThetaScn = n; }
   void     SetNumPhiScanPoints(const UInt_t n) { fPhiScn = n; }
   
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
   
   ClassDef(TSnAngleFromChanCorlMod, 3); // reconstruct theta/phi of plane wave from channel correlations
};

#endif // SNS_TSnAngleFromChanCorlMod
