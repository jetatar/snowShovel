#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TFile.h>
#include <TChain.h>

#include "TSnSaveCalibDataMod.h"
#include "TSnRawTreeMaker.h"
#endif

Bool_t tryLoadTree(const Char_t* treename,
                   const Char_t* treefns,
                   TChain*&      ntree) {
   delete ntree;
   ntree = new TChain(treename);
   ntree->Add(treefns);
   return (ntree->GetEntries()>0);
}

Bool_t tryLoadDataTree(const Char_t* treefns,
                       TChain*& ntree,
                       TString* dbrn=0) {
   // this function MUST be compiled in order to work
   // (i.e. .x mmmmmmm.C+()  and NOT .x mmmmmmm.C() )
   //
   // the pointer passed in will be set to either
   // 1) the CalibTree
   // 2) the FpnSubTree
   // 3) the RawDataTree
   // (in that order) from the specified files
   //
   // the pointer should be a null pointer to avoid memory leaks
   //
   // if dbrn is non-null, the string will be set to the name of the
   // data branch appropriate for the chosen tree
   
   Bool_t ok = (ntree==0);
   if (ok) {
      static const Int_t   nts   = 3;
      static const Char_t* tns[] = {
         TSnSaveCalibDataMod::kCalEvtTreeNm,
         TSnSaveCalibDataMod::kFpnEvtTreeNm,
         TSnRawTreeMaker::kETname
      };
      static const Char_t* bns[] = {
         TSnSaveCalibDataMod::kAmpOutDatBrNm,
         TSnSaveCalibDataMod::kFPNSubDatBrNm,
         TSnRawTreeMaker::kEWvBrNm
      };
      
      ok = kFALSE;
      const Char_t** tn = &(tns[0]);
      for (Int_t i=0; (i<nts && (ok==kFALSE)); ++i, ++tn) {
         ok = tryLoadTree(*tn, treefns, ntree);
         if (ok && (dbrn!=0)) {
            *dbrn = bns[i];
         }
      }
   } else {
      Error("tryLoadDataTree","Tree pointer must be null.");
   }
   return ok;
}
