#ifndef SNS_TSnCalDatModule
#define SNS_TSnCalDatModule

class TMap;

#include "TAModule.h"

class TSnCalWvData;

class TSnCalDatModule : public TAModule {
 public:
   typedef Bool_t (TSnCalDatModule::*IsBranchFcn)(const Char_t* bname);
   
   struct AdrWrap_t : TObject {
      TSnCalWvData** fAdr;
      AdrWrap_t(TSnCalWvData** a=0) : fAdr(a) {}
      virtual ~AdrWrap_t() {}
      ClassDef(TSnCalDatModule::AdrWrap_t, 1); // wrapper for a pointer to a pointer to a TSnCalWvData
   };
   
 protected:
   TMap*               fNmToCalDat; // the calibrated/processed data from the tree or evt obj, indexed by its name (this allows reading multiple branches or objects)
   IsBranchFcn         fIsBr;       // function to use to check if the cal/proc data obj is in a branch or evt obj (default: IsFpnSubOrAmpOutBr)
   
   TSnCalDatModule() :
      fNmToCalDat(0), fIsBr(&TSnCalDatModule::IsFpnSubOrAmpOutBr) {}
   TSnCalDatModule(const Char_t* name, const Char_t* title);
   
   virtual void ReqCalData(const Char_t* bname, TSnCalWvData*& address);
   virtual void LoadCalData(const Char_t* bname);

 public:
   virtual ~TSnCalDatModule();

   Bool_t  IsFpnSubOrAmpOutBr(const Char_t* bname);
   Bool_t  IsTreeBranch(const Char_t* bname);
   
   void    SetUseIsFpnSubOrAmpOutBr()
      { fIsBr = &TSnCalDatModule::IsFpnSubOrAmpOutBr; }
   void    SetUseIsTreeBranch()
      { fIsBr = &TSnCalDatModule::IsTreeBranch; }
   
   ClassDef(TSnCalDatModule, 1); // base class for modules wanting to load calibrated (fpn sub, gain cal or processed data)
};

#endif // SNS_TSnCalDatModule
