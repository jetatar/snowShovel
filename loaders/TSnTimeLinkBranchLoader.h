#ifndef SNS_TSnTimeLinkBranchLoader
#define SNS_TSnTimeLinkBranchLoader

#include <TString.h>
class TTimeStamp;

#include "TSnLinkedTreeBranchLoader.h"

class TSnTimeLinkBranchLoader : public TSnLinkedTreeBranchLoader {
////////////////////////////////////////////////////////////////////////////////
//
// TSnTimeLinkBranchLoader
//
// Base class for loaders linking entries in two trees via a TTimeStamp.
// 
// Finds the entry in the linked tree that is connected in time to the
// time in main (event) tree. Two methods are available: "at or prior
// to" or "absolute closest".
//
// Note that the behavior depends on
// TTree::GetEntryNumberWithBestIndex, and if the behavior of that
// function changes, then so will this loader.
//
// Used by TSnVoltageTreeLoader and TSnHeartbeatTreeLoader.
//
////////////////////////////////////////////////////////////////////////////////
 public:
   enum EMethod_t {
      kPriorClosest,
      kAbsoluteClosest
   };

 private:
   TString          fMyBrNm;   // the time branch in the linked tree
   TString          fMainBrNm; // the time branch in the main tree
   TSnTimeLinkBranchLoader::EMethod_t fMeth; // the method used to match times (default: kAbsoluteClosest)
   
   //TTimeStamp*      fMyTime;   //! the time in the linked tree
   //TTimeStamp*      fMainTime; //! the time in the main tree

   TTimeStamp*      fMainCache; //! the previously used main timestamp 
   
 public:
   TSnTimeLinkBranchLoader() : /*fMyTime(0), fMainTime(0),*/ fMainCache(0) {}
   TSnTimeLinkBranchLoader(TAMBranchInfo* binfo,
                           const Char_t* linkedTreeName,
                           const Char_t* linkedTimeBranchName,
                           const Char_t* mainTimeBranchName,
                           const TSnTimeLinkBranchLoader::EMethod_t method) :
      TSnLinkedTreeBranchLoader(binfo, linkedTreeName),
      fMyBrNm(linkedTimeBranchName),
      fMainBrNm(mainTimeBranchName),
      fMeth(method),
	/*fMyTime(0), fMainTime(0),*/ fMainCache(0) {
      // normal ctor
   }
   virtual ~TSnTimeLinkBranchLoader();
   
   virtual TString GetMyBranchName() const { return fMyBrNm; }
   virtual TString GetMainBranchName() const { return fMainBrNm; }

   // need to do this because we can't load the address of just the
   // split part of the branch that has the time stamp -- the whole
   // branch has to have it address set and we don't know what kind
   // of object the full branch is
   virtual TTimeStamp* GetMyTime() const = 0;
   virtual TTimeStamp* GetMainTime() const = 0;
   virtual void        SetPointerToMyLinkData() = 0;
   virtual void        SetPointerToMainLinkData() = 0;
   virtual Int_t       BuildIndexOn(TTree* mytree);
   
   TSnTimeLinkBranchLoader::EMethod_t GetMethod() const { return fMeth; }
   
   void SetMethod(const TSnTimeLinkBranchLoader::EMethod_t& m) { fMeth = m; }
   
   virtual Int_t  GetEntry(Long64_t mainEntry);
   using TSnLinkedTreeBranchLoader::Notify;
   virtual Bool_t Notify(TTree* mainTree);
   
   ClassDef(TSnTimeLinkBranchLoader, 3); // base class for loaders that link via a TTimeStamp branch
};

#endif // SNS_TSnTimeLinkBranchLoader
