#ifndef SNS_TSnCommActHiRate
#define SNS_TSnCommActHiRate

class TString;

#include "TSnCommStatusAction.h"

class TSnCommObject;
class TSnCommActor;
class TSnDAQConfig;
class TSnRunInfo;

class TSnCommActHiRate : public TSnCommStatusAction {
 public:
   static const Char_t* kTrgOffConfLbl; // the label of the emergency config

 private:
   Float_t   fThmRateCut; // (exclusive) max trigger rate (default 10kHz)
   Float_t   fEvtRateCut; // (exclusive) max event saving rate (default 10Hz)
   
   static
   TSnCommObject* NewThermsOffConfPack(const TString& stnlbl,
                                       const Int_t svers);
   static
   void SetDAQConfig(TSnDAQConfig& conf, const TString& clbl,
                     const Int_t svers);
   static
   void SetRunInfo(TSnRunInfo& runf, const TString& clbl, 
                   const TString& stnlbl, const Int_t svers);

   virtual TSnCommObject* operator()(const TSnCommIncStatus& is);
   
 public:
   TSnCommActHiRate() :
      fThmRateCut(10.0e3),
      fEvtRateCut(10.0) {}
   TSnCommActHiRate(TSnCommActor* a) :
      TSnCommStatusAction(a),
      fThmRateCut(10.0e3),
      fEvtRateCut(10.0) {}
   virtual ~TSnCommActHiRate() {}

   Float_t GetThmRateCut() const { return fThmRateCut; }
   Float_t GetEvtRateCut() const { return fEvtRateCut; }
   virtual TString        GetLabel() const { return "Check For High Rates"; }
   
   void    SetThmRateCut(const Float_t c) { fThmRateCut = c; }
   void    SetEvtRateCut(const Float_t c) { fEvtRateCut = c; }

   ClassDef(TSnCommActHiRate, 1); // check for high rates and sends a thermals-off config if so
};

#endif // SNS_TSnCommActHiRate
