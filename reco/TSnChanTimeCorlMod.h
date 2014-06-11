#ifndef SNS_TSnChanTimeCorlMod
#define SNS_TSnChanTimeCorlMod

#include <TString.h>
class TObjArray;

#include "TSnCalDatModule.h"
class TSnCalWvData;

class TSnChanTimeCorlMod : public TSnCalDatModule {
 private:
   TString        fWvDatNm; // name of the calibrated wv data branch or object: i.e. FPN sub, amp out, filtered, etc. (no default!)
   TString        fCorlNm;  // name of the output correlations array (default: CrossChanCorls.)
   
   // event data
   TSnCalWvData*  fDat;   //! the event data
   TObjArray*     fCorls; //! the output correlations of this event
   
 protected:
   virtual void  SlaveBegin();
   virtual void  Process();

 public:
   TSnChanTimeCorlMod() : fDat(0), fCorls(0) {}
   TSnChanTimeCorlMod(const Char_t* name, const Char_t* wvdatnm) :
      TSnCalDatModule(name, "chan time correlations mod"),
      fWvDatNm(wvdatnm),
      fCorlNm("CrossChanCorls."),
      fDat(0), fCorls(0) { // normal ctor
   }
   virtual ~TSnChanTimeCorlMod() {}
   
   static
   TString    GetChanCorlInterpName(const UChar_t ch, const UChar_t xc) {
      TString a = Form("CorlCh%dCh%dSpl3",ch,xc);
      return a;
   }
   
   TString    GetWvDataName() const { return fWvDatNm; }
   TString    GetChanCorlName() const { return fCorlNm; }
   
   void       SetWvDataName(const Char_t* n) { fWvDatNm = n; }
   void       SetChanCorlName(const Char_t* n) { fCorlNm = n; }
   
   ClassDef(TSnChanTimeCorlMod, 1); // find channel correlation vs sample functions
};

#endif // SNS_TSnChanTimeCorlMod
