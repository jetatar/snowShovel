#ifndef SNS_TSnAngleFromSingleChanOffsets
#define SNS_TSnAngleFromSingleChanOffsets

#include <TString.h>
#include <TVector3.h>
class TObjArray;

#include "NSnConstants.h"
#include "TSnFittingModule.h"
class TSnGeoStnSite;
class TSnRecoChanOffsets;
class TSnRecoSingleChanOffsetSet;
class TSnEventMetadata;
class TSnEventHeader;

class TSnAngleFromSingleChanOffsets : public TSnFittingModule {
 public:
   enum EPars_t {
      kTheta,
      kPhi,
      kNpars
   };

   static const UInt_t kDrawParSpace; // verbosity level at which to display the fit space

 private:
   TString       fChDtsNm;  // name of channel delays object in an event (default: SingleChanTimeOffsets.)
   TString       fChOffsNm; // name of published channel offsets object (default: SingleChanTimeOffsets)
   TString       fGeoNm;    // name of published geoemtry object (default: AriannaGeo)
   TString       fRecAngNm; // name of output reco direction object (default: AngleFromSingleChanOffsets.)
   Double_t      fNgTopFern;// group index of refraction at the top of the fern where the LPDAs are (default: 1.33)

   // event data
   TSnRecoSingleChanOffsetSet* fDts;       //! the single channel time offsets
   const TSnGeoStnSite* fGeo;              //! top geometry volume
   const TSnRecoChanOffsets* fOffsets;     //! a priori known channel offsets
   TVector3 fPosCh[NSnConstants::kNchans]; //! convenient storage of lpda positions. assumed not to change event to event.

   TSnEventMetadata* fMeta; //! event meta data for debugging
   TSnEventHeader*   fHdr;  //! event header for debugging
   
 protected:
   virtual void SlaveBegin();
   virtual void Process();
   
 public:
   TSnAngleFromSingleChanOffsets() : fNgTopFern(1.33), fDts(0), fGeo(0) {}
   TSnAngleFromSingleChanOffsets(const Char_t* name,
                                 const Char_t* miniName="Minuit2",
                                 const Char_t* algoName="Migrad");
   virtual ~TSnAngleFromSingleChanOffsets() {}

   TString GetRecoAngleName() const { return fRecAngNm; }
   TString GetSingleChanDelaysName() const { return fChDtsNm; }
   TString GetChanOffsetsName() const { return fChOffsNm; }
   TString GetGeometryName() const { return fGeoNm; }
   Double_t GetNgTopFern() const { return fNgTopFern; }
   
   
   void    SetRecoAngleName(const Char_t* n) { fRecAngNm = n; }
   void    SetSingleChanDelaysName(const Char_t* n) { fChDtsNm = n; }
   void    SetChanOffsetsName(const Char_t* n) { fChOffsNm = n; }
   void    SetGeometryName(const Char_t* n) { fGeoNm = n; }
   void    SetNgTopFern(const Double_t n) { fNgTopFern = n; }

   Double_t GetAngleChi2(const Double_t* pars);
   
   static
   const Char_t* GetParName(const TSnAngleFromSingleChanOffsets::EPars_t x) {
      switch (x) {
         case kTheta:
            return "Theta";
         case kPhi:
            return "Phi";
         default:
            return "UNKNOWN";
      };
   }
   
   
   ClassDef(TSnAngleFromSingleChanOffsets, 2); // reconstruct theta/phi of plane wave from single channel offsets
};

#endif // SNS_TSnAngleFromSingleChanOffsets
