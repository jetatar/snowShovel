#include "TSnCalDatModule.h"

#include <TMap.h>
#include <TString.h>
#include <TTree.h>

#include "TSnCalWvData.h"
#include "TSnSaveCalibDataMod.h"

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

ClassImp(TSnCalDatModule);
ClassImp(TSnCalDatModule::AdrWrap_t);

TSnCalDatModule::TSnCalDatModule(const Char_t* name, const Char_t* title):
   TAModule(name, title),
   fNmToCalDat(new TMap),
   fIsBr(&TSnCalDatModule::IsFpnSubOrAmpOutBr) {
   // normal ctor
   fNmToCalDat->SetOwnerKeyValue(kTRUE,kTRUE);
}

TSnCalDatModule::~TSnCalDatModule() {
   delete fNmToCalDat;
}

Bool_t TSnCalDatModule::IsFpnSubOrAmpOutBr(const Char_t* bname) {
   // check if bname ends with the FPN sub or amp out branch names.
   // this should allow it to work around any alternate tree prefix tags.
   TString b(bname);
   return (b.EndsWith(TSnSaveCalibDataMod::kFPNSubDatBrNm) ||
           b.EndsWith(TSnSaveCalibDataMod::kAmpOutDatBrNm));
}

Bool_t TSnCalDatModule::IsTreeBranch(const Char_t* bname) {
   // this may be slow. explicitly check if this is a branch in the
   // current tree.
   // this won't work with any speical loaders for the branch
   //
   // this may not work if the tree isn't available early enough
   const TAMSelector* sel = GetSelector();
   if (sel!=0) {
      const TTree*    t   = sel->GetTree();
      if (t!=0) {
         // TTree doesn't have a const GetBranch or FindBranch,
         // so this is the closest thing.
         return t->GetBranchStatus(bname);
      }
   } else {
      SendError(kAbortAnalysis, "IsTreeBranch",
                "Could not get selector!");
   }
   return kFALSE;
}

void TSnCalDatModule::ReqCalData(const Char_t* bname, TSnCalWvData*& address) {
   if (CALL_MEMBER_FN(*this,fIsBr)(bname)) {
      ReqBranch(bname, address);
   } // otherwise it's an obj that will be added to the event
   
   if ( fNmToCalDat->FindObject(bname)==0 ) {
      fNmToCalDat->Add(new TObjString(bname),
                       new TSnCalDatModule::AdrWrap_t(&address));
   } else {
      SendError(kWarning, "ReqCalData",
                "Ignoring duplicate request for "
                "data [%s].", bname);
   }
}



void TSnCalDatModule::LoadCalData(const Char_t* bname) {
   if (CALL_MEMBER_FN(*this,fIsBr)(bname)) {
      LoadBranch(bname);
   } else {
      const TSnCalDatModule::AdrWrap_t* a = 
         dynamic_cast<const TSnCalDatModule::AdrWrap_t*>(
            fNmToCalDat->GetValue(bname));
      if (a!=0) {
         *(a->fAdr) = dynamic_cast<TSnCalWvData*>( FindObjThisEvt(bname) );
      } else {
         SendError(kAbortModule, "LoadCalData",
                   "Could not find address for cal data "
                   "[%s]. Must use ReqCalData first!",
                   bname);
      }
   }
}
