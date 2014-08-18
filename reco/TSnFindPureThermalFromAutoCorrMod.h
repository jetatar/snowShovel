#ifndef SNS_TSnFindPureThermalFromAutoCorrMod
#define SNS_TSnFindPureThermalFromAutoCorrMod

#include <TString.h>

#include "TAModule.h"

class TSnFindPureThermalFromAutoCorrMod : public TAModule {
 public:
   enum EAutoCorrTagAlgo_t {
      kSmallestAnyChan // cut on the smallest auto correlation value on any chan in the event
   };
   enum EBehavior_t {
      kCutPureThermal,    // skip events identified as pure thermal
      kCutNonPureThermal, // skip events NOT identified as pure thermal
      kAddTagToEvent      // don't skip any events, but add a TParameter<Bool_t> to the event that is true if and only if the event is pure thermal
   };

 private:
   TString          fAutoCorrNm; // name of the TObjArray holding the auto correlation interpolation objects for each channel (output of TSnChanTimeCorlAltEvtsMod with main and alt event branch names being the same) (default: "SingleChanCorls.")
   TString          fPureThmTagNm; // name of TParameter<Bool_t> object added to event in kAddTagToEvent; the bool is true for pure thermal events (default: "IsPureThermalFromAutoCorr")
   TSnFindPureThermalFromAutoCorrMod::EAutoCorrTagAlgo_t
                    fAutoCorrTagAlgo; // algorithm to use to identify pure thermal events (default: kSmallestAnyChan)
   TSnFindPureThermalFromAutoCorrMod::EBehavior_t
                    fBehavior; // whether to skip pure thermal events, skip non pure thermal events, keep all but tag events, etc. See EBehavior_t. (default: kAddTagToEvent)
   Double_t         fAutoCorrCut; // auto correlation cut value (default: -0.45)
   
   
   
   Bool_t DoSmallestAnyChanCheck(const TObjArray* const acs, Bool_t& isPureThm);
   
   
 protected:
   void     Process();

 public:
   TSnFindPureThermalFromAutoCorrMod() :
      fAutoCorrTagAlgo(kSmallestAnyChan),
      fBehavior(kAddTagToEvent),
      fAutoCorrCut(-0.45) {
      // default ctor - do not use
   }
   TSnFindPureThermalFromAutoCorrMod(const Char_t* name,
                                     const Char_t* autoCorrNm="SingleChanCorls."
                                     );
   virtual ~TSnFindPureThermalFromAutoCorrMod() {}
   
   TString  GetAutoCorrName() const { return fAutoCorrNm; }
   TString  GetPureThermalTagName() const { return fPureThmTagNm; }
   TString  GetAutoCorrTagAlgo() const
      { return GetAutoCorrTagAlgoAsString(fAutoCorrTagAlgo); }
   TString  GetTagBehavior() const
      { return GetTagBehaviorAsString(fBehavior); }
   Double_t GetAutoCorrCut() const { return fAutoCorrCut; }

   void     SetAutoCorrName(const Char_t* n) { fAutoCorrNm = n; }
   void     SetPureThermalTagName(const Char_t* n) { fPureThmTagNm = n; }
   void     SetAutoCorrTagAlgo(const Char_t* n)
      { fAutoCorrTagAlgo = GetAutoCorrTagAlgoFromString(n); }
   void     SetTagBehavior(const Char_t* n)
      { fBehavior = GetTagBehaviorFromString(n); }
   void     SetAutoCorrCut(const Double_t v) { fAutoCorrCut = v; }
   

   TSnFindPureThermalFromAutoCorrMod::EAutoCorrTagAlgo_t
            GetAutoCorrTagAlgoVal() const { return fAutoCorrTagAlgo; }
   TSnFindPureThermalFromAutoCorrMod::EBehavior_t
            GetTagBehaviorVal() const { return fBehavior; }
   void     SetAutoCorrTagAlgoVal(
      const TSnFindPureThermalFromAutoCorrMod::EAutoCorrTagAlgo_t v)
      { fAutoCorrTagAlgo = v; }
   void     SetTagBehaviorVal(
      const TSnFindPureThermalFromAutoCorrMod::EBehavior_t v)
      { fBehavior = v; }
   
   
   static
   const Char_t* GetAutoCorrTagAlgoAsString(
      const TSnFindPureThermalFromAutoCorrMod::EAutoCorrTagAlgo_t x) {
      // convert from the enum value x to a descriptive string
      // returns "UNKNOWN" if x is invalid
      switch (x) {
         case kSmallestAnyChan:
            return "SmallestAnyChan";
         default:
            return "UNKNOWN";
      };
   }
   
   static
   TSnFindPureThermalFromAutoCorrMod::EAutoCorrTagAlgo_t
                 GetAutoCorrTagAlgoFromString(const TString n);

   static
   const Char_t* GetTagBehaviorAsString(
      const TSnFindPureThermalFromAutoCorrMod::EBehavior_t x) {
      // convert from the enum value x to a descriptive string
      // returns "UNKNOWN" if x is invalid
      switch (x) {
         case kCutPureThermal:
            return "CutPureThermal";
         case kCutNonPureThermal:
            return "CutNonPureThermal";
         case kAddTagToEvent:
            return "AddTagToEvent";
         default:
            return "UNKNOWN";
      };
   }
   
   static
   TSnFindPureThermalFromAutoCorrMod::EBehavior_t 
                 GetTagBehaviorFromString(const TString n);
   
   ClassDef(TSnFindPureThermalFromAutoCorrMod,1); // cut away (or keep) pure thermal events by cutting on autocorrelation values
};

#endif // SNS_TSnFindPureThermalFromAutoCorrMod
